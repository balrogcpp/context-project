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

#include "PhysicsManager.h"
#include "ConfiguratorJson.h"
#include "ShaderResolver.h"

namespace Context {
//----------------------------------------------------------------------------------------------------------------------
std::shared_ptr<btDynamicsWorld> PhysicsManager::GetPhyWorld() const {
  return phy_world_;
}
//----------------------------------------------------------------------------------------------------------------------
void PhysicsManager::Setup() {
  ConfiguratorJson::Assign(physics_skip_frames_, "physics_skip_frames");
  ConfiguratorJson::Assign(physics_max_sub_steps_, "physics_max_sub_steps");
  ConfiguratorJson::Assign(physics_debug_show_collider_, "physics_debug_show_collider");

  float gravity_x = 0;
  float gravity_y = -9.8f;
  float gravity_z = 0;
  ConfiguratorJson::Assign(gravity_x, "physics_gravity_x");
  ConfiguratorJson::Assign(gravity_y, "physics_gravity_y");
  ConfiguratorJson::Assign(gravity_z, "physics_gravity_z");

  broadphase_ = std::make_shared<btAxisSweep3>(btVector3(-1000, -1000, -1000), btVector3(1000, 1000, 1000), 1024);
  collision_config_ = std::make_shared<btDefaultCollisionConfiguration>();
  dispatcher_ = std::make_shared<btCollisionDispatcher>(collision_config_.get());
  solver_ = std::make_shared<btSequentialImpulseConstraintSolver>();
  phy_world_ = std::make_shared<btDiscreteDynamicsWorld>(dispatcher_.get(),
                                                         broadphase_.get(),
                                                         solver_.get(),
                                                         collision_config_.get());

  phy_world_->setGravity(btVector3(0.0, gravity_y, 0.0));

  if (physics_debug_show_collider_) {
    dbg_draw_ = std::make_shared<BtOgre::DebugDrawer>(scene_->getRootSceneNode(), phy_world_.get());
    dbg_draw_->setDebugMode(physics_debug_show_collider_);
    phy_world_->setDebugDrawer(dbg_draw_.get());

#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
    ShaderResolver::FixMaterial("BtOgre/DebugLines");
#endif
  }

  stopped_ = false;
}
//----------------------------------------------------------------------------------------------------------------------
bool PhysicsManager::frameRenderingQueued(const Ogre::FrameEvent &evt) {
  if (!stopped_) {
    factor_++;
    cumulative_ += evt.timeSinceLastFrame;

    if (factor_ == (physics_skip_frames_ - 1)) {
      if (phy_world_) {
        phy_world_->stepSimulation(cumulative_, physics_max_sub_steps_);
      }
      cumulative_ = 0;
      factor_ = 0;
    }
  }

  if (physics_debug_show_collider_ && dbg_draw_) {
    dbg_draw_->step();
  }

  return true;
}
//----------------------------------------------------------------------------------------------------------------------
void PhysicsManager::Reset() {
  stopped_ = true;
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
void PhysicsManager::Start() {
  stopped_ = false;
}
//----------------------------------------------------------------------------------------------------------------------
void PhysicsManager::Stop() {
  stopped_ = true;
}
//----------------------------------------------------------------------------------------------------------------------
void PhysicsManager::SetPhysicsDebugShowCollider(bool physics_debug_show_collider) {
  physics_debug_show_collider_ = physics_debug_show_collider;
}
//----------------------------------------------------------------------------------------------------------------------
void PhysicsManager::AddRigidBody(btRigidBody *body) {
  phy_world_->addRigidBody(body);
//  rigid_bodies_.push_back(body);
}
//----------------------------------------------------------------------------------------------------------------------
btRigidBody *PhysicsManager::GenerateProxy(Ogre::Entity *entity, const std::string &proxy_type, std::string &physics_type) {
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

  btRigidBody *rigid_body = nullptr;

  return rigid_body;
}
//----------------------------------------------------------------------------------------------------------------------
void PhysicsManager::ProcessData(Ogre::UserObjectBindings &user_object_bindings,
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
  if (user_object_bindings.getUserAny("proxy").has_value()) {
    proxy_type = Ogre::any_cast<std::string>(user_object_bindings.getUserAny("proxy"));
  }

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

    if (entity->getNumManualLodLevels() > 0) {
      converter = std::make_unique<BtOgre::StaticMeshToShapeConverter>(entity->getManualLodLevel(
          entity->getNumManualLodLevels() - 1));
    } else {
      converter = std::make_unique<BtOgre::StaticMeshToShapeConverter>(entity);
    }

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

    PhysicsManager::GetSingleton().AddRigidBody(entBody);
  }
}
} //namespace Context
