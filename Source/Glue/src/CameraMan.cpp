// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "CameraMan.h"
#include <BulletDynamics/Dynamics/btRigidBody.h>

namespace Glue {

CameraMan::CameraMan() {}

CameraMan::~CameraMan() {}

void CameraMan::ManualStop() {
  if (style == Style::FREELOOK) {
    move_forward = false;
    move_back = false;
    move_left = false;
    move_right = false;
    move_up = false;
    move_down = false;
    velocity = Ogre::Vector3::ZERO;
  }
}

void CameraMan::Update(float PassedTime) {
  if (style == Style::FREELOOK) {
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
      velocity += accel * top_speed * PassedTime;
    } else {
      velocity -= velocity * PassedTime;
    }

    if (velocity.squaredLength() > top_speed * top_speed) {
      velocity.normalise();
      velocity *= top_speed;
    }

    if (velocity != Ogre::Vector3::ZERO) node->translate(velocity * PassedTime);
    if (target) {
      Ogre::Vector3 move = target->getPosition() - prev_pos;
      prev_pos = target->getPosition();
      node->translate(move);
    }
  } else if (style == Style::FPS) {
    top_speed = move_fast ? run_speed : const_speed;

    velocity = {0, 0, 0};
    if (move_forward) velocity.z = -run_speed;
    if (move_back) velocity.z = run_speed;
    if (move_right) velocity.x = run_speed;
    if (move_left) velocity.x = -run_speed;
    if (move_up) velocity.y = run_speed;
    if (move_down) velocity.y = -run_speed;

    if (velocity.squaredLength() > top_speed * top_speed) {
      velocity.normalise();
      velocity *= top_speed;
    }

    Ogre::Real pitchAngle;
    Ogre::Real pitchAngleSign;

    Ogre::Vector3 velocity = yaw_node->getOrientation() * pitch_node->getOrientation() * velocity;

    if (rigid) {
      if (!velocity.isZeroLength()) {
        rigid->setFriction(1.0);
        float speed = rigid->getLinearVelocity().length();
        if (speed < const_speed) rigid->applyCentralForce(btVector3(velocity.x, 0, velocity.z).normalize() * 10000.0f);
      } else {
        rigid->setFriction(10.0);
      }
    }

    float speed = rigid->getLinearVelocity().length();
    if (speed > (run_speed + run_speed) / 2.0f) {
      animation_time += PassedTime;
    } else {
      animation_time -= PassedTime;
    }

    if (animation_time > anim_duration) {
      animation_time = anim_duration;
    } else if (animation_time < 0.0f) {
      animation_time = 0.0f;
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
    if (style == Style::FREELOOK) {
      node->yaw(Ogre::Degree(-dx));
      node->pitch(Ogre::Degree(-dy));
    } else if (style == Style::FPS) {
      yaw_node->yaw(Ogre::Degree(-dx));
      pitch_node->pitch(Ogre::Degree(-dy));
    }
  }
}

void CameraMan::OnKeyDown(SDL_Keycode sym) {
  SDL_Scancode code = SDL_GetScancodeFromKey(sym);

  if (!stop) {
    if (style == Style::FREELOOK) {
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
    } else if (style == Style::FPS) {
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
    if (style == Style::FREELOOK) {
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

    } else if (style == Style::FPS) {
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

void CameraMan::SetRigidBody(btRigidBody *RigidBodyPtr) { rigid = RigidBodyPtr; }

Ogre::SceneNode *CameraMan::GetCameraNode() const {
  if (style == Style::FPS) {
    return roll_node;
  } else {
    return node;
  }
}

Ogre::Camera *CameraMan::GetCamera() const { return camera; }

void CameraMan::AttachNode(Ogre::SceneNode *ParentPtr, Ogre::SceneNode *ProxyPtr) {
  if (style == Style::FPS) {
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
  } else if (style == Style::MANUAL) {
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

void CameraMan::AttachCamera(Ogre::SceneNode *ParentPtr, Ogre::Camera *CameraPtr, Ogre::SceneNode *ProxyPtr) {
  node = ParentPtr;
  this->camera = CameraPtr;

  if (style == Style::FPS) {
    node->setOrientation(Ogre::Quaternion(90.0, 1.0, 0.0, 1.0));

    // SetUp the camera's yaw node as a child of camera's top node.
    yaw_node = ParentPtr->createChildSceneNode("CameraYaw");

    // SetUp the camera's pitch node as a child of camera's yaw node.
    pitch_node = yaw_node->createChildSceneNode("CameraPitch");

    // SetUp the camera's roll node as a child of camera's pitch node
    // and attach the camera to it.
    roll_node = pitch_node->createChildSceneNode("CameraRoll");
    roll_node->attachObject(CameraPtr);
  } else {
    node->attachObject(CameraPtr);
  }

  if (style == Style::FREELOOK) {
    node->setFixedYawAxis(true);  // also fix axis with lookAt calls
    node->setAutoTracking(false);
  } else if (style == Style::MANUAL) {
    ManualStop();
    node->setAutoTracking(false);
  } else if (style == Style::FPS) {
    node->setFixedYawAxis(true, Ogre::Vector3::UNIT_Z);  // also fix axis with lookAt calls
    node->setAutoTracking(false);
  }
}

void CameraMan::UnregCamera() {
  roll_node->detachAllObjects();
  pitch_node->detachAllObjects();
  yaw_node->detachAllObjects();
  node->detachAllObjects();
  node = nullptr;
  yaw_node = nullptr;
  pitch_node = nullptr;
  roll_node = nullptr;
}

void CameraMan::SetStyle(Style style) { style = style; }

CameraMan::Style CameraMan::GetStyle() const noexcept { return style; }

}  // namespace Glue
