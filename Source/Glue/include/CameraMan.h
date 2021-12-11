// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "PhysicalInput/InputHandler.h"
#include "Object.h"
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
  enum Style { FREELOOK, ORBIT, FPS, MANUAL };

  CameraMan();
  virtual ~CameraMan();

  void ManualStop();
  void Update(float PassedTime) override;
  void OnMouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) override;
  void OnKeyDown(SDL_Keycode sym) override;
  void OnKeyUp(SDL_Keycode sym) override;

  void SetRigidBody(btRigidBody* RigidBodyPtr);
  Ogre::SceneNode* GetCameraNode() const;
  Ogre::Camera* GetCamera() const;
  void AttachNode(Ogre::SceneNode* ParentPtr, Ogre::SceneNode* ProxyPtr = nullptr);
  void AttachCamera(Ogre::SceneNode* ParentPtr, Ogre::Camera* CameraPtr, Ogre::SceneNode* ProxyPtr = nullptr);
  void UnregCamera();
  void SetStyle(Style style);
  Style GetStyle() const noexcept;

 protected:
  Ogre::SceneNode* node = nullptr;
  Ogre::SceneNode* yaw_node = nullptr;
  Ogre::SceneNode* pitch_node = nullptr;
  Ogre::SceneNode* roll_node = nullptr;
  btRigidBody* rigid = nullptr;
  Ogre::Camera* camera = nullptr;
  Ogre::SceneNode* target = nullptr;
  Style style = Style::MANUAL;
  float top_speed = 10;
  float run_speed = 20;
  float animation_time = 0.5;
  float anim_duration = 0.5;  // ms
  float const_speed = 5;
  bool move_forward = false;
  bool move_back = false;
  bool move_left = false;
  bool move_right = false;
  bool move_up = false;
  bool move_down = false;
  bool move_fast = false;
  bool stop = false;
  Ogre::Vector3 offset;
  Ogre::Vector3 velocity;
  Ogre::Vector3 prev_pos;
};

}  // namespace Glue
