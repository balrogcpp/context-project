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
#include <OgreRoot.h>
#include <OgreSceneNode.h>
#include "InputHandler.h"
#include "Object.h"

#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "view_ptr.h"

namespace xio {
class CameraMan final : public Object, public MutedInputObserver {
 public:
  enum class Style  // enumerator values for different styles of camera movement
  {
    FREELOOK,
    ORBIT,
    FPS,
    MANUAL
  };

  CameraMan();
  virtual ~CameraMan();

//----------------------------------------------------------------------------------------------------------------------
  void ManualStop();
  void Update(float time) override;
  void OnMouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) override;
  void OnKeyDown(SDL_Keycode sym) override;
  void OnKeyUp(SDL_Keycode sym) override;

 private:
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
  float anim_duration_ = 0.5;//ms
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

 public:
  void SetRigidBody(view_ptr<btRigidBody> rigid_body);

  Ogre::SceneNode *GetCameraNode() const;

  Ogre::Camera *GetCamera() const;

  void AttachNode(Ogre::SceneNode *parent, Ogre::SceneNode *proxy = nullptr);

  void AttachCamera(Ogre::SceneNode *parent, Ogre::Camera *camera, Ogre::SceneNode *proxy = nullptr);

  void UnregCamera();

  void SetStyle(Style style);

  Style GetStyle() const noexcept;
};

} //namespace
