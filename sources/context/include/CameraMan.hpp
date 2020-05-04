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

#include "InputListener.hpp"
#include "NoCopy.hpp"

#include <Ogre.h>

namespace Ogre {
class SceneNode;
}

namespace Context {

enum class CameraStyle   // enumerator values for different styles of camera movement
{
  FREELOOK,
  ORBIT,
  FPS,
  MANUAL
};

class CameraMan : public InputListener, public NoCopy, public Ogre::FrameListener {
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

  void MouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) override;
  void LeftButtonDown(int x, int y) override;
  void LeftButtonUp(int x, int y) override;
  void RightButtonDown(int x, int y) override;
  void RightButtonUp(int x, int y) override;
  void MiddleButtonDown(int x, int y) override;
  void MiddleButtonUp(int x, int y) override;
  void KeyDown(SDL_Keycode sym) override;
  void KeyUp(SDL_Keycode sym) override;

 private:
  Ogre::SceneNode *cameraNode = nullptr;
  Ogre::SceneNode *cameraYawNode = nullptr;
  Ogre::SceneNode *cameraPitchNode = nullptr;
  Ogre::SceneNode *cameraRollNode = nullptr;
  Ogre::Degree dx_, dy_;
  Ogre::Camera *camera = nullptr;
  CameraStyle camera_style_ = CameraStyle::FPS;
  Ogre::SceneNode *target_ = nullptr;
  bool orbiting_ = false;
  bool zoom_ = false;
  bool moving_ = false;
  float top_speed_ = 1.0f;
  float run_speed_ = 8.0f;
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
  Ogre::Vector3 offset = Ogre::Vector3::ZERO;
  Ogre::Vector3 velocity = Ogre::Vector3::ZERO;
  Ogre::Vector3 prevPos = Ogre::Vector3::ZERO;
  Ogre::Vector3 translate = Ogre::Vector3::ZERO;

};

}
