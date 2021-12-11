// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Physics.h"
#include "BtOgre/BtOgre.h"
#include <BulletCollision/CollisionDispatch/btCollisionDispatcherMt.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolverMt.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.h>

using namespace std;

namespace Glue {

Physics::Physics(bool threaded) : threaded(threaded) {
  auto *sheduler = btCreateDefaultTaskScheduler();

  if (!sheduler) throw Exception("Bullet physics no task sheduler available");

  btSetTaskScheduler(sheduler);
  broadphase = make_unique<btDbvtBroadphase>();
  config = make_unique<btDefaultCollisionConfiguration>();
  dispatcher = make_unique<btCollisionDispatcherMt>(config.get(), 40);
  solver = make_unique<btSequentialImpulseConstraintSolverMt>();

  btConstraintSolverPoolMt *solverPool = new btConstraintSolverPoolMt(BT_MAX_THREAD_COUNT);

  world = make_unique<btDiscreteDynamicsWorldMt>(dispatcher.get(), broadphase.get(), solverPool, solver.get(),
                                                 config.get());

  world->setGravity(btVector3(0.0, -9.8, 0.0));

  if (debug) {
    auto *node = Ogre::Root::getSingleton().getSceneManager("Default")->getRootSceneNode();
    dbg_draw = make_unique<BtOgre::DebugDrawer>(node, world.get());
    dbg_draw->setDebugMode(debug);
    world->setDebugDrawer(dbg_draw.get());
  }

  running = true;
  pause = false;
}

Physics::~Physics() {
  running = false;

  Cleanup();
}

void Physics::Resume() { pause = false; }

void Physics::Pause() { pause = true; }

void Physics::Update(float time) {
  if (threaded) return;

  if (pause) return;

  world->stepSimulation(time, steps, 1.0f / update_rate);

  //if (debug) dbg_draw->update();
}

void Physics::Cleanup() {
  world->clearForces();

  // remove the rigidbodies from the dynamics world and delete them
  for (int i = world->getNumCollisionObjects() - 1; i >= 0; i--) {
    btCollisionObject *obj = world->getCollisionObjectArray()[i];
    world->removeCollisionObject(obj);
    delete obj;
  }

  for (int i = world->getNumConstraints() - 1; i >= 0; i--) {
    btTypedConstraint *constraint = world->getConstraint(i);
    world->removeConstraint(constraint);
    delete constraint;
  }
}

void Physics::AddRigidBody(btRigidBody *body) { world->addRigidBody(body); }

void Physics::CreateTerrainHeightfieldShape(int size, float *data, const float &min_height,
                                                  const float &max_height, const Ogre::Vector3 &position,
                                                  const float &scale) {
  // Convert height data in a format suitable for the physics engine
  auto *terrainHeights = new float[size * size];
  assert(terrainHeights != 0);

  for (int i = 0; i < size; i++) {
    memcpy(terrainHeights + size * i, data + size * (size - i - 1), sizeof(float) * size);
  }

  const btScalar heightScale = 1.0f;

  btVector3 localScaling(scale, heightScale, scale);

  auto *terrainShape =
      new btHeightfieldTerrainShape(size, size, terrainHeights, 1, min_height, max_height, 1, PHY_FLOAT, false);

  terrainShape->setUseDiamondSubdivision(true);
  terrainShape->setLocalScaling(localScaling);

  auto *groundMotionState =
      new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(size / 2, 0, size / 2)));
  btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, terrainShape, btVector3(0, 0, 0));

  // SetUp Rigid Body using 0 mass so it is static
  auto *entBody = new btRigidBody(groundRigidBodyCI);

  entBody->setFriction(0.8f);
  entBody->setHitFraction(0.8f);
  entBody->setRestitution(0.6f);
  entBody->getWorldTransform().setOrigin(
      btVector3(position.x, position.y + (max_height - min_height) / 2 - 0.5, position.z));
  entBody->getWorldTransform().setRotation(BtOgre::Convert::toBullet(Ogre::Quaternion::IDENTITY));
  entBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);

  entBody->setUserIndex(0);
  AddRigidBody(entBody);
  world->setForceUpdateAllAabbs(false);
}

void Physics::ProcessData(Ogre::Entity *entity, Ogre::SceneNode *parent_node, const string &proxy_type,
                                const string &physics_type, float mass, float mass_radius, float inertia_tensor,
                                float velocity_min, float velocity_max, float friction) {
  btRigidBody *entBody = nullptr;

  if (physics_type == TYPE_STATIC) {
    unique_ptr<BtOgre::StaticMeshToShapeConverter> converter;

    if (entity->getNumManualLodLevels() > 0)
      converter = make_unique<BtOgre::StaticMeshToShapeConverter>(
          entity->getManualLodLevel(entity->getNumManualLodLevels() - 1));
    else
      converter = make_unique<BtOgre::StaticMeshToShapeConverter>(entity);

    if (proxy_type == PROXY_CAPSULE) {
      auto *entShape = converter->createCapsule();
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(0, bodyState, entShape, btVector3(0, 0, 0));
    } else if (proxy_type == PROXY_BOX) {
      auto *entShape = converter->createBox();
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(0, bodyState, entShape, btVector3(0, 0, 0));
    } else if (proxy_type == PROXY_SPHERE) {
      auto *entShape = converter->createSphere();
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(0, bodyState, entShape, btVector3(0, 0, 0));
    } else if (proxy_type == PROXY_CYLINDER) {
      auto *entShape = converter->createCylinder();
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(0, bodyState, entShape, btVector3(0, 0, 0));
    } else if (proxy_type == PROXY_TRIMESH) {
      auto *entShape = converter->createTrimesh();
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(0, bodyState, entShape, btVector3(0, 0, 0));
    } else if (proxy_type == PROXY_CONVEX) {
      auto *entShape = converter->createConvex();
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(0, bodyState, entShape, btVector3(0, 0, 0));
    } else if (proxy_type.empty()) {
      auto *entShape = converter->createConvex();
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(0, bodyState, entShape, btVector3(0, 0, 0));
    } else {
      auto *entShape = converter->createConvex();
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(0, bodyState, entShape, btVector3(0, 0, 0));
    }
  } else if (physics_type == TYPE_DYNAMIC) {
    unique_ptr<BtOgre::StaticMeshToShapeConverter> converter;
    btVector3 inertia;

    if (entity->getNumManualLodLevels() > 0) {
      converter = make_unique<BtOgre::StaticMeshToShapeConverter>(
          entity->getManualLodLevel(entity->getNumManualLodLevels() - 1));
      auto *entShape = converter->createConvex();
      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else {
      converter = make_unique<BtOgre::StaticMeshToShapeConverter>(entity);
    }

    if (proxy_type == PROXY_CAPSULE) {
      auto *entShape = converter->createCapsule();
      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type == PROXY_BOX) {
      auto *entShape = converter->createBox();
      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type == PROXY_SPHERE) {
      auto *entShape = converter->createSphere();
      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type == PROXY_CYLINDER) {
      auto *entShape = converter->createCylinder();
      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type == PROXY_TRIMESH) {
      auto *entShape = converter->createTrimesh();
      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type == PROXY_CONVEX) {
      auto *entShape = converter->createConvex();
      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type.empty()) {
      auto *entShape = converter->createConvex();
      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else {
      auto *entShape = converter->createConvex();
      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    }
  } else if (physics_type == TYPE_GHOST) {
    unique_ptr<BtOgre::StaticMeshToShapeConverter> converter;
    btVector3 inertia;

    if (entity->getNumManualLodLevels() > 0) {
      converter = make_unique<BtOgre::StaticMeshToShapeConverter>(
          entity->getManualLodLevel(entity->getNumManualLodLevels() - 1));
    } else {
      converter = make_unique<BtOgre::StaticMeshToShapeConverter>(entity);
    }
    if (proxy_type == PROXY_CAPSULE) {
      auto *entShape = converter->createCapsule();

      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type == PROXY_BOX) {
      auto *entShape = converter->createBox();

      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type == PROXY_SPHERE) {
      auto *entShape = converter->createSphere();

      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type == PROXY_CYLINDER) {
      auto *entShape = converter->createCylinder();

      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type == PROXY_TRIMESH) {
      auto *entShape = converter->createTrimesh();

      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type == PROXY_CONVEX) {
      auto *entShape = converter->createConvex();

      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type.empty()) {
      auto *entShape = converter->createConvex();

      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else {
      auto *entShape = converter->createConvex();

      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
      entBody->setAngularFactor(0);
    }
  }

  if (entBody) {
    //    if (anisotropic_friction) {
    //      entBody->setAnisotropicFriction({friction_x, friction_y,
    //      friction_z});
    //    } else {
    //      entBody->setFriction(friction_x);
    //    }
    //    entBody->setUserIndex(0);
    AddRigidBody(entBody);
  }
}

void Physics::ProcessData(Ogre::UserObjectBindings &user_object_bindings, Ogre::Entity *entity,
                                Ogre::SceneNode *parent_node) {
  string proxy_type;
  if (user_object_bindings.getUserAny("proxy").has_value())
    proxy_type = Ogre::any_cast<string>(user_object_bindings.getUserAny("proxy"));

  string physics_type = Ogre::any_cast<string>(user_object_bindings.getUserAny("physics_type"));
  float mass = Ogre::any_cast<float>(user_object_bindings.getUserAny("mass"));
  //  float mass_radius = Ogre::any_cast<float>(user_object_bindings.getUserAny("mass_radius"));
  //  float inertia_tensor = Ogre::any_cast<float>(user_object_bindings.getUserAny("inertia_tensor"));
  //  float velocity_min = Ogre::any_cast<float>(user_object_bindings.getUserAny("velocity_min"));
  //  float velocity_max = Ogre::any_cast<float>(user_object_bindings.getUserAny("velocity_max"));
  //  bool lock_trans_x = Ogre::any_cast<bool>(user_object_bindings.getUserAny("lock_trans_x"));
  //  bool lock_trans_y = Ogre::any_cast<bool>(user_object_bindings.getUserAny("lock_trans_y"));
  //  bool lock_trans_z = Ogre::any_cast<bool>(user_object_bindings.getUserAny("lock_trans_z"));
  //  bool lock_rot_x = Ogre::any_cast<bool>(user_object_bindings.getUserAny("lock_rot_x"));
  //  bool lock_rot_y = Ogre::any_cast<bool>(user_object_bindings.getUserAny("lock_rot_y"));
  //  bool lock_rot_z = Ogre::any_cast<bool>(user_object_bindings.getUserAny("lock_rot_z"));
  bool anisotropic_friction = Ogre::any_cast<bool>(user_object_bindings.getUserAny("anisotropic_friction"));
  float friction_x = Ogre::any_cast<float>(user_object_bindings.getUserAny("friction_x"));
  float friction_y = Ogre::any_cast<float>(user_object_bindings.getUserAny("friction_y"));
  float friction_z = Ogre::any_cast<float>(user_object_bindings.getUserAny("friction_z"));
  //  float damping_trans = Ogre::any_cast<float>(user_object_bindings.getUserAny("damping_trans"));
  //  float damping_rot = Ogre::any_cast<float>(user_object_bindings.getUserAny("damping_rot"));
  btRigidBody *entBody = nullptr;

  if (physics_type == TYPE_STATIC) {
    unique_ptr<BtOgre::StaticMeshToShapeConverter> converter;

//    if (proxy_type.find(".mesh") != string::npos) {
//      Ogre::MeshManager::getSingleton().load(proxy_type, Ogre::RGN_AUTODETECT);
//      auto mesh = Ogre::MeshManager::getSingleton().getByName(proxy_type);
//      converter = make_unique<BtOgre::StaticMeshToShapeConverter>(entity, mesh);
//      proxy_type = PROXY_TRIMESH;
//    } else {
      converter = make_unique<BtOgre::StaticMeshToShapeConverter>(entity);
//    }

    if (proxy_type == PROXY_CAPSULE) {
      auto *entShape = converter->createCapsule();
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(0, bodyState, entShape, btVector3(0, 0, 0));
    } else if (proxy_type == PROXY_BOX) {
      auto *entShape = converter->createBox();
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(0, bodyState, entShape, btVector3(0, 0, 0));
    } else if (proxy_type == PROXY_SPHERE) {
      auto *entShape = converter->createSphere();
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(0, bodyState, entShape, btVector3(0, 0, 0));
    } else if (proxy_type == PROXY_CYLINDER) {
      auto *entShape = converter->createCylinder();
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(0, bodyState, entShape, btVector3(0, 0, 0));
    } else if (proxy_type == PROXY_TRIMESH) {
      auto *entShape = converter->createTrimesh();
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(0, bodyState, entShape, btVector3(0, 0, 0));
    } else if (proxy_type == PROXY_CONVEX) {
      auto *entShape = converter->createConvex();
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(0, bodyState, entShape, btVector3(0, 0, 0));
    } else if (proxy_type.empty()) {
      auto *entShape = converter->createConvex();
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(0, bodyState, entShape, btVector3(0, 0, 0));
    } else {
      auto *entShape = converter->createConvex();
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(0, bodyState, entShape, btVector3(0, 0, 0));
    }

    entBody->setAngularFactor(0);
  } else if (physics_type == TYPE_DYNAMIC) {
    unique_ptr<BtOgre::StaticMeshToShapeConverter> converter;
    btVector3 inertia;

//    if (proxy_type.find(".mesh") != string::npos) {
//      Ogre::MeshManager::getSingleton().load(proxy_type, Ogre::RGN_AUTODETECT);
//      auto mesh = Ogre::MeshManager::getSingleton().getByName(proxy_type);
//      converter = make_unique<BtOgre::StaticMeshToShapeConverter>(entity, mesh);
//      proxy_type = PROXY_TRIMESH;
//    } else {
//      if (entity->getMesh()->getNumLodLevels() > 0) {
//        auto lod = entity->getMesh()->getLodLevel(entity->getMesh()->getNumLodLevels() - 1).manualMesh;
//        Ogre::MeshManager::getSingleton().load("Cube.mesh", Ogre::RGN_DEFAULT);
//        converter = make_unique<BtOgre::StaticMeshToShapeConverter>(entity, lod);
//      } else {
        converter = make_unique<BtOgre::StaticMeshToShapeConverter>(entity);
//      }
//    }
    if (proxy_type == PROXY_CAPSULE) {
      auto *entShape = converter->createCapsule();
      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type == PROXY_BOX) {
      auto *entShape = converter->createBox();
      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type == PROXY_SPHERE) {
      auto *entShape = converter->createSphere();
      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type == PROXY_CYLINDER) {
      auto *entShape = converter->createCylinder();
      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type == PROXY_TRIMESH) {
      auto *entShape = converter->createTrimesh();
      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type == PROXY_CONVEX) {
      auto *entShape = converter->createConvex();
      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type.empty()) {
      auto *entShape = converter->createConvex();
      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else {
      auto *entShape = converter->createConvex();
      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    }
  } else if (physics_type == TYPE_GHOST) {
    unique_ptr<BtOgre::StaticMeshToShapeConverter> converter;
    btVector3 inertia;

    if (entity->getNumManualLodLevels() > 0) {
      converter = make_unique<BtOgre::StaticMeshToShapeConverter>(
          entity->getManualLodLevel(entity->getNumManualLodLevels() - 1));
    } else {
      converter = make_unique<BtOgre::StaticMeshToShapeConverter>(entity);
    }
    if (proxy_type == PROXY_CAPSULE) {
      auto *entShape = converter->createCapsule();

      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type == PROXY_BOX) {
      auto *entShape = converter->createBox();

      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type == PROXY_SPHERE) {
      auto *entShape = converter->createSphere();

      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type == PROXY_CYLINDER) {
      auto *entShape = converter->createCylinder();

      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type == PROXY_TRIMESH) {
      auto *entShape = converter->createTrimesh();

      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type == PROXY_CONVEX) {
      auto *entShape = converter->createConvex();

      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type.empty()) {
      auto *entShape = converter->createConvex();

      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else {
      auto *entShape = converter->createConvex();

      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
      entBody->setAngularFactor(0);
    }
  }

  if (entBody) {
    if (anisotropic_friction) {
      entBody->setAnisotropicFriction({friction_x, friction_y, friction_z});
    } else {
      entBody->setFriction(friction_x);
    }

    //    entBody->setUserIndex(0);
    AddRigidBody(entBody);
  }
}

}  // namespace Glue
