// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include "Components/Component.h"
#include "BtOgre/BtOgre.h"
#include <OgreAny.h>
#include <OgreFrameListener.h>
#include <functional>
#include <map>
#include <thread>

namespace BtOgre {
class DebugDrawer;
}

namespace Ogre {
class Entity;
class SceneNode;
}  // namespace Ogre

// Forward declaration
struct btDbvtBroadphase;
class btAxisSweep3;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btCollisionDispatcherMt;
class btSequentialImpulseConstraintSolver;
class btSequentialImpulseConstraintSolverMt;
class btDynamicsWorld;
class btDiscreteDynamicsWorld;
class btDiscreteDynamicsWorldMt;
class btRigidBody;
class btCollisionObject;

namespace Glue {

class Physics final : public Component<Physics> {
  ///
  struct ContactInfo {
    ContactInfo() = default;
    ContactInfo(const btCollisionObject *a, int points) : a_(a), points_(points) {}
    ContactInfo &operator=(const ContactInfo &that) = default;

    const btCollisionObject *a_;
    int points_;
  };

 public:
  /// Constructors
  Physics(bool threaded = false);
  virtual ~Physics();

  /// Create thread
  void InitThread();
  void DispatchCollisions();
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
  std::unique_ptr<BtOgre::DebugDrawer> DbgDraw;
  std::unique_ptr<btDbvtBroadphase> BtBroadphase;
  std::unique_ptr<btDefaultCollisionConfiguration> BtConfig;
  std::unique_ptr<btCollisionDispatcherMt> BtDispatcher;
  std::unique_ptr<btSequentialImpulseConstraintSolverMt> BtSolver;
  std::unique_ptr<btDiscreteDynamicsWorldMt> BtWorld;
  std::unique_ptr<std::thread> UpdateThread;
  std::map<const btCollisionObject *, ContactInfo> ContactList;
  std::function<void(int a, int b)> ContactCalback;
  int SubSteps = 4;
  bool Running = false;
  std::atomic<bool> MTPaused = false;
  std::atomic<bool> MTRunning = false;
  bool Threaded = false;
  int64_t UpdateRate = 60;
  bool DebugWraw = false;
  const std::string TYPE_STATIC = "static";
  const std::string TYPE_DYNAMIC = "dynamic";
  const std::string TYPE_ACTOR = "actor";
  const std::string TYPE_GHOST = "ghost";
  const std::string TYPE_NONE = "none";
  const std::string PROXY_BOX = "box";
  const std::string PROXY_CAPSULE = "capsule";
  const std::string PROXY_SPHERE = "sphere";
  const std::string PROXY_CYLINDER = "cylinder";
  const std::string PROXY_TRIMESH = "trimesh";
  const std::string PROXY_CONVEX = "convex";
};

}  // namespace Glue
