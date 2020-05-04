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
    : cameraNode(ogre_camera_node) {
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
      cameraNode->setAutoTracking(true, target);
    } else {
      cameraNode->setAutoTracking(false);
    }
    prevPos = target->getPosition();
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
    Ogre::Vector3 offset = cameraNode->getPosition() - target_->_getDerivedPosition() - offset;
    return offset.length();
  } else {
    Ogre::Vector3 offset = cameraNode->getPosition() - offset;
    return offset.length();
  }
}
//----------------------------------------------------------------------------------------------------------------------
void CameraMan::SetStyle(CameraStyle style) {
  if (this->camera_style_ != CameraStyle::ORBIT && style == CameraStyle::ORBIT) {
    cameraNode->setFixedYawAxis(true, Ogre::Vector3::UNIT_Z); // also fix axis with lookAt calls
//    ogre_camera_node_->setFixedYawAxis(true, Ogre::Vector3::UNIT_X); // also fix axis with lookAt calls

    // try to replicate the camera configuration
    float dist = GetDistToTarget();
    const Ogre::Quaternion &q = cameraNode->getOrientation();
    SetYawPitchDist(q.getYaw(), q.getPitch(), dist == 0 ? 400 : dist); // enforce some distance
    cameraNode->setAutoTracking(true, target_);
    moving_ = true;
  } else if (this->camera_style_ != CameraStyle::FREELOOK && style == CameraStyle::FREELOOK) {
    cameraNode->setFixedYawAxis(true); // also fix axis with lookAt calls
    cameraNode->setAutoTracking(false);
  } else if (this->camera_style_ != CameraStyle::MANUAL && style == CameraStyle::MANUAL) {
    ManualStop();
    cameraNode->setAutoTracking(false);
  } else if (this->camera_style_ != CameraStyle::FPS && style == CameraStyle::FPS) {
    cameraNode->setFixedYawAxis(true, Ogre::Vector3::UNIT_Z); // also fix axis with lookAt calls
    cameraNode->setAutoTracking(false);
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
  this->cameraNode = ogre_camera_node;

  this->camera = ContextManager::GetSingleton().GetOgreCamera();
  // Create the camera's yaw node as a child of camera's top node.
  this->cameraYawNode = this->cameraNode->createChildSceneNode();

  // Create the camera's pitch node as a child of camera's yaw node.
  this->cameraPitchNode = this->cameraYawNode->createChildSceneNode();

  // Create the camera's roll node as a child of camera's pitch node
  // and attach the camera to it.
  this->cameraRollNode = this->cameraPitchNode->createChildSceneNode();
  this->cameraRollNode->attachObject(this->camera);
  if (this->camera_style_ == CameraStyle::FPS) {
    ogre_camera_node->setOrientation(Ogre::Quaternion(90.0, 1.0, 0.0, 1.0));
  }
}
//----------------------------------------------------------------------------------------------------------------------
void CameraMan::UnregCamera() {
  this->cameraRollNode->detachAllObjects();
  this->cameraPitchNode->detachAllObjects();
  this->cameraYawNode->detachAllObjects();
  this->cameraNode->detachAllObjects();
  cameraNode = nullptr;
  cameraYawNode = nullptr;
  cameraPitchNode = nullptr;
  cameraRollNode = nullptr;
}
//----------------------------------------------------------------------------------------------------------------------
Ogre::SceneNode *CameraMan::GetCameraNode() {
  if (camera_style_ == CameraStyle::FPS) {
    return cameraRollNode;
  } else {
    return cameraNode;
  }

  return nullptr;
}
//----------------------------------------------------------------------------------------------------------------------
void CameraMan::SetYawPitchDist(Ogre::Radian yaw, Ogre::Radian pitch, float dist) {
  if (target_) {
    cameraNode->setPosition(target_->_getDerivedPosition());
    cameraNode->setOrientation(target_->_getDerivedOrientation());
  }
  cameraNode->yaw(yaw);
  cameraNode->pitch(-pitch);
  cameraNode->translate(Ogre::Vector3(0, dist, 0), Ogre::Node::TS_LOCAL);
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
    velocity = Ogre::Vector3::ZERO;
  }
}
//----------------------------------------------------------------------------------------------------------------------
bool CameraMan::frameRenderingQueued(const Ogre::FrameEvent &evt) {
  float timeSinceLastFrame = evt.timeSinceLastFrame;

  if (camera_style_ == CameraStyle::FREELOOK) {
    top_speed_ = move_fast_ ? run_speed_ : const_speed_;

    Ogre::Vector3 accel = Ogre::Vector3::ZERO;
    Ogre::Matrix3 axes = cameraRollNode->getLocalAxes();

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
      velocity += accel * top_speed_ * timeSinceLastFrame * 10;
    } else {
      velocity -= velocity * timeSinceLastFrame * 10;
    }

    if (velocity.squaredLength() > top_speed_ * top_speed_) {
      velocity.normalise();
      velocity *= top_speed_;
    }

    if (velocity != Ogre::Vector3::ZERO) {
      cameraNode->translate(velocity * timeSinceLastFrame);
      float x = cameraNode->getPosition().x;
      float z = cameraNode->getPosition().z;
    }
    if (target_) {
      Ogre::Vector3 move = target_->getPosition() - prevPos;
      prevPos = target_->getPosition();
      cameraNode->translate(move);
    }

  } else if (camera_style_ == CameraStyle::FPS) {
    top_speed_ = move_fast_ ? run_speed_ : const_speed_;

    velocity = {0, 0, 0};
    if (move_forward_) {
      velocity.z = -run_speed_;
    }
    if (move_back_) {
      velocity.z = run_speed_;
    }
    if (move_right_) {
      velocity.x = run_speed_;
    }
    if (move_left_) {
      velocity.x = -run_speed_;
    }
    if (move_up_) {
      velocity.y = run_speed_;
    }
    if (move_down_) {
      velocity.y = -run_speed_;
    }

    if (velocity.squaredLength() > top_speed_ * top_speed_) {
      velocity.normalise();
      velocity *= top_speed_;
    }

    Ogre::Real pitchAngle;
    Ogre::Real pitchAngleSign;

    // Yaws the camera according to the mouse relative movement.
    this->cameraYawNode->yaw(-dx_);

    // Pitches the camera according to the mouse relative movement.
    this->cameraPitchNode->pitch(-dy_);

    // Translates the camera according to the translate vector which is
    // controlled by the keyboard arrows.
    //
    // NOTE: We multiply the mTranslateVector by the cameraPitchNode's
    // orientation quaternion and the cameraYawNode's orientation
    // quaternion to translate the camera accoding to the camera's
    // orientation around the Y-axis and the X-axis.
    this->cameraNode->translate(this->cameraYawNode->getOrientation() *
                                    this->cameraPitchNode->getOrientation() *
                                    this->velocity * timeSinceLastFrame,
                                Ogre::SceneNode::TS_LOCAL);

    // Angle of rotation around the X-axis.
    pitchAngle = (2 * Ogre::Degree(Ogre::Math::ACos(this->cameraPitchNode->getOrientation().w)).valueDegrees());

    // Just to determine the sign of the angle we pick up above, the
    // value itself does not interest us.
    pitchAngleSign = this->cameraPitchNode->getOrientation().x;

    // Limit the pitch between -90 degress and +90 degrees, Quake3-style.
    if (pitchAngle > 90.0f) {
      if (pitchAngleSign > 0)
        // Set orientation to 90 degrees on X-axis.
        this->cameraPitchNode->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f),
                                                               Ogre::Math::Sqrt(0.5f), 0, 0));
      else if (pitchAngleSign < 0)
        // Sets orientation to -90 degrees on X-axis.
        this->cameraPitchNode->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f),
                                                               -Ogre::Math::Sqrt(0.5f), 0, 0));
    }

//    cameraNode->translate(velocity * timeSinceLastFrame);
    float x = cameraNode->getPosition().x;
    float z = cameraNode->getPosition().z;
    cameraNode->setPosition(x, DotSceneLoaderB::GetHeigh(x, z) + heigh_, z);
    dx_ = 0;
    dy_ = 0;

  } else if (camera_style_ == CameraStyle::ORBIT) {
    Ogre::Vector3 move = cameraNode->getPosition() - prevPos;
    prevPos = cameraNode->getPosition();
    cameraNode->translate(move);

  }

  return true;
}
//----------------------------------------------------------------------------------------------------------------------
void CameraMan::MouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) {
  if (!stop_) {
    if (camera_style_ == CameraStyle::FREELOOK) {
      cameraNode->yaw(Ogre::Degree(-dx * 0.25f));
      cameraNode->pitch(Ogre::Degree(-dy * 0.1f));

      if (target_) {
        float dist = GetDistToTarget();
        Ogre::Vector3 delta = cameraNode->getOrientation() * Ogre::Vector3(-dx, dy, 0);

        // the further the camera is, the faster it moves
        delta *= dist / 1000.0f;
        offset += delta;

      }
    } else if (camera_style_ == CameraStyle::FPS) {
      dx_ = Ogre::Degree(dx * 0.25f);
      dy_ = Ogre::Degree(dy * 0.1f);
//      Ogre::Real pitchAngle;
//      Ogre::Real pitchAngleSign;
//
//      // Yaws the camera according to the mouse relative movement.
//      this->cameraYawNode->yaw(Ogre::Degree(-dx * 0.25f));
//
//      // Pitches the camera according to the mouse relative movement.
//      this->cameraPitchNode->pitch(Ogre::Degree(-dy * 0.1f));
//
//      // Translates the camera according to the translate vector which is
//      // controlled by the keyboard arrows.
//      //
//      // NOTE: We multiply the mTranslateVector by the cameraPitchNode's
//      // orientation quaternion and the cameraYawNode's orientation
//      // quaternion to translate the camera accoding to the camera's
//      // orientation around the Y-axis and the X-axis.
//      this->cameraNode->translate(this->cameraYawNode->getOrientation() *
//                                      this->cameraPitchNode->getOrientation() *
//                                      this->mTranslateVector,
//                                  Ogre::SceneNode::TS_LOCAL);
//
//      // Angle of rotation around the X-axis.
//      pitchAngle = (2 * Ogre::Degree(Ogre::Math::ACos(this->cameraPitchNode->getOrientation().w)).valueDegrees());
//
//      // Just to determine the sign of the angle we pick up above, the
//      // value itself does not interest us.
//      pitchAngleSign = this->cameraPitchNode->getOrientation().x;
//
//      // Limit the pitch between -90 degress and +90 degrees, Quake3-style.
//      if (pitchAngle > 90.0f)
//      {
//        if (pitchAngleSign > 0)
//          // Set orientation to 90 degrees on X-axis.
//          this->cameraPitchNode->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f),
//                                                                 Ogre::Math::Sqrt(0.5f), 0, 0));
//        else if (pitchAngleSign < 0)
//          // Sets orientation to -90 degrees on X-axis.
//          this->cameraPitchNode->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f),
//                                                                 -Ogre::Math::Sqrt(0.5f), 0, 0));
//      }

    } else if (camera_style_ == CameraStyle::ORBIT) {
      float dist = GetDistToTarget();

      if (orbiting_)   // yaw around the target, and pitch locally
      {
        cameraNode->setPosition(target_->_getDerivedPosition() + offset);

        cameraNode->yaw(Ogre::Degree(-dx * 0.25f), Ogre::Node::TS_PARENT);
        cameraNode->pitch(Ogre::Degree(-dy * 0.25f));

        cameraNode->translate(Ogre::Vector3(0, 0, dist), Ogre::Node::TS_LOCAL);
        // don't let the camera Go over the top or around the bottom of the target
      } else if (moving_)  // move the camera along the image plane
      {
        Ogre::Vector3 delta = cameraNode->getOrientation() * Ogre::Vector3(-dx, dy, 0);

        // the further the camera is, the faster it moves
        delta *= dist / 1000.0f;
        offset += delta;

      }
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void CameraMan::LeftButtonDown(int x, int y) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void CameraMan::LeftButtonUp(int x, int y) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void CameraMan::RightButtonDown(int x, int y) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void CameraMan::RightButtonUp(int x, int y) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void CameraMan::MiddleButtonDown(int x, int y) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void CameraMan::MiddleButtonUp(int x, int y) {
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

} //namespace Context
