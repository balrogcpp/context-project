// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Components/Component.h"
#include "LazySingleton.h"
#include <OgreAny.h>
#include <OgreFrameListener.h>
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

struct ContactInfo {
  ContactInfo() = default;
  ContactInfo(const btCollisionObject *a, int points) : a_(a), points_(points) {}
  ContactInfo &operator=(const ContactInfo &that) = default;

  const btCollisionObject *a_;
  int points_;
};

class Physics final : public Component, public Singleton<Physics> {
 public:
  Physics(bool threaded = false);
  virtual ~Physics();

  void Cleanup() override;
  void Update(float time) override;
  void Resume() override;
  void Pause() override;

  void AddRigidBody(btRigidBody *body);
  void ProcessData(Ogre::UserObjectBindings &user_data, Ogre::Entity *entity, Ogre::SceneNode *parent_node);
  void ProcessData(Ogre::Entity *entity, Ogre::SceneNode *parent_node = nullptr, const std::string &proxy_type = "box",
                   const std::string &physics_type = "static", float mass = 0.0, float mass_radius = 0.0,
                   float inertia_tensor = 0.0, float velocity_min = 0.0, float velocity_max = 0.0,
                   float friction = 1.0);
  void CreateTerrainHeightfieldShape(int size, float *data, const float &min_height, const float &max_height,
                                     const Ogre::Vector3 &position, const float &scale);

 protected:
  std::unique_ptr<BtOgre::DebugDrawer> dbg_draw;
  std::unique_ptr<btDbvtBroadphase> broadphase;
  std::unique_ptr<btDefaultCollisionConfiguration> config;
  std::unique_ptr<btCollisionDispatcherMt> dispatcher;
  std::unique_ptr<btSequentialImpulseConstraintSolverMt> solver;
  std::unique_ptr<btDiscreteDynamicsWorldMt> world;
  std::unique_ptr<std::thread> update_thread;
  std::map<const btCollisionObject *, ContactInfo> contacts;
  std::function<void(int a, int b)> callback;
  int steps = 4;
  bool threaded = false;
  int64_t update_rate = 60;
  bool pause = true;
  bool running = false;
  bool debug = true;
  int64_t time_of_last_frame = 0;
  int64_t cumulated_time_ = 0;
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
