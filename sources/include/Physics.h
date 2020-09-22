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
struct Contact {
  Contact() = default;
  Contact(const btCollisionObject *a, int points)
    : a_(a), points_(points)
  {}

  Contact& operator= (const Contact& that) = default;

  const btCollisionObject * a_;
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
  void DispatchCollisions();
  void AddRigidBody(btRigidBody *body);
  void ProcessData(Ogre::UserObjectBindings &user_data, Ogre::Entity *entity, Ogre::SceneNode *parent_node);
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
  std::vector<btCollisionObject *> rigid_bodies_;
  std::map<const btCollisionObject *, Contact> contacts_;
  std::function<void(int a, int b)> callback_;
 private:
  int steps_ = 8;
  bool pause_ = false;
  bool debug_ = false;

 public:
  void Start() noexcept {
    pause_ = false;
  }
  void Pause() noexcept {
    pause_ = true;
  }
  void SetCallback(const std::function<void(int a, int b)> &callback) {
    callback_ = callback;
  }
};
}