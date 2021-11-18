﻿// This source file is part of "glue project"
// Created by Andrew Vasiliev

#pragma once
#include "InputHandler.h"
#include "Object.h"
#include "view_ptr.h"
#include <OgreVector.h>
extern "C" {
#include <SDL2/SDL_keycode.h>
}

namespace Ogre {
class SceneNode;
class Camera;
}  // namespace Ogre

class btRigidBody;

namespace glue {
class CameraMan final : public Object, public MutedInputObserver {
 public:
  enum class Style  // enumerator values for different styles of camera movement
  { FREELOOK,
    ORBIT,
    FPS,
    MANUAL };

  CameraMan();
  virtual ~CameraMan();

  void ManualStop();
  void Update(float time) override;
  void OnMouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) override;
  void OnKeyDown(SDL_Keycode sym) override;
  void OnKeyUp(SDL_Keycode sym) override;

  void SetRigidBody(view_ptr<btRigidBody> rigid_body);
  Ogre::SceneNode *GetCameraNode() const;
  Ogre::Camera *GetCamera() const;
  void AttachNode(Ogre::SceneNode *parent, Ogre::SceneNode *proxy = nullptr);
  void AttachCamera(Ogre::SceneNode *parent, Ogre::Camera *camera, Ogre::SceneNode *proxy = nullptr);
  void UnregCamera();
  void SetStyle(Style style);
  Style GetStyle() const noexcept;

 protected:
  view_ptr<Ogre::SceneNode> node_;
  view_ptr<Ogre::SceneNode> camera_yaw_node_;
  view_ptr<Ogre::SceneNode> camera_pitch_node_;
  view_ptr<Ogre::SceneNode> camera_roll_node_;
  view_ptr<btRigidBody> rigid_;
  Ogre::Degree dx_, dy_;
  view_ptr<Ogre::Camera> camera_;
  view_ptr<Ogre::SceneNode> target_;
  Style style_ = Style::MANUAL;
  float top_speed_ = 10;
  float run_speed_ = 20;
  float animation_time_ = 0.5;
  float anim_duration_ = 0.5;  // ms
  float const_speed_ = 5;
  bool move_forward_ = false;
  bool move_back_ = false;
  bool move_left_ = false;
  bool move_right_ = false;
  bool move_up_ = false;
  bool move_down_ = false;
  bool move_fast_ = false;
  bool stop_ = false;
  Ogre::Vector3 offset_;
  Ogre::Vector3 velocity_;
  Ogre::Vector3 prev_pos_;
};

}  // namespace glue
