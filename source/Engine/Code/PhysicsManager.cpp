/// created by Andrey VasilievBullet

#include "pch.h"
#include "PhysicsManager.h"
#include <Bullet/OgreBullet.h>
#include <BulletCollision/CollisionDispatch/btCollisionDispatcherMt.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolverMt.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.h>

using namespace std;

namespace Glue {

PhysicsManager::PhysicsManager() : sleep(false), threaded(false), updateRate(60), SubSteps(4) {
  auto *Scheduler = btCreateDefaultTaskScheduler();
  if (!Scheduler) throw std::runtime_error("Bullet physics no task scheduler available");
  btSetTaskScheduler(Scheduler);
  btBroadphase = make_unique<btDbvtBroadphase>();
  btConfig = make_unique<btDefaultCollisionConfiguration>();

#if (OGRE_THREAD_SUPPORT > 0)
  btDispatcher = make_unique<btCollisionDispatcherMt>(btConfig.get());
  btSolver = make_unique<btSequentialImpulseConstraintSolverMt>();
  auto *solverPool = new btConstraintSolverPoolMt(BT_MAX_THREAD_COUNT);
  btWorld = make_unique<btDiscreteDynamicsWorldMt>(btDispatcher.get(), btBroadphase.get(), solverPool, btSolver.get(), btConfig.get());
#else
  btDispatcher = make_unique<btCollisionDispatcher>(btConfig.get());
  btSolver = make_unique<btSequentialImpulseConstraintSolver>();
  btWorld = make_unique<btDiscreteDynamicsWorld>(btDispatcher.get(), btBroadphase.get(), btSolver.get(), btConfig.get());
#endif

  btWorld->setGravity(btVector3(0.0, -9.8, 0.0));
  if (threaded) InitThread();
  sleep = false;
}

void PhysicsManager::InitThread() {
  if (!threaded) return;

  static int64_t time_of_last_frame_ = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();

  function<void(void)> update_func_ = [&]() {
    while (!sleep) {
      auto before_update = chrono::system_clock::now().time_since_epoch();
      int64_t micros_before_update = chrono::duration_cast<chrono::microseconds>(before_update).count();
      float frame_time = static_cast<float>(micros_before_update - time_of_last_frame_) / 1e+6;
      time_of_last_frame_ = micros_before_update;

      // Actually do calculations
      if (!sleep) {
        btWorld->stepSimulation(frame_time, SubSteps, 1.0f / updateRate);
      }
      // Actually do calculations

      auto after_update = chrono::system_clock::now().time_since_epoch();
      int64_t micros_after_update = chrono::duration_cast<chrono::microseconds>(after_update).count();

      auto update_time = micros_after_update - micros_before_update;

      auto delay = static_cast<int64_t>((1e+6 / updateRate) - update_time);
      if (delay > 0) this_thread::sleep_for(chrono::microseconds(delay));
    }
  };

  updateThread = make_unique<thread>(update_func_);
  updateThread->detach();
}

PhysicsManager::~PhysicsManager() {}

void PhysicsManager::OnUpdate(float time) {
  if (threaded || sleep) return;

  btWorld->stepSimulation(time, SubSteps, 1.0f / updateRate);
}

void PhysicsManager::OnSetUp() {}

void PhysicsManager::OnClean() {
  btWorld->clearForces();

  // remove the rigidbodies from the dynamics btWorld and delete them
  for (int i = btWorld->getNumCollisionObjects() - 1; i >= 0; i--) {
    btCollisionObject *obj = btWorld->getCollisionObjectArray()[i];
    btWorld->removeCollisionObject(obj);
    delete obj;
  }

  for (int i = btWorld->getNumConstraints() - 1; i >= 0; i--) {
    btTypedConstraint *constraint = btWorld->getConstraint(i);
    btWorld->removeConstraint(constraint);
    delete constraint;
  }
}

void PhysicsManager::CreateTerrainHeightfieldShape(Ogre::TerrainGroup *terrainGroup) {
  for (auto it = terrainGroup->getTerrainIterator(); it.hasMoreElements();) {
    auto *terrain = it.getNext()->instance;
    CreateTerrainHeightfieldShape(terrain);
  }
}

void PhysicsManager::CreateTerrainHeightfieldShape(Ogre::Terrain *terrain) {
  int size = terrain->getSize();
  float *data = terrain->getHeightData();
  float minHeight = terrain->getMinHeight();
  float maxHeight = terrain->getMaxHeight();
  Ogre::Vector3 position = terrain->getPosition();
  float scale = terrain->getWorldSize() / (static_cast<float>(terrain->getSize() - 1));

  // Convert height data in a format suitable for the physics engine
  auto *terrainHeights = new float[size * size];
  assert(terrainHeights != 0);

  for (int i = 0; i < size; i++) {
    memcpy(terrainHeights + size * i, data + size * (size - i - 1), sizeof(float) * size);
  }

  const btScalar heightScale = 1.0f;

  btVector3 localScaling(scale, heightScale, scale);

  auto *terrainShape = new btHeightfieldTerrainShape(size, size, terrainHeights, 1, minHeight, maxHeight, 1, PHY_FLOAT, false);

  terrainShape->setUseDiamondSubdivision(true);
  terrainShape->setLocalScaling(localScaling);

  auto *groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(size / 2, 0, size / 2)));
  btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, terrainShape, btVector3(0, 0, 0));

  // SetUp Rigid Body using 0 mass so it is static
  auto *rigidBody = new btRigidBody(groundRigidBodyCI);

  rigidBody->setFriction(0.4f);
  rigidBody->setHitFraction(0.8f);
  rigidBody->setRestitution(0.6f);
  rigidBody->getWorldTransform().setOrigin(btVector3(position.x, position.y + (maxHeight - minHeight) / 2 - 0.5, position.z));
  rigidBody->getWorldTransform().setRotation(Ogre::Bullet::convert(Ogre::Quaternion::IDENTITY));
  rigidBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);

  rigidBody->setUserIndex(0);
  btWorld->addRigidBody(rigidBody);
  btWorld->setForceUpdateAllAabbs(false);
}

void PhysicsManager::ProcessData(Ogre::Entity *entity, const Ogre::UserObjectBindings &userData) {
  string proxy = Ogre::any_cast<string>(userData.getUserAny("proxy"));
  string physics_type = Ogre::any_cast<string>(userData.getUserAny("physics_type"));
  float mass = Ogre::any_cast<float>(userData.getUserAny("mass"));
  float mass_radius = Ogre::any_cast<float>(userData.getUserAny("mass_radius"));
  float inertia_tensor = Ogre::any_cast<float>(userData.getUserAny("inertia_tensor"));
  float velocity_min = Ogre::any_cast<float>(userData.getUserAny("velocity_min"));
  float velocity_max = Ogre::any_cast<float>(userData.getUserAny("velocity_max"));
  bool lock_trans_x = Ogre::any_cast<bool>(userData.getUserAny("lock_trans_x"));
  bool lock_trans_y = Ogre::any_cast<bool>(userData.getUserAny("lock_trans_y"));
  bool lock_trans_z = Ogre::any_cast<bool>(userData.getUserAny("lock_trans_z"));
  bool lock_rot_x = Ogre::any_cast<bool>(userData.getUserAny("lock_rot_x"));
  bool lock_rot_y = Ogre::any_cast<bool>(userData.getUserAny("lock_rot_y"));
  bool lock_rot_z = Ogre::any_cast<bool>(userData.getUserAny("lock_rot_z"));
  bool anisotropic_friction = Ogre::any_cast<bool>(userData.getUserAny("anisotropic_friction"));
  float friction_x = Ogre::any_cast<float>(userData.getUserAny("friction_x"));
  float friction_y = Ogre::any_cast<float>(userData.getUserAny("friction_y"));
  float friction_z = Ogre::any_cast<float>(userData.getUserAny("friction_z"));
  float damping_trans = Ogre::any_cast<float>(userData.getUserAny("damping_trans"));
  float damping_rot = Ogre::any_cast<float>(userData.getUserAny("damping_rot"));
  bool isStatic = (physics_type != "dynamic");

  btRigidBody *rigidBody = nullptr;
  btVector3 Inertia = btVector3(0, 0, 0);

  Ogre::SceneNode *parent = entity->getParentSceneNode();
  btConvexInternalShape *entShape = nullptr;

  if (proxy == "box")
    entShape = Ogre::Bullet::createBoxCollider(entity);
  else if (proxy == "capsule")
    entShape = Ogre::Bullet::createCapsuleCollider(entity);
  else if (proxy == "sphere")
    entShape = Ogre::Bullet::createSphereCollider(entity);
  else if (proxy == "cylinder")
    entShape = Ogre::Bullet::createCylinderCollider(entity);
  else
    entShape = Ogre::Bullet::createBoxCollider(entity);

  if (mass > 0.0) entShape->calculateLocalInertia(mass, Inertia);

  auto *bodyState = new Ogre::Bullet::RigidBodyState(parent);

  rigidBody = new btRigidBody(mass, bodyState, entShape, Inertia);
  if (mass == 0.0) rigidBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);

  btWorld->addRigidBody(rigidBody);
}

}  // namespace Glue
