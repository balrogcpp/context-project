// This source file is part of "glue project". Created by Andrey Vasiliev

#include "PCHeader.h"
#include "Components/Physics.h"
#include <BulletCollision/CollisionDispatch/btCollisionDispatcherMt.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolverMt.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.h>

using namespace std;
using namespace Ogre;

namespace Glue {

Physics::Physics(bool threaded) : Threaded(threaded) {
  auto *Scheduler = btCreateDefaultTaskScheduler();

  if (!Scheduler) throw Exception("Bullet physics no task scheduler available");

  btSetTaskScheduler(Scheduler);
  BtBroadphase = make_unique<btDbvtBroadphase>();
  BtConfig = make_unique<btDefaultCollisionConfiguration>();
  BtDispatcher = make_unique<btCollisionDispatcherMt>(BtConfig.get(), 40);
  BtSolver = make_unique<btSequentialImpulseConstraintSolverMt>();

  btConstraintSolverPoolMt *solverPool = new btConstraintSolverPoolMt(BT_MAX_THREAD_COUNT);

  BtWorld = make_unique<btDiscreteDynamicsWorldMt>(BtDispatcher.get(), BtBroadphase.get(), solverPool, BtSolver.get(), BtConfig.get());

  BtWorld->setGravity(btVector3(0.0, -9.8, 0.0));

  if (DebugWraw) {
    auto *node = Ogre::Root::getSingleton().getSceneManager("Default")->getRootSceneNode();
    DbgDraw = make_unique<BtOgre::DebugDrawer>(node, BtWorld.get());
    DbgDraw->setDebugMode(DebugWraw);
    BtWorld->setDebugDrawer(DbgDraw.get());
  }

  if (Threaded) InitThread();

  Paused = false;
  Running = true;
}

void Physics::InitThread() {
  if (!Threaded) return;

  static int64_t time_of_last_frame_ = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();

  function<void(void)> update_func_ = [&]() {
    while (Running) {
      auto before_update = chrono::system_clock::now().time_since_epoch();
      int64_t micros_before_update = chrono::duration_cast<chrono::microseconds>(before_update).count();
      float frame_time = static_cast<float>(micros_before_update - time_of_last_frame_) / 1e+6;
      time_of_last_frame_ = micros_before_update;

      // Actually do calculations
      if (!Paused) {
        BtWorld->stepSimulation(frame_time, SubSteps, 1.0f / UpdateRate);

        if (DebugWraw) DbgDraw->update();

        DispatchCollisions();
      }
      // Actually do calculations

      auto after_update = chrono::system_clock::now().time_since_epoch();
      int64_t micros_after_update = chrono::duration_cast<chrono::microseconds>(after_update).count();

      auto update_time = micros_after_update - micros_before_update;

      auto delay = static_cast<int64_t>((1e+6 / UpdateRate) - update_time);
      if (delay > 0) this_thread::sleep_for(chrono::microseconds(delay));
    }
  };

  UpdateThread = make_unique<thread>(update_func_);
  UpdateThread->detach();
}

void Physics::DispatchCollisions() {
  //  map<const btCollisionObject *, ContactInfo> new_contacts;

  /* Browse all collision pairs */

  //  for (size_t i = 0; i < world_->getDispatcher()->getNumManifolds(); i++) {
  //    btPersistentManifold *manifold = world_->getDispatcher()->getManifoldByIndexInternal(i);
  //    auto *obA = manifold->getBody0();
  //    auto *obB = manifold->getBody1();

  /* Check all contacts points */
  //    for (size_t j = 0; j < manifold->getNumContacts(); j++) {
  //      btManifoldPoint &pt = manifold->getContactPoint(j);
  //      if (pt.getDistance() < 1E-6) {
  //        const btVector3 &ptA = pt.getPositionWorldOnA();
  //        const btVector3 &ptB = pt.getPositionWorldOnB();
  //        const btVector3 &normalOnB = pt.m_normalWorldOnB;
  //        if (new_contacts.find(obA) == new_contacts.end())
  //          new_contacts[obA] = {obB, manifold->getNumContacts()};
  //      }
  //    }
  //  }

  /* Check for added contacts ... */
  //  for (const auto &it : new_contacts) {
  //    bool detected = false;
  //    if (contacts_.find(it.first) == contacts_.end()) {
  //      detected = true;
  //    } else {
  //      contacts_.erase(it.first);
  //            if (new_contacts[it.first].points_ == contacts_[it.first].points_)
  //            {
  //              contacts_.erase(it.first);
  //            } else {
  //              detected = true;
  //            }
  //    }

  //    if (detected && callback_) callback_(it.first->getUserIndex(), it.first->getUserIndex());
  //  }

  /* ... and removed contacts */
  //  for (const auto &it : contacts_) {
  //  }
  //  contacts_.clear();
  //  contacts_ = new_contacts;
}

Physics::~Physics() { Running = false; }

void Physics::OnResume() { Paused = false; }

void Physics::OnPause() { Paused = true; }

void Physics::OnUpdate(float time) {
  if (Threaded || Paused) return;

  BtWorld->stepSimulation(time, SubSteps, 1.0f / UpdateRate);

  if (DebugWraw) DbgDraw->update();

  DispatchCollisions();
}

void Physics::OnClean() {
  BtWorld->clearForces();

  // remove the rigidbodies from the dynamics BtWorld and delete them
  for (int i = BtWorld->getNumCollisionObjects() - 1; i >= 0; i--) {
    btCollisionObject *obj = BtWorld->getCollisionObjectArray()[i];
    BtWorld->removeCollisionObject(obj);
    delete obj;
  }

  for (int i = BtWorld->getNumConstraints() - 1; i >= 0; i--) {
    btTypedConstraint *constraint = BtWorld->getConstraint(i);
    BtWorld->removeConstraint(constraint);
    delete constraint;
  }
}

void Physics::AddRigidBody(btRigidBody *body) { BtWorld->addRigidBody(body); }

void Physics::CreateTerrainHeightfieldShape(int size, float *data, const float &min_height, const float &max_height, const Ogre::Vector3 &position,
                                            const float &scale) {
  // Convert height data in a format suitable for the physics engine
  auto *terrainHeights = new float[size * size];
  assert(terrainHeights != 0);

  for (int i = 0; i < size; i++) {
    memcpy(terrainHeights + size * i, data + size * (size - i - 1), sizeof(float) * size);
  }

  const btScalar heightScale = 1.0f;

  btVector3 localScaling(scale, heightScale, scale);

  auto *terrainShape = new btHeightfieldTerrainShape(size, size, terrainHeights, 1, min_height, max_height, 1, PHY_FLOAT, false);

  terrainShape->setUseDiamondSubdivision(true);
  terrainShape->setLocalScaling(localScaling);

  auto *groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(size / 2, 0, size / 2)));
  btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, terrainShape, btVector3(0, 0, 0));

  // SetUp Rigid Body using 0 mass so it is static
  auto *entBody = new btRigidBody(groundRigidBodyCI);

  entBody->setFriction(0.8f);
  entBody->setHitFraction(0.8f);
  entBody->setRestitution(0.6f);
  entBody->getWorldTransform().setOrigin(btVector3(position.x, position.y + (max_height - min_height) / 2 - 0.5, position.z));
  entBody->getWorldTransform().setRotation(BtOgre::Convert::toBullet(Ogre::Quaternion::IDENTITY));
  entBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);

  entBody->setUserIndex(0);
  AddRigidBody(entBody);
  BtWorld->setForceUpdateAllAabbs(false);
}

void Physics::ProcessData(Ogre::Entity *EntityPtr, Ogre::SceneNode *ParentNode, bool Static, const std::string &ProxyType, float Mass,
                          float friction) {
  btRigidBody *entBody = nullptr;
  btVector3 Inertia = btVector3(0, 0, 0);

  if (!EntityPtr->getParentSceneNode() && ParentNode) ParentNode->attachObject(EntityPtr);
  auto *entShape = BtOgre::createCapsuleCollider(EntityPtr);

  if (Static)
    Mass = 0.0;
  else
    entShape->calculateLocalInertia(Mass, Inertia);

  auto *bodyState = new BtOgre::RigidBodyState(ParentNode);

  entBody = new btRigidBody(Mass, bodyState, entShape, Inertia);
  if (Static) entBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);

  AddRigidBody(entBody);
}

void Physics::ProcessData(Entity *EntityPtr, SceneNode *ParentNode, const UserObjectBindings &UserData) {
  string ProxyType;
  if (UserData.getUserAny("proxy").has_value()) ProxyType = any_cast<string>(UserData.getUserAny("proxy"));
  bool Static = false;
  string physics_type = any_cast<string>(UserData.getUserAny("physics_type"));
  float Mass = any_cast<float>(UserData.getUserAny("mass"));
  float mass_radius = any_cast<float>(UserData.getUserAny("mass_radius"));
  float inertia_tensor = any_cast<float>(UserData.getUserAny("inertia_tensor"));
  float velocity_min = any_cast<float>(UserData.getUserAny("velocity_min"));
  float velocity_max = any_cast<float>(UserData.getUserAny("velocity_max"));
  bool lock_trans_x = any_cast<bool>(UserData.getUserAny("lock_trans_x"));
  bool lock_trans_y = any_cast<bool>(UserData.getUserAny("lock_trans_y"));
  bool lock_trans_z = any_cast<bool>(UserData.getUserAny("lock_trans_z"));
  bool lock_rot_x = any_cast<bool>(UserData.getUserAny("lock_rot_x"));
  bool lock_rot_y = any_cast<bool>(UserData.getUserAny("lock_rot_y"));
  bool lock_rot_z = any_cast<bool>(UserData.getUserAny("lock_rot_z"));
  bool anisotropic_friction = any_cast<bool>(UserData.getUserAny("anisotropic_friction"));
  float friction_x = any_cast<float>(UserData.getUserAny("friction_x"));
  float friction_y = any_cast<float>(UserData.getUserAny("friction_y"));
  float friction_z = any_cast<float>(UserData.getUserAny("friction_z"));
  float damping_trans = any_cast<float>(UserData.getUserAny("damping_trans"));
  float damping_rot = any_cast<float>(UserData.getUserAny("damping_rot"));

  btRigidBody *entBody = nullptr;
  btVector3 Inertia = btVector3(0, 0, 0);

  if (!EntityPtr->getParentSceneNode() && ParentNode) ParentNode->attachObject(EntityPtr);
  auto *entShape = BtOgre::createCapsuleCollider(EntityPtr);

  if (Static)
    Mass = 0.0;
  else
    entShape->calculateLocalInertia(Mass, Inertia);

  auto *bodyState = new BtOgre::RigidBodyState(ParentNode);

  entBody = new btRigidBody(Mass, bodyState, entShape, Inertia);
  if (Static) entBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);

  AddRigidBody(entBody);
}

}  // namespace Glue
