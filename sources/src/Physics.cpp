/*
MIT License

Copyright (c) 2020 Andrey Vasiliev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "pcheader.h"
#include "Physics.h"

namespace xio {
//----------------------------------------------------------------------------------------------------------------------
Physics::Physics() {
  broadphase_ = std::make_unique<btAxisSweep3>(btVector3(-1000, -1000, -1000), btVector3(1000, 1000, 1000), 1024);
  collision_config_ = std::make_unique<btDefaultCollisionConfiguration>();
  dispatcher_ = std::make_unique<btCollisionDispatcher>(collision_config_.get());
  solver_ = std::make_unique<btSequentialImpulseConstraintSolver>();
  phy_world_ = std::make_unique<btDiscreteDynamicsWorld>(dispatcher_.get(),
                                                         broadphase_.get(),
                                                         solver_.get(),
                                                         collision_config_.get());

  phy_world_->setGravity(btVector3(0.0, -9.8, 0.0));

  if (physics_debug_show_collider_) {
    dbg_draw_ = std::make_unique<BtOgre::DebugDrawer>(Ogre::Root::getSingleton().getSceneManager("Default")->getRootSceneNode(), phy_world_.get());
    dbg_draw_->setDebugMode(physics_debug_show_collider_);
    phy_world_->setDebugDrawer(dbg_draw_.get());
  }

  pause_ = false;
}
//----------------------------------------------------------------------------------------------------------------------
Physics::~Physics() {}
//----------------------------------------------------------------------------------------------------------------------
void Physics::Update(float time) {
  if (!pause_) {
    phy_world_->stepSimulation(time, sub_steps_);
  }

  if (physics_debug_show_collider_)
    dbg_draw_->step();
}
//----------------------------------------------------------------------------------------------------------------------
void Physics::Clear() {
  Pause();
  phy_world_->clearForces();

  //remove the rigidbodies from the dynamics world and delete them
  for (int i = phy_world_->getNumCollisionObjects() - 1; i >= 0; i--) {
    btCollisionObject *obj = phy_world_->getCollisionObjectArray()[i];
    phy_world_->removeCollisionObject(obj);
    delete obj;
  }

  //delete collision shapes
  for (int j = 0; j < collision_shapes_.size(); j++) {
    btCollisionShape *shape = collision_shapes_[j];
    delete shape;
  }

  collision_shapes_.clear();
  rigid_bodies_.clear();
}
//----------------------------------------------------------------------------------------------------------------------
void Physics::AddRigidBody(btRigidBody *body) {
  phy_world_->addRigidBody(body);
}
//----------------------------------------------------------------------------------------------------------------------
void Physics::CreateTerrainHeightfieldShape(int size,
                                            float *data,
                                            const float &min_height,
                                            const float &max_height,
                                            const Ogre::Vector3 &position,
                                            const float &scale) {
  // Convert height data in a format suitable for the physics engine
  auto *terrainHeights = new float[size * size];
  assert(terrainHeights != 0);

  for (int i = 0; i < size; i++) {
    memcpy(terrainHeights + size * i, data + size * (size - i - 1), sizeof(float) * size);
  }

  const btScalar heightScale = 1.0f;

  btVector3 localScaling(scale, heightScale, scale);

  auto *terrainShape = new btHeightfieldTerrainShape(size,
                                                     size,
                                                     terrainHeights,
                                                     1,
                                                     min_height,
                                                     max_height,
                                                     1,
                                                     PHY_FLOAT,
                                                     false);

  terrainShape->setUseDiamondSubdivision(true);
  terrainShape->setLocalScaling(localScaling);

  auto *groundMotionState =
      new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(size / 2, 0, size / 2)));
  btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, terrainShape, btVector3(0, 0, 0));

  //Create Rigid Body using 0 mass so it is static
  auto *entBody = new btRigidBody(groundRigidBodyCI);

  entBody->setFriction(0.8f);
  entBody->setHitFraction(0.8f);
  entBody->setRestitution(0.6f);
  entBody->getWorldTransform().setOrigin(btVector3(position.x,
                                                   position.y + (max_height - min_height) / 2 - 0.5,
                                                   position.z));
  entBody->getWorldTransform().setRotation(BtOgre::Convert::toBullet(Ogre::Quaternion::IDENTITY));
  entBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);

  AddRigidBody(entBody);
  phy_world_->setForceUpdateAllAabbs(false);
}
//----------------------------------------------------------------------------------------------------------------------
void Physics::ProcessData(Ogre::UserObjectBindings &user_object_bindings,
                          Ogre::Entity *entity,
                          Ogre::SceneNode *parent_node) {
  const std::string physics_type_static = "STATIC";
  const std::string physics_type_dynamic = "dynamic";
  const std::string physics_type_actor = "actor";
  const std::string physics_type_ghost = "ghost";
  const std::string physics_type_none = "none";
  const std::string proxy_box = "box";
  const std::string proxy_capsule = "capsule";
  const std::string proxy_sphere = "sphere";
  const std::string proxy_cylinder = "cylinder";
  const std::string proxy_trimesh = "trimesh";
  const std::string proxy_convex = "convex";

  std::string proxy_type;
  if (user_object_bindings.getUserAny("proxy").has_value())
    proxy_type = Ogre::any_cast<std::string>(user_object_bindings.getUserAny("proxy"));

  std::string physics_type = Ogre::any_cast<std::string>(user_object_bindings.getUserAny("physics_type"));
  float mass = Ogre::any_cast<float>(user_object_bindings.getUserAny("mass"));
  float mass_radius = Ogre::any_cast<float>(user_object_bindings.getUserAny("mass_radius"));
  float inertia_tensor = Ogre::any_cast<float>(user_object_bindings.getUserAny("inertia_tensor"));
  float velocity_min = Ogre::any_cast<float>(user_object_bindings.getUserAny("velocity_min"));
  float velocity_max = Ogre::any_cast<float>(user_object_bindings.getUserAny("velocity_max"));
  bool lock_trans_x = Ogre::any_cast<bool>(user_object_bindings.getUserAny("lock_trans_x"));
  bool lock_trans_y = Ogre::any_cast<bool>(user_object_bindings.getUserAny("lock_trans_y"));
  bool lock_trans_z = Ogre::any_cast<bool>(user_object_bindings.getUserAny("lock_trans_z"));
  bool lock_rot_x = Ogre::any_cast<bool>(user_object_bindings.getUserAny("lock_rot_x"));
  bool lock_rot_y = Ogre::any_cast<bool>(user_object_bindings.getUserAny("lock_rot_y"));
  bool lock_rot_z = Ogre::any_cast<bool>(user_object_bindings.getUserAny("lock_rot_z"));
  bool anisotropic_friction = Ogre::any_cast<bool>(user_object_bindings.getUserAny("anisotropic_friction"));
  float friction_x = Ogre::any_cast<float>(user_object_bindings.getUserAny("friction_x"));
  float friction_y = Ogre::any_cast<float>(user_object_bindings.getUserAny("friction_y"));
  float friction_z = Ogre::any_cast<float>(user_object_bindings.getUserAny("friction_z"));
  float damping_trans = Ogre::any_cast<float>(user_object_bindings.getUserAny("damping_trans"));
  float damping_rot = Ogre::any_cast<float>(user_object_bindings.getUserAny("damping_rot"));
  btRigidBody *entBody = nullptr;

  if (physics_type == physics_type_static) {
    std::unique_ptr<BtOgre::StaticMeshToShapeConverter> converter;

    if (entity->getNumManualLodLevels() > 0)
      converter = std::make_unique<BtOgre::StaticMeshToShapeConverter>(entity->getManualLodLevel(entity->getNumManualLodLevels() - 1));
    else
      converter = std::make_unique<BtOgre::StaticMeshToShapeConverter>(entity);

    if (proxy_type == proxy_capsule) {
      auto *entShape = converter->createCapsule();

      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(0, bodyState, entShape, btVector3(0, 0, 0));
    } else if (proxy_type == proxy_box) {
      auto *entShape = converter->createBox();

      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(0, bodyState, entShape, btVector3(0, 0, 0));
    } else if (proxy_type == proxy_sphere) {
      auto *entShape = converter->createSphere();

      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(0, bodyState, entShape, btVector3(0, 0, 0));
    } else if (proxy_type == proxy_cylinder) {
      auto *entShape = converter->createCylinder();

      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(0, bodyState, entShape, btVector3(0, 0, 0));
    } else if (proxy_type == proxy_trimesh) {
      auto *entShape = converter->createTrimesh();

      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(0, bodyState, entShape, btVector3(0, 0, 0));
    } else if (proxy_type == proxy_convex) {
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
  } else if (physics_type == physics_type_dynamic) {
    std::unique_ptr<BtOgre::StaticMeshToShapeConverter> converter;
    btVector3 inertia;

    if (entity->getNumManualLodLevels() > 0) {
      converter = std::make_unique<BtOgre::StaticMeshToShapeConverter>(entity->getManualLodLevel(
          entity->getNumManualLodLevels() - 1));

      auto *entShape = converter->createConvex();
      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);

    } else {
      converter = std::make_unique<BtOgre::StaticMeshToShapeConverter>(entity);
    }

    if (proxy_type == proxy_capsule) {
      auto *entShape = converter->createCapsule();

      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type == proxy_box) {
      auto *entShape = converter->createBox();

      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type == proxy_sphere) {
      auto *entShape = converter->createSphere();

      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type == proxy_cylinder) {
      auto *entShape = converter->createCylinder();

      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type == proxy_trimesh) {
      auto *entShape = converter->createTrimesh();

      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type == proxy_convex) {
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
  } else if (physics_type == physics_type_ghost) {
    std::unique_ptr<BtOgre::StaticMeshToShapeConverter> converter;
    btVector3 inertia;

    if (entity->getNumManualLodLevels() > 0) {
      converter = std::make_unique<BtOgre::StaticMeshToShapeConverter>(entity->getManualLodLevel(
          entity->getNumManualLodLevels() - 1));
    } else {
      converter = std::make_unique<BtOgre::StaticMeshToShapeConverter>(entity);
    }
    if (proxy_type == proxy_capsule) {
      auto *entShape = converter->createCapsule();

      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type == proxy_box) {
      auto *entShape = converter->createBox();

      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type == proxy_sphere) {
      auto *entShape = converter->createSphere();

      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type == proxy_cylinder) {
      auto *entShape = converter->createCylinder();

      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type == proxy_trimesh) {
      auto *entShape = converter->createTrimesh();

      entShape->calculateLocalInertia(mass, inertia);
      auto *bodyState = new BtOgre::RigidBodyState(parent_node);
      entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    } else if (proxy_type == proxy_convex) {
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

    AddRigidBody(entBody);
  }
}
}