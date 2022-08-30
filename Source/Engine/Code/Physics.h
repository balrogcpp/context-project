// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include "System.h"
#include <Bullet/OgreBullet.h>
#include <OgreAny.h>
#include <OgreFrameListener.h>
#include <BulletCollision/CollisionDispatch/btCollisionDispatcherMt.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolverMt.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.h>
#include <thread>


namespace Glue {

class Physics final : public System<Physics> {
 public:
  /// Constructors
  Physics(bool threaded = false);
  virtual ~Physics();

  /// Create thread
  void InitThread();
  void OnClean() override;
  void OnUpdate(float time) override;
  void OnResume() override;
  void OnPause() override;

  void AddRigidBody(btRigidBody *RidigBodyPtr);
  void ProcessData(Ogre::Entity *EntityPtr, Ogre::SceneNode *ParentNode, const Ogre::UserObjectBindings &UserData);
  void ProcessData(Ogre::Entity *EntityPtr, Ogre::SceneNode *ParentNode, bool Static = true, const std::string &ProxyType = "box", float Mass = 1.0, float friction = 1.0);
  void CreateTerrainHeightfieldShape(int size, float *data, const float &min_height, const float &max_height, const Ogre::Vector3 &position,
                                     const float &scale);

 protected:
  std::unique_ptr<btDbvtBroadphase> BtBroadphase;
  std::unique_ptr<btDefaultCollisionConfiguration> BtConfig;
  std::unique_ptr<btCollisionDispatcher> BtDispatcher;
  std::unique_ptr<btSequentialImpulseConstraintSolver> BtSolver;
  std::unique_ptr<btDiscreteDynamicsWorld> BtWorld;
  std::unique_ptr<std::thread> UpdateThread;
  int SubSteps = 4;
  bool Running = false;
  std::atomic<bool> MTPaused = false;
  std::atomic<bool> MTRunning = false;
  bool Threaded = false;
  int64_t UpdateRate = 60;
};

}  // namespace Glue
