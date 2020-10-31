//MIT License
//
//Copyright (c) 2020 Andrey Vasiliev
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

namespace BtOgre {
class DebugDrawer;
}

namespace Ogre {
class Entity;
class SceneNode;
}

class btAxisSweep3;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;
class btDynamicsWorld;
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

class Physics final : public Component, public Singleton<Physics> {
 public:
  Physics();
  virtual ~Physics();

  void Create() final {}
  void Reset() final {}
  void Clean() final;
  void Loop(float time) final;
  void Resume() final {
    pause_ = false;
  }
  void Pause() final {
    pause_ = true;
  }

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

 private:
  std::unique_ptr<BtOgre::DebugDrawer> dbg_draw_;
  std::unique_ptr<btAxisSweep3> broadphase_;
  std::unique_ptr<btDefaultCollisionConfiguration> configurator_;
  std::unique_ptr<btCollisionDispatcher> dispatcher_;
  std::unique_ptr<btSequentialImpulseConstraintSolver> solver_;
  std::unique_ptr<btDynamicsWorld> world_;
  std::map<const btCollisionObject *, ContactInfo> contacts_;
  std::function<void(int a, int b)> callback_;
  int steps_ = 8;
  bool pause_ = false;
  bool debug_ = false;
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

 public:
  void SetCallback(const std::function<void(int a, int b)> &callback) {
    callback_ = callback;
  }
};
}