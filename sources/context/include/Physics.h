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

#pragma once

#include "Manager.h"

#include <OgreAny.h>

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
class btCollisionShape;
class btCollisionObject;

namespace Context {

class Physics final : public Manager {
 public:
  Physics();
  virtual ~Physics();

  void Init() final {}
  void Clear() final;
  void AddRigidBody(btRigidBody *body);
  void ProcessData(Ogre::UserObjectBindings &user_data, Ogre::Entity *entity, Ogre::SceneNode *parent_node);

 private:
  bool frameRenderingQueued(const Ogre::FrameEvent &evt) final;

 private:
  std::shared_ptr<BtOgre::DebugDrawer> dbg_draw_;
  std::shared_ptr<btAxisSweep3> broadphase_;
  std::shared_ptr<btDefaultCollisionConfiguration> collision_config_;
  std::shared_ptr<btCollisionDispatcher> dispatcher_;
  std::shared_ptr<btSequentialImpulseConstraintSolver> solver_;
  std::shared_ptr<btDynamicsWorld> phy_world_;
  std::vector<btCollisionObject *> rigid_bodies_;
  std::vector<btCollisionShape *> collision_shapes_;

  int skip_frames_ = 2;
  int sub_steps_ = 8;
  bool stopped_ = false;
  bool physics_debug_show_collider_ = false;

 public:
  std::shared_ptr<btDynamicsWorld> GetPhyWorld() const;

  void Start() noexcept {
    stopped_ = false;
  }

  void Stop() noexcept {
    stopped_ = true;
  }
}; //class PhysicsManager
} //namespace Context
