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

#include "IO.h"
#include "Singleton.h"

#include <Ogre.h>

namespace Ogre {
class SceneNode;
}

class btRigidBody;

namespace Context {

enum class CameraStyle   // enumerator values for different styles of camera movement
{
  FREELOOK,
  ORBIT,
  FPS,
  MANUAL
};

 class CameraMan : public io::InputListener, public Singleton, public Ogre::FrameListener {
 public:
  CameraMan();
  explicit CameraMan(Ogre::SceneNode *ogre_camera_node);
  ~CameraMan() override;

 public:
  void SetTarget(Ogre::SceneNode *target);
  Ogre::SceneNode *GetTarget();
  void SetTopSpeed(float top_speed);
  float GetTopSpeed();
  float GetDistToTarget();
  void SetStyle(CameraStyle style);
  CameraStyle GetStyle();
  void Freeze(bool freeze);
  void RegCamera(Ogre::SceneNode *ogre_camera_node);
  void UnregCamera();
  Ogre::SceneNode *GetCameraNode();
  virtual void SetYawPitchDist(Ogre::Radian yaw, Ogre::Radian pitch, float dist);
  void ManualStop();

  bool frameRenderingQueued(const Ogre::FrameEvent &event) override;

  void Move(int x, int y, int dx, int dy, bool left, bool right, bool middle) override;
  void LbDown(int x, int y) override;
  void LbUp(int x, int y) override;
  void RbDown(int x, int y) override;
  void RbUp(int x, int y) override;
  void MbDown(int x, int y) override;
  void MbUp(int x, int y) override;
  void KeyDown(SDL_Keycode sym) override;
  void KeyUp(SDL_Keycode sym) override;

 private:
  Ogre::SceneNode *camera_node_ = nullptr;
  Ogre::SceneNode *camera_yaw_node_ = nullptr;
  Ogre::SceneNode *camera_pitch_node_ = nullptr;
  Ogre::SceneNode *camera_roll_node_ = nullptr;
  btRigidBody *rigid_body_ = nullptr;

 public:
  void SetRigidBody(btRigidBody *rigid_body);
  btRigidBody *GetRigidBody() const;

 private:
  Ogre::Degree dx_, dy_;
  Ogre::Camera *ogre_camera_ = nullptr;
  CameraStyle camera_style_ = CameraStyle::FPS;
  Ogre::SceneNode *target_ = nullptr;
  bool orbiting_ = false;
  bool zoom_ = false;
  bool moving_ = false;
  float top_speed_ = 10.0f;
  float run_speed_ = 20.0f;
  float animation_time_ = 0.0f;
  float anim_duration_ = .5f;//ms
  float const_speed_ = 1.5f;
  float heigh_ = 1.5f;
  bool move_forward_ = false;
  bool move_back_ = false;
  bool move_left_ = false;
  bool move_right_ = false;
  bool move_up_ = false;
  bool move_down_ = false;
  bool move_fast_ = false;
  bool stop_ = false;
  Ogre::Vector3 offset_ = Ogre::Vector3::ZERO;
  Ogre::Vector3 velocity_ = Ogre::Vector3::ZERO;
  Ogre::Vector3 prev_pos_ = Ogre::Vector3::ZERO;
};

}