// This source file is part of "glue project". Created by Andrey Vasiliev

#include "PCHeader.h"
#include "Objects/CameraMan.h"
#include <BulletDynamics/Dynamics/btRigidBody.h>

namespace Glue {

CameraMan::CameraMan() {}

CameraMan::~CameraMan() {}

void CameraMan::ManualStop() {
  if (Style == FREELOOK) {
    MoveForward = false;
    MoveBack = false;
    MoveLeft = false;
    MoveRight = false;
    MoveUp = false;
    MoveDown = false;
    Velocity = Ogre::Vector3::ZERO;
  }
}

void CameraMan::Update(float PassedTime) {
  if (Style == FREELOOK) {
    MaxSpeed = MoveFast ? RunSpeed : ConstSpeed;

    Ogre::Vector3 accel = Ogre::Vector3::ZERO;
    Ogre::Matrix3 axes = RollNode->getLocalAxes();

    if (MoveForward) accel -= axes.GetColumn(2);
    if (MoveBack) accel += axes.GetColumn(2);
    if (MoveRight) accel += axes.GetColumn(0);
    if (MoveLeft) accel -= axes.GetColumn(0);
    if (MoveUp) accel += axes.GetColumn(1);
    if (MoveDown) accel -= axes.GetColumn(1);

    if (accel.squaredLength() != 0) {
      accel.normalise();
      Velocity += accel * MaxSpeed * PassedTime;
    } else {
      Velocity -= Velocity * PassedTime;
    }

    if (Velocity.squaredLength() > MaxSpeed * MaxSpeed) {
      Velocity.normalise();
      Velocity *= MaxSpeed;
    }

    if (Velocity != Ogre::Vector3::ZERO) CameraNode->translate(Velocity * PassedTime);
    if (LookTarget) {
      Ogre::Vector3 move = LookTarget->getPosition() - PrevPos;
      PrevPos = LookTarget->getPosition();
      CameraNode->translate(move);
    }
  } else if (Style == FPS) {
    MaxSpeed = MoveFast ? RunSpeed : ConstSpeed;

    Velocity = {0, 0, 0};
    if (MoveForward) Velocity.z = -RunSpeed;
    if (MoveBack) Velocity.z = RunSpeed;
    if (MoveRight) Velocity.x = RunSpeed;
    if (MoveLeft) Velocity.x = -RunSpeed;
    if (MoveUp) Velocity.y = RunSpeed;
    if (MoveDown) Velocity.y = -RunSpeed;

    if (Velocity.squaredLength() > MaxSpeed * MaxSpeed) {
      Velocity.normalise();
      Velocity *= MaxSpeed;
    }

    Ogre::Real pitchAngle;
    Ogre::Real pitchAngleSign;

    Ogre::Vector3 Direction = YawNode->getOrientation() * PitchNode->getOrientation() * Velocity;

    if (RigidBody) {
      if (!Velocity.isZeroLength()) {
        RigidBody->setFriction(1);
        float speed = RigidBody->getLinearVelocity().length();
        if (speed < ConstSpeed) RigidBody->applyCentralForce(btVector3(Direction.x, 0, Direction.z).normalize() * 10000.0f);
      } else {
        RigidBody->setFriction(10);
      }
    }

    // Angle of rotation around the X-axis.
    pitchAngle = (2 * Ogre::Degree(Ogre::Math::ACos(PitchNode->getOrientation().w)).valueDegrees());

    // Just to determine the sign of the angle we pick up above, the
    // value itself does not interest us.
    pitchAngleSign = PitchNode->getOrientation().x;

    // Limit the pitch between -90 degress and +90 degrees, Quake3-style.
    if (pitchAngle > 90.0f) {
      if (pitchAngleSign > 0)
        // Set orientation to 90 degrees on X-axis.
        PitchNode->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f), Ogre::Math::Sqrt(0.5f), 0, 0));
      else if (pitchAngleSign < 0)
        // Sets orientation to -90 degrees on X-axis.
        PitchNode->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f), -Ogre::Math::Sqrt(0.5f), 0, 0));
    }
  }
}

void CameraMan::OnMouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) {
  if (Style == FREELOOK) {
    CameraNode->yaw(Ogre::Degree(-dx));
    CameraNode->pitch(Ogre::Degree(-dy));
  } else if (Style == FPS) {
    YawNode->yaw(Ogre::Degree(-dx));
    PitchNode->pitch(Ogre::Degree(-dy));
  }
}

void CameraMan::OnKeyDown(SDL_Keycode sym) {
  SDL_Scancode code = SDL_GetScancodeFromKey(sym);

  if (Style == FREELOOK) {
    if (code == SDL_SCANCODE_W || sym == SDLK_UP)
      MoveForward = true;
    else if (code == SDL_SCANCODE_S || sym == SDLK_DOWN)
      MoveBack = true;
    else if (code == SDL_SCANCODE_A || sym == SDLK_LEFT)
      MoveLeft = true;
    else if (code == SDL_SCANCODE_D || sym == SDLK_RIGHT)
      MoveRight = true;
    else if (sym == SDLK_PAGEUP)
      MoveUp = true;
    else if (sym == SDLK_PAGEDOWN)
      MoveDown = true;
    else if (sym == SDLK_LSHIFT)
      MoveFast = true;
  } else if (Style == FPS) {
    if (code == SDL_SCANCODE_W || sym == SDLK_UP)
      MoveForward = true;
    else if (code == SDL_SCANCODE_S || sym == SDLK_DOWN)
      MoveBack = true;
    else if (code == SDL_SCANCODE_A || sym == SDLK_LEFT)
      MoveLeft = true;
    else if (code == SDL_SCANCODE_D || sym == SDLK_RIGHT)
      MoveRight = true;
    else if (sym == SDLK_PAGEUP)
      MoveUp = true;
    else if (sym == SDLK_PAGEDOWN)
      MoveDown = true;
    else if (sym == SDLK_LSHIFT)
      MoveFast = true;
  }
}

void CameraMan::OnKeyUp(SDL_Keycode sym) {
  SDL_Scancode code = SDL_GetScancodeFromKey(sym);

  if (Style == FREELOOK) {
    if (code == SDL_SCANCODE_W || sym == SDLK_UP)
      MoveForward = false;
    else if (code == SDL_SCANCODE_S || sym == SDLK_DOWN)
      MoveBack = false;
    else if (code == SDL_SCANCODE_A || sym == SDLK_LEFT)
      MoveLeft = false;
    else if (code == SDL_SCANCODE_D || sym == SDLK_RIGHT)
      MoveRight = false;
    else if (sym == SDLK_PAGEUP)
      MoveUp = false;
    else if (sym == SDLK_PAGEDOWN)
      MoveDown = false;
    else if (sym == SDLK_LSHIFT)
      MoveFast = false;

  } else if (Style == FPS) {
    if (code == SDL_SCANCODE_W || sym == SDLK_UP)
      MoveForward = false;
    else if (code == SDL_SCANCODE_S || sym == SDLK_DOWN)
      MoveBack = false;
    else if (code == SDL_SCANCODE_A || sym == SDLK_LEFT)
      MoveLeft = false;
    else if (code == SDL_SCANCODE_D || sym == SDLK_RIGHT)
      MoveRight = false;
    else if (sym == SDLK_PAGEUP)
      MoveUp = false;
    else if (sym == SDLK_PAGEDOWN)
      MoveDown = false;
    else if (sym == SDLK_LSHIFT)
      MoveFast = false;
  }
}

void CameraMan::SetRigidBody(btRigidBody *RigidBodyPtr) { RigidBody = RigidBodyPtr; }

Ogre::SceneNode *CameraMan::GetCameraNode() const {
  if (Style == FPS) {
    return RollNode;
  } else {
    return CameraNode;
  }
}

Ogre::Camera *CameraMan::GetCamera() const { return OgreCamera; }

void CameraMan::AttachNode(Ogre::SceneNode *ParentPtr, Ogre::SceneNode *ProxyPtr) {
  if (Style == FPS) {
    for (const auto it : ParentPtr->getAttachedObjects()) {
      ParentPtr->detachObject(it);
      RollNode->attachObject(it);
    }

    for (const auto it : ParentPtr->getChildren()) {
      if (it == YawNode) continue;

      ParentPtr->removeChild(it);
      RollNode->removeChild(it);
      RollNode->addChild(it);
    }
  } else if (Style == MANUAL) {
    if (!ProxyPtr || ParentPtr == ProxyPtr) return;

    for (const auto it : ParentPtr->getAttachedObjects()) {
      if (dynamic_cast<Ogre::Camera *>(it)) continue;

      ParentPtr->detachObject(it);
      ProxyPtr->attachObject(it);
    }

    for (const auto it : ParentPtr->getChildren()) {
      ParentPtr->removeChild(it);
      ProxyPtr->removeChild(it);
      ProxyPtr->addChild(it);
    }
  }
}

void CameraMan::AttachCamera(Ogre::Camera *CameraPtr, Ogre::SceneNode *ProxyPtr) {
  auto *ParentPtr = CameraPtr->getParentSceneNode();
  CameraNode = ParentPtr;
  this->OgreCamera = CameraPtr;
  ParentPtr->detachObject(CameraPtr);

  if (Style == FPS) {
    CameraNode->setOrientation(Ogre::Quaternion(90.0, 1.0, 0.0, 1.0));

    // SetUp the camera's yaw node as a child of camera's top node.
    YawNode = ParentPtr->createChildSceneNode("CameraYaw");

    // SetUp the camera's pitch node as a child of camera's yaw node.
    PitchNode = YawNode->createChildSceneNode("CameraPitch");

    // SetUp the camera's roll node as a child of camera's pitch node
    // and attach the camera to it.
    RollNode = PitchNode->createChildSceneNode("CameraRoll");
    RollNode->attachObject(CameraPtr);

    AttachNode(ParentPtr);
  } else {
    CameraNode->attachObject(CameraPtr);
  }

  if (Style == FREELOOK) {
    CameraNode->setFixedYawAxis(true);  // also fix axis with lookAt calls
    CameraNode->setAutoTracking(false);
  } else if (Style == MANUAL) {
    ManualStop();
    CameraNode->setAutoTracking(false);
  } else if (Style == FPS) {
    CameraNode->setFixedYawAxis(true, Ogre::Vector3::UNIT_Z);  // also fix axis with lookAt calls
    CameraNode->setAutoTracking(false);
  }
}

void CameraMan::UnregCamera() {
  RollNode->detachAllObjects();
  PitchNode->detachAllObjects();
  YawNode->detachAllObjects();
  CameraNode->detachAllObjects();
  CameraNode = nullptr;
  YawNode = nullptr;
  PitchNode = nullptr;
  RollNode = nullptr;
}

void CameraMan::SetStyle(ControlStyle style) { Style = style; }

CameraMan::ControlStyle CameraMan::GetStyle() const noexcept { return Style; }

}  // namespace Glue
