// This source file is part of "glue project". Created by Andrey Vasiliev

#pragma once
#include "Input/InputObserver.h"
#include "Objects/Object.h"
#include <OgreVector.h>

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
  Ogre::SceneNode* YawNode = nullptr;
  Ogre::SceneNode* PitchNode = nullptr;
  Ogre::SceneNode* RollNode = nullptr;
  btRigidBody* RigidBody = nullptr;
  Ogre::Camera* OgreCamera = nullptr;
  Ogre::SceneNode* LookTarget = nullptr;
  ControlStyle Style = ControlStyle::MANUAL;
  float RunSpeed = 20.0;
  float ConstSpeed = 5.0;
  float MaxSpeed = RunSpeed;
  bool MoveForward = false;
  bool MoveBack = false;
  bool MoveLeft = false;
  bool MoveRight = false;
  bool MoveUp = false;
  bool MoveDown = false;
  bool MoveFast = false;
  Ogre::Vector3 Velocity{0, 0, 0};
  Ogre::Vector3 PrevPos{0, 0, 0};
};

}  // namespace Glue
