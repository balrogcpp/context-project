// This source file is part of "glue project". Created by Andrey Vasiliev

#pragma once
#include "Input/InputObserver.h"
#include "Objects/Object.h"
#include <OgreVector.h>
extern "C" {
#include <SDL2/SDL_keycode.h>
}

namespace Ogre {
class SceneNode;
class Camera;
}  // namespace Ogre

class btRigidBody;

namespace Glue {

class CameraMan final : public Object, public InputObserver {
 public:
  // enumerator values for different styles of camera movement
  enum ControlStyle { FREELOOK, ORBIT, FPS, MANUAL };

  CameraMan();
  virtual ~CameraMan();


  void Update(float PassedTime) override;

  void OnMouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) override;
  void OnKeyDown(SDL_Keycode sym) override;
  void OnKeyUp(SDL_Keycode sym) override;

  void ManualStop();
  void SetRigidBody(btRigidBody* RigidBodyPtr);
  Ogre::SceneNode* GetCameraNode() const;
  Ogre::Camera* GetCamera() const;
  void AttachNode(Ogre::SceneNode* ParentPtr, Ogre::SceneNode* ProxyPtr = nullptr);
  void AttachCamera(Ogre::Camera* CameraPtr, Ogre::SceneNode* ProxyPtr = nullptr);
  void UnregCamera();
  void SetStyle(ControlStyle style);
  ControlStyle GetStyle() const noexcept;

 protected:
  Ogre::SceneNode* CameraNode = nullptr;
  Ogre::SceneNode* yaw_node = nullptr;
  Ogre::SceneNode* pitch_node = nullptr;
  Ogre::SceneNode* roll_node = nullptr;
  btRigidBody* RigidBody = nullptr;
  Ogre::Camera* OgreCamera = nullptr;
  Ogre::SceneNode* target = nullptr;
  ControlStyle Style = ControlStyle::MANUAL;
  float top_speed = 10.0;
  float run_speed = 20.0;
  float animation_time = 0.5;
  float anim_duration = 0.5;
  float const_speed = 5.0;
  bool move_forward = false;
  bool move_back = false;
  bool move_left = false;
  bool move_right = false;
  bool move_up = false;
  bool move_down = false;
  bool move_fast = false;
  bool stop = false;
  Ogre::Vector3 Velocity = {0, 0, 0};
  Ogre::Vector3 prev_pos = {0, 0, 0};
};

}  // namespace Glue
