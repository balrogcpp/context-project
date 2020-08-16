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

#include "pcheader.hpp"

#include "ContextManager.hpp"
#include "InputManager.hpp"
#include "CameraMan.hpp"
#include "DotSceneLoaderB.hpp"

namespace Context {

//----------------------------------------------------------------------------------------------------------------------
CameraMan::CameraMan() {
  InputManager::GetSingleton().RegisterListener(this);
  ContextManager::GetSingleton().GetOgreRootPtr()->addFrameListener(this);
}
//----------------------------------------------------------------------------------------------------------------------
CameraMan::CameraMan(Ogre::SceneNode *ogre_camera_node)
    : camera_node_(ogre_camera_node) {
  ContextManager::GetSingleton().GetOgreRootPtr()->addFrameListener(this);
}
//----------------------------------------------------------------------------------------------------------------------
CameraMan::~CameraMan() {
  ContextManager::GetSingleton().GetOgreRootPtr()->removeFrameListener(this);
}
//----------------------------------------------------------------------------------------------------------------------
void CameraMan::SetTarget(Ogre::SceneNode *target) {
  if (this->target_ != target) {
    this->target_ = target;
    if (target) {
      SetYawPitchDist(Ogre::Degree(0), Ogre::Degree(45), 150);
      camera_node_->setAutoTracking(true, target);
    } else {
      camera_node_->setAutoTracking(false);
    }
    prev_pos_ = target->getPosition();
  }
}
//----------------------------------------------------------------------------------------------------------------------
Ogre::SceneNode *CameraMan::GetTarget() {
  return target_;
}
//----------------------------------------------------------------------------------------------------------------------
void CameraMan::SetTopSpeed(float top_speed) {
  this->top_speed_ = top_speed;
  this->const_speed_ = top_speed;
}
//----------------------------------------------------------------------------------------------------------------------
float CameraMan::GetTopSpeed() {
  return top_speed_;
}
//----------------------------------------------------------------------------------------------------------------------
float CameraMan::GetDistToTarget() {
  if (target_) {
    Ogre::Vector3 offset = camera_node_->getPosition() - target_->_getDerivedPosition() - offset;
    return offset.length();
  } else {
    Ogre::Vector3 offset = camera_node_->getPosition() - offset;
    return offset.length();
  }
}
//----------------------------------------------------------------------------------------------------------------------
void CameraMan::SetStyle(CameraStyle style) {
  if (this->camera_style_ != CameraStyle::ORBIT && style == CameraStyle::ORBIT) {
    camera_node_->setFixedYawAxis(true, Ogre::Vector3::UNIT_Z); // also fix axis with lookAt calls
//    ogre_camera_node_->setFixedYawAxis(true, Ogre::Vector3::UNIT_X); // also fix axis with lookAt calls

    // try to replicate the camera configuration
    float dist = GetDistToTarget();
    const Ogre::Quaternion &q = camera_node_->getOrientation();
    SetYawPitchDist(q.getYaw(), q.getPitch(), dist == 0 ? 400 : dist); // enforce some distance
    camera_node_->setAutoTracking(true, target_);
    moving_ = true;
  } else if (this->camera_style_ != CameraStyle::FREELOOK && style == CameraStyle::FREELOOK) {
    camera_node_->setFixedYawAxis(true); // also fix axis with lookAt calls
    camera_node_->setAutoTracking(false);
  } else if (this->camera_style_ != CameraStyle::MANUAL && style == CameraStyle::MANUAL) {
    ManualStop();
    camera_node_->setAutoTracking(false);
  } else if (this->camera_style_ != CameraStyle::FPS && style == CameraStyle::FPS) {
    camera_node_->setFixedYawAxis(true, Ogre::Vector3::UNIT_Z); // also fix axis with lookAt calls
    camera_node_->setAutoTracking(false);
  }

  this->camera_style_ = style;
}
//----------------------------------------------------------------------------------------------------------------------
CameraStyle CameraMan::GetStyle() {
  return camera_style_;
}
//----------------------------------------------------------------------------------------------------------------------
void CameraMan::Freeze(bool status) {
  this->stop_ = status;
}
//----------------------------------------------------------------------------------------------------------------------
void CameraMan::RegCamera(Ogre::SceneNode *ogre_camera_node) {
  this->camera_node_ = ogre_camera_node;

  this->ogre_camera_ = ContextManager::GetSingleton().GetOgreCamera();
  // Create the camera's yaw node as a child of camera's top node.
  this->camera_yaw_node_ = this->camera_node_->createChildSceneNode();

  // Create the camera's pitch node as a child of camera's yaw node.
  this->camera_pitch_node_ = this->camera_yaw_node_->createChildSceneNode();

  // Create the camera's roll node as a child of camera's pitch node
  // and attach the camera to it.
  this->camera_roll_node_ = this->camera_pitch_node_->createChildSceneNode();
  this->camera_roll_node_->attachObject(this->ogre_camera_);
  if (this->camera_style_ == CameraStyle::FPS) {
    ogre_camera_node->setOrientation(Ogre::Quaternion(90.0, 1.0, 0.0, 1.0));
  }
}
//----------------------------------------------------------------------------------------------------------------------
void CameraMan::UnregCamera() {
  this->camera_roll_node_->detachAllObjects();
  this->camera_pitch_node_->detachAllObjects();
  this->camera_yaw_node_->detachAllObjects();
  this->camera_node_->detachAllObjects();
  camera_node_ = nullptr;
  camera_yaw_node_ = nullptr;
  camera_pitch_node_ = nullptr;
  camera_roll_node_ = nullptr;
}
//----------------------------------------------------------------------------------------------------------------------
Ogre::SceneNode *CameraMan::GetCameraNode() {
  if (camera_style_ == CameraStyle::FPS) {
    return camera_roll_node_;
  } else {
    return camera_node_;
  }

  return nullptr;
}
//----------------------------------------------------------------------------------------------------------------------
void CameraMan::SetYawPitchDist(Ogre::Radian yaw, Ogre::Radian pitch, float dist) {
  if (target_) {
    camera_node_->setPosition(target_->_getDerivedPosition());
    camera_node_->setOrientation(target_->_getDerivedOrientation());
  }
  camera_node_->yaw(yaw);
  camera_node_->pitch(-pitch);
  camera_node_->translate(Ogre::Vector3(0, dist, 0), Ogre::Node::TS_LOCAL);
}
//----------------------------------------------------------------------------------------------------------------------
void CameraMan::ManualStop() {
  if (camera_style_ == CameraStyle::FREELOOK) {
    move_forward_ = false;
    move_back_ = false;
    move_left_ = false;
    move_right_ = false;
    move_up_ = false;
    move_down_ = false;
    velocity_ = Ogre::Vector3::ZERO;
  }
}
//----------------------------------------------------------------------------------------------------------------------
bool CameraMan::frameRenderingQueued(const Ogre::FrameEvent &evt) {
  float timeSinceLastFrame = evt.timeSinceLastFrame;

  if (camera_style_ == CameraStyle::FREELOOK) {
    top_speed_ = move_fast_ ? run_speed_ : const_speed_;

    Ogre::Vector3 accel = Ogre::Vector3::ZERO;
    Ogre::Matrix3 axes = camera_roll_node_->getLocalAxes();

    if (move_forward_) {
      accel -= axes.GetColumn(2);
    }
    if (move_back_) {
      accel += axes.GetColumn(2);
    }
    if (move_right_) {
      accel += axes.GetColumn(0);
    }
    if (move_left_) {
      accel -= axes.GetColumn(0);
    }
    if (move_up_) {
      accel += axes.GetColumn(1);
    }
    if (move_down_) {
      accel -= axes.GetColumn(1);
    }

    if (accel.squaredLength() != 0) {
      accel.normalise();
      velocity_ += accel * top_speed_ * timeSinceLastFrame * 10;
    } else {
      velocity_ -= velocity_ * timeSinceLastFrame * 10;
    }

    if (velocity_.squaredLength() > top_speed_ * top_speed_) {
      velocity_.normalise();
      velocity_ *= top_speed_;
    }

    if (velocity_ != Ogre::Vector3::ZERO) {
      camera_node_->translate(velocity_ * timeSinceLastFrame);
      float x = camera_node_->getPosition().x;
      float z = camera_node_->getPosition().z;
    }
    if (target_) {
      Ogre::Vector3 move = target_->getPosition() - prev_pos_;
      prev_pos_ = target_->getPosition();
      camera_node_->translate(move);
    }

  } else if (camera_style_ == CameraStyle::FPS) {
    top_speed_ = move_fast_ ? run_speed_ : const_speed_;

    velocity_ = {0, 0, 0};
    if (move_forward_) {
      velocity_.z = -run_speed_;
    }
    if (move_back_) {
      velocity_.z = run_speed_;
    }
    if (move_right_) {
      velocity_.x = run_speed_;
    }
    if (move_left_) {
      velocity_.x = -run_speed_;
    }
    if (move_up_) {
      velocity_.y = run_speed_;
    }
    if (move_down_) {
      velocity_.y = -run_speed_;
    }

    if (velocity_.squaredLength() > top_speed_ * top_speed_) {
      velocity_.normalise();
      velocity_ *= top_speed_;
    }

    Ogre::Real pitchAngle;
    Ogre::Real pitchAngleSign;

    // Yaws the camera according to the mouse relative movement.
    this->camera_yaw_node_->yaw(-dx_);

    // Pitches the camera according to the mouse relative movement.
    this->camera_pitch_node_->pitch(-dy_);

    // Translates the camera according to the translate vector which is
    // controlled by the keyboard arrows.
    //
    // NOTE: We multiply the mTranslateVector by the cameraPitchNode's
    // orientation quaternion and the cameraYawNode's orientation
    // quaternion to translate the camera accoding to the camera's
    // orientation around the Y-axis and the X-axis.
    Ogre::Vector3 velocity = camera_yaw_node_->getOrientation() *
                                    camera_pitch_node_->getOrientation() *
                                    velocity_;
    if (rigid_body_) {
      if (!velocity_.isZeroLength()) {
        rigid_body_->setLinearVelocity({velocity.x, velocity.y, velocity.z});
//        rigid_body_->applyCentralImpulse({velocity.x, velocity.y, velocity.z});
        rigid_body_->setFriction(0.0);
      } else {
        rigid_body_->setFriction(10.0);
      }
    }

    float speed = rigid_body_->getLinearVelocity().length();

    if (speed > (run_speed_ + run_speed_) / 2.0f) {
      animation_time_ += evt.timeSinceLastFrame;
    }
    else {
      animation_time_ -= evt.timeSinceLastFrame;
    }

    if (animation_time_ > anim_duration_) {
      animation_time_ = anim_duration_;
    } else if (animation_time_ < 0.0f) {
      animation_time_ = 0.0f;
    }

//    std::cout << speed << std::endl;

//    ogre_camera_->setFOVy(Ogre::Radian(1.0 + 0.25 * (animation_time_ / anim_duration_)));

    // Angle of rotation around the X-axis.
    pitchAngle = (2 * Ogre::Degree(Ogre::Math::ACos(this->camera_pitch_node_->getOrientation().w)).valueDegrees());

    // Just to determine the sign of the angle we pick up above, the
    // value itself does not interest us.
    pitchAngleSign = this->camera_pitch_node_->getOrientation().x;

    // Limit the pitch between -90 degress and +90 degrees, Quake3-style.
    if (pitchAngle > 90.0f) {
      if (pitchAngleSign > 0)
        // Set orientation to 90 degrees on X-axis.
        this->camera_pitch_node_->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f),
                                                                  Ogre::Math::Sqrt(0.5f), 0, 0));
      else if (pitchAngleSign < 0)
        // Sets orientation to -90 degrees on X-axis.
        this->camera_pitch_node_->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f),
                                                                  -Ogre::Math::Sqrt(0.5f), 0, 0));
    }

    dx_ = 0;
    dy_ = 0;

  } else if (camera_style_ == CameraStyle::ORBIT) {
    Ogre::Vector3 move = camera_node_->getPosition() - prev_pos_;
    prev_pos_ = camera_node_->getPosition();
    camera_node_->translate(move);

  }

  return true;
}
//----------------------------------------------------------------------------------------------------------------------
void CameraMan::Move(int x, int y, int dx, int dy, bool left, bool right, bool middle) {
  if (!stop_) {
    if (camera_style_ == CameraStyle::FREELOOK) {
      camera_node_->yaw(Ogre::Degree(-dx));
      camera_node_->pitch(Ogre::Degree(-dy));

      if (target_) {
        float dist = GetDistToTarget();
        Ogre::Vector3 delta = camera_node_->getOrientation() * Ogre::Vector3(-dx, dy, 0);

        // the further the camera is, the faster it moves
        delta *= dist / 1000.0f;
        offset_ += delta;
      }
    } else if (camera_style_ == CameraStyle::FPS) {
      dx_ = Ogre::Degree(dx);
      dy_ = Ogre::Degree(dy);
    } else if (camera_style_ == CameraStyle::ORBIT) {
      float dist = GetDistToTarget();

      if (orbiting_)   // yaw around the target, and pitch locally
      {
        camera_node_->setPosition(target_->_getDerivedPosition() + offset_);

        camera_node_->yaw(Ogre::Degree(-dx * 0.25f), Ogre::Node::TS_PARENT);
        camera_node_->pitch(Ogre::Degree(-dy * 0.25f));

        camera_node_->translate(Ogre::Vector3(0, 0, dist), Ogre::Node::TS_LOCAL);
        // don't let the camera Go over the top or around the bottom of the target
      } else if (moving_)  // move the camera along the image plane
      {
        Ogre::Vector3 delta = camera_node_->getOrientation() * Ogre::Vector3(-dx, dy, 0);

        // the further the camera is, the faster it moves
        delta *= dist / 1000.0f;
        offset_ += delta;

      }
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void CameraMan::LbDown(int x, int y) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void CameraMan::LbUp(int x, int y) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void CameraMan::RbDown(int x, int y) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void CameraMan::RbUp(int x, int y) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void CameraMan::MbDown(int x, int y) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void CameraMan::MbUp(int x, int y) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void CameraMan::KeyDown(SDL_Keycode sym) {
  SDL_Scancode code = SDL_GetScancodeFromKey(sym);

  if (!stop_) {
    if (camera_style_ == CameraStyle::FREELOOK) {
      if (code == SDL_SCANCODE_W || sym == SDLK_UP)
        move_forward_ = true;
      else if (code == SDL_SCANCODE_S || sym == SDLK_DOWN)
        move_back_ = true;
      else if (code == SDL_SCANCODE_A || sym == SDLK_LEFT)
        move_left_ = true;
      else if (code == SDL_SCANCODE_D || sym == SDLK_RIGHT)
        move_right_ = true;
      else if (sym == SDLK_PAGEUP)
        move_up_ = true;
      else if (sym == SDLK_PAGEDOWN)
        move_down_ = true;
      else if (sym == SDLK_LSHIFT)
        move_fast_ = true;
    } else if (camera_style_ == CameraStyle::FPS) {
      if (code == SDL_SCANCODE_W || sym == SDLK_UP)
        move_forward_ = true;
      else if (code == SDL_SCANCODE_S || sym == SDLK_DOWN)
        move_back_ = true;
      else if (code == SDL_SCANCODE_A || sym == SDLK_LEFT)
        move_left_ = true;
      else if (code == SDL_SCANCODE_D || sym == SDLK_RIGHT)
        move_right_ = true;
      else if (sym == SDLK_PAGEUP)
        move_up_ = true;
      else if (sym == SDLK_PAGEDOWN)
        move_down_ = true;
      else if (sym == SDLK_LSHIFT)
        move_fast_ = true;

    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void CameraMan::KeyUp(SDL_Keycode sym) {
  SDL_Scancode code = SDL_GetScancodeFromKey(sym);

  if (!stop_) {
    if (camera_style_ == CameraStyle::FREELOOK) {
      if (code == SDL_SCANCODE_W || sym == SDLK_UP)
        move_forward_ = false;
      else if (code == SDL_SCANCODE_S || sym == SDLK_DOWN)
        move_back_ = false;
      else if (code == SDL_SCANCODE_A || sym == SDLK_LEFT)
        move_left_ = false;
      else if (code == SDL_SCANCODE_D || sym == SDLK_RIGHT)
        move_right_ = false;
      else if (sym == SDLK_PAGEUP)
        move_up_ = false;
      else if (sym == SDLK_PAGEDOWN)
        move_down_ = false;
      else if (sym == SDLK_LSHIFT)
        move_fast_ = false;

    } else if (camera_style_ == CameraStyle::FPS) {
      if (code == SDL_SCANCODE_W || sym == SDLK_UP)
        move_forward_ = false;
      else if (code == SDL_SCANCODE_S || sym == SDLK_DOWN)
        move_back_ = false;
      else if (code == SDL_SCANCODE_A || sym == SDLK_LEFT)
        move_left_ = false;
      else if (code == SDL_SCANCODE_D || sym == SDLK_RIGHT)
        move_right_ = false;
      else if (sym == SDLK_PAGEUP)
        move_up_ = false;
      else if (sym == SDLK_PAGEDOWN)
        move_down_ = false;
      else if (sym == SDLK_LSHIFT)
        move_fast_ = false;

    }
  }
}
btRigidBody *CameraMan::GetRigidBody() const {
  if (camera_style_ == CameraStyle::FPS)
    return rigid_body_;
  else
    return nullptr;
}
void CameraMan::SetRigidBody(btRigidBody *rigid_body) {
  if (camera_style_ == CameraStyle::FPS)
    rigid_body_ = rigid_body;
}

} //namespace Context
