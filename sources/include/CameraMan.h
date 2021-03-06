//MIT License
//
//Copyright (c) 2021 Andrei Vasilev
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
#include "Input.h"
#include "Entity.h"
#include "ComponentLocator.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"

namespace xio {
class CameraMan final : public Entity, public InputObserver {
 public:
  enum   // enumerator values for different styles of camera movement
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

  Ogre::SceneNode *node_;
  Ogre::SceneNode *camera_yaw_node_;
  Ogre::SceneNode *camera_pitch_node_;
  Ogre::SceneNode *camera_roll_node_;
  btRigidBody *rigid_;
  Ogre::Degree dx_, dy_;
  Ogre::Camera *camera_;
  int style_;
  Ogre::SceneNode *target_;
  bool orbiting_;
  bool moving_;
  float top_speed_;
  float run_speed_;
  float animation_time_;
  float anim_duration_;//ms
  float const_speed_;
  float heigh_;
  bool move_forward_;
  bool move_back_;
  bool move_left_;
  bool move_right_;
  bool move_up_;
  bool move_down_;
  bool move_fast_;
  bool stop_;
  Ogre::Vector3 offset_;
  Ogre::Vector3 velocity_;
  Ogre::Vector3 prev_pos_;

 public:
  void SetRigidBody(btRigidBody *rigid_body);

  btRigidBody *GetRigidBody() const;

  Ogre::SceneNode *GetCameraNode() const;

  Ogre::Camera *GetCamera() const;

  void AttachNode(Ogre::SceneNode *parent, Ogre::SceneNode *proxy = nullptr);

  void AttachCamera(Ogre::SceneNode *parent, Ogre::Camera *camera, Ogre::SceneNode *proxy = nullptr);

  void UnregCamera();

  void UpdateStyle();

  void SetStyle(int style);

  int GetStyle() const noexcept;
};
}