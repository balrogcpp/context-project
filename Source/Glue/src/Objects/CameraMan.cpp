// This source file is part of "glue project". Created by Andrey Vasiliev

#include "PCHeader.h"
#include "Objects/CameraMan.h"
#include <BulletDynamics/Dynamics/btRigidBody.h>

namespace Glue {

CameraMan::CameraMan() {}

CameraMan::~CameraMan() {}

void CameraMan::ManualStop() {
  if (Style == FREELOOK) {
    move_forward = false;
    move_back = false;
    move_left = false;
    move_right = false;
    move_up = false;
    move_down = false;
    Velocity = Ogre::Vector3::ZERO;
  }
}

void CameraMan::Update(float PassedTime) {
  if (Style == FREELOOK) {
    top_speed = move_fast ? run_speed : const_speed;

    Ogre::Vector3 accel = Ogre::Vector3::ZERO;
    Ogre::Matrix3 axes = roll_node->getLocalAxes();

    if (move_forward) accel -= axes.GetColumn(2);
    if (move_back) accel += axes.GetColumn(2);
    if (move_right) accel += axes.GetColumn(0);
    if (move_left) accel -= axes.GetColumn(0);
    if (move_up) accel += axes.GetColumn(1);
    if (move_down) accel -= axes.GetColumn(1);

    if (accel.squaredLength() != 0) {
      accel.normalise();
      Velocity += accel * top_speed * PassedTime;
    } else {
      Velocity -= Velocity * PassedTime;
    }

    if (Velocity.squaredLength() > top_speed * top_speed) {
      Velocity.normalise();
      Velocity *= top_speed;
    }

    if (Velocity != Ogre::Vector3::ZERO) CameraNode->translate(Velocity * PassedTime);
    if (target) {
      Ogre::Vector3 move = target->getPosition() - prev_pos;
      prev_pos = target->getPosition();
      CameraNode->translate(move);
    }
  } else if (Style == FPS) {
    top_speed = move_fast ? run_speed : const_speed;

    Velocity = {0, 0, 0};
    if (move_forward) Velocity.z = -run_speed;
    if (move_back) Velocity.z = run_speed;
    if (move_right) Velocity.x = run_speed;
    if (move_left) Velocity.x = -run_speed;
    if (move_up) Velocity.y = run_speed;
    if (move_down) Velocity.y = -run_speed;

    if (Velocity.squaredLength() > top_speed * top_speed) {
      Velocity.normalise();
      Velocity *= top_speed;
    }

    Ogre::Real pitchAngle;
    Ogre::Real pitchAngleSign;

    Ogre::Vector3 Direction = yaw_node->getOrientation() * pitch_node->getOrientation() * Velocity;

    if (RigidBody) {
      if (!Velocity.isZeroLength()) {
        RigidBody->setFriction(1);
        float speed = RigidBody->getLinearVelocity().length();
        if (speed < const_speed) RigidBody->applyCentralForce(btVector3(Direction.x, 0, Direction.z).normalize() * 10000.0f);
      } else {
        RigidBody->setFriction(10);
      }

      //float speed = RigidBody->getLinearVelocity().length();
      //if (speed > run_speed) {
      //  animation_time += PassedTime;
      //} else {
      //  animation_time -= PassedTime;
      //}

      //if (animation_time > anim_duration) {
      //  animation_time = anim_duration;
      //} else if (animation_time < 0.0f) {
      //  animation_time = 0.0f;
      //}
    }

    // Angle of rotation around the X-axis.
    pitchAngle = (2 * Ogre::Degree(Ogre::Math::ACos(pitch_node->getOrientation().w)).valueDegrees());

    // Just to determine the sign of the angle we pick up above, the
    // value itself does not interest us.
    pitchAngleSign = pitch_node->getOrientation().x;

    // Limit the pitch between -90 degress and +90 degrees, Quake3-style.
    if (pitchAngle > 90.0f) {
      if (pitchAngleSign > 0)
        // Set orientation to 90 degrees on X-axis.
        pitch_node->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f), Ogre::Math::Sqrt(0.5f), 0, 0));
      else if (pitchAngleSign < 0)
        // Sets orientation to -90 degrees on X-axis.
        pitch_node->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f), -Ogre::Math::Sqrt(0.5f), 0, 0));
    }
  }
}

void CameraMan::OnMouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) {
  if (!stop) {
    if (Style == FREELOOK) {
      CameraNode->yaw(Ogre::Degree(-dx));
      CameraNode->pitch(Ogre::Degree(-dy));
    } else if (Style == FPS) {
      yaw_node->yaw(Ogre::Degree(-dx));
      pitch_node->pitch(Ogre::Degree(-dy));
    }
  }
}

void CameraMan::OnKeyDown(SDL_Keycode sym) {
  SDL_Scancode code = SDL_GetScancodeFromKey(sym);

  if (!stop) {
    if (Style == FREELOOK) {
      if (code == SDL_SCANCODE_W || sym == SDLK_UP)
        move_forward = true;
      else if (code == SDL_SCANCODE_S || sym == SDLK_DOWN)
        move_back = true;
      else if (code == SDL_SCANCODE_A || sym == SDLK_LEFT)
        move_left = true;
      else if (code == SDL_SCANCODE_D || sym == SDLK_RIGHT)
        move_right = true;
      else if (sym == SDLK_PAGEUP)
        move_up = true;
      else if (sym == SDLK_PAGEDOWN)
        move_down = true;
      else if (sym == SDLK_LSHIFT)
        move_fast = true;
    } else if (Style == FPS) {
      if (code == SDL_SCANCODE_W || sym == SDLK_UP)
        move_forward = true;
      else if (code == SDL_SCANCODE_S || sym == SDLK_DOWN)
        move_back = true;
      else if (code == SDL_SCANCODE_A || sym == SDLK_LEFT)
        move_left = true;
      else if (code == SDL_SCANCODE_D || sym == SDLK_RIGHT)
        move_right = true;
      else if (sym == SDLK_PAGEUP)
        move_up = true;
      else if (sym == SDLK_PAGEDOWN)
        move_down = true;
      else if (sym == SDLK_LSHIFT)
        move_fast = true;
    }
  }
}

void CameraMan::OnKeyUp(SDL_Keycode sym) {
  SDL_Scancode code = SDL_GetScancodeFromKey(sym);

  if (!stop) {
    if (Style == FREELOOK) {
      if (code == SDL_SCANCODE_W || sym == SDLK_UP)
        move_forward = false;
      else if (code == SDL_SCANCODE_S || sym == SDLK_DOWN)
        move_back = false;
      else if (code == SDL_SCANCODE_A || sym == SDLK_LEFT)
        move_left = false;
      else if (code == SDL_SCANCODE_D || sym == SDLK_RIGHT)
        move_right = false;
      else if (sym == SDLK_PAGEUP)
        move_up = false;
      else if (sym == SDLK_PAGEDOWN)
        move_down = false;
      else if (sym == SDLK_LSHIFT)
        move_fast = false;

    } else if (Style == FPS) {
      if (code == SDL_SCANCODE_W || sym == SDLK_UP)
        move_forward = false;
      else if (code == SDL_SCANCODE_S || sym == SDLK_DOWN)
        move_back = false;
      else if (code == SDL_SCANCODE_A || sym == SDLK_LEFT)
        move_left = false;
      else if (code == SDL_SCANCODE_D || sym == SDLK_RIGHT)
        move_right = false;
      else if (sym == SDLK_PAGEUP)
        move_up = false;
      else if (sym == SDLK_PAGEDOWN)
        move_down = false;
      else if (sym == SDLK_LSHIFT)
        move_fast = false;
    }
  }
}

void CameraMan::SetRigidBody(btRigidBody *RigidBodyPtr) { RigidBody = RigidBodyPtr; }

Ogre::SceneNode *CameraMan::GetCameraNode() const {
  if (Style == FPS) {
    return roll_node;
  } else {
    return CameraNode;
  }
}

Ogre::Camera *CameraMan::GetCamera() const { return OgreCamera; }

void CameraMan::AttachNode(Ogre::SceneNode *ParentPtr, Ogre::SceneNode *ProxyPtr) {
  if (Style == FPS) {
    for (const auto it : ParentPtr->getAttachedObjects()) {
      ParentPtr->detachObject(it);
      roll_node->attachObject(it);
    }

    for (const auto it : ParentPtr->getChildren()) {
      if (it == yaw_node) continue;

      ParentPtr->removeChild(it);
      roll_node->removeChild(it);
      roll_node->addChild(it);
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
    yaw_node = ParentPtr->createChildSceneNode("CameraYaw");

    // SetUp the camera's pitch node as a child of camera's yaw node.
    pitch_node = yaw_node->createChildSceneNode("CameraPitch");

    // SetUp the camera's roll node as a child of camera's pitch node
    // and attach the camera to it.
    roll_node = pitch_node->createChildSceneNode("CameraRoll");
    roll_node->attachObject(CameraPtr);

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
  roll_node->detachAllObjects();
  pitch_node->detachAllObjects();
  yaw_node->detachAllObjects();
  CameraNode->detachAllObjects();
  CameraNode = nullptr;
  yaw_node = nullptr;
  pitch_node = nullptr;
  roll_node = nullptr;
}

void CameraMan::SetStyle(ControlStyle style) { Style = style; }

CameraMan::ControlStyle CameraMan::GetStyle() const noexcept { return Style; }

}  // namespace Glue
