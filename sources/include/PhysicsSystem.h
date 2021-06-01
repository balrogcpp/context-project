//MIT License
//
//Copyright (c) 2021 Andrew Vasiliev
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#pragma once
#include "Component.h"
#include "Singleton.h"
#include <OgreFrameListener.h>
#include <OgreAny.h>
#include <map>
#include <thread>

namespace BtOgre {
class DebugDrawer;
}

namespace Ogre {
class Entity;
class SceneNode;
}

class btDbvtBroadphase;
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

namespace xio {
struct ContactInfo {
  ContactInfo() = default;
  ContactInfo(const btCollisionObject *a, int points)
      : a_(a), points_(points) {}

  ContactInfo &operator=(const ContactInfo &that) = default;

  const btCollisionObject *a_;
  int points_;
};

class PhysicsSystem final : public Component, public Singleton<PhysicsSystem> {
 public:
  PhysicsSystem(bool threaded = false);
  virtual ~PhysicsSystem();

  void Cleanup() override;
  void Update(float time) override;
  void Resume() override;
  void Pause() override;

  void DispatchCollisions();
  void AddRigidBody(btRigidBody *body);
  void ProcessData(Ogre::UserObjectBindings &user_data, Ogre::Entity *entity, Ogre::SceneNode *parent_node);
  void ProcessData(Ogre::Entity *entity,
                   Ogre::SceneNode *parent_node = nullptr,
                   const std::string &proxy_type = "box",
                   const std::string &physics_type = "static",
                   float mass = 0.0,
                   float mass_radius = 0.0,
                   float inertia_tensor = 0.0,
                   float velocity_min = 0.0,
                   float velocity_max = 0.0,
                   float friction = 1.0);
  void CreateTerrainHeightfieldShape(int size,
                                     float *data,
                                     const float &min_height,
                                     const float &max_height,
                                     const Ogre::Vector3 &position,
                                     const float &scale);

  void SetCallback(const std::function<void(int a, int b)> &callback) {
	callback_ = callback;
  }

  bool IsThreaded() const;

 private:
  void InitThread_();

  std::unique_ptr<BtOgre::DebugDrawer> dbg_draw_;
  std::unique_ptr<btDbvtBroadphase> broadphase_;
  std::unique_ptr<btDefaultCollisionConfiguration> config_;
  std::unique_ptr<btCollisionDispatcherMt> dispatcher_;
  std::unique_ptr<btSequentialImpulseConstraintSolverMt> solver_;
  std::unique_ptr<btDiscreteDynamicsWorldMt> world_;
  std::unique_ptr<std::thread> update_thread_;
  std::map<const btCollisionObject *, ContactInfo> contacts_;
  std::function<void(int a, int b)> callback_;
  int steps_ = 4;
  bool threaded_ = false;
  int64_t update_rate_ = 60;
  std::atomic<bool> pause_ = true;
  std::atomic<bool> running_ = false;
  bool debug_ = false;
  int64_t time_of_last_frame_ = 0;
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

} //namespace
