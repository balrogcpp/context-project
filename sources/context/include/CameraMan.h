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

#include "IO.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"

namespace Ogre {
class SceneNode;
}

namespace Context {
enum CameraStyle   // enumerator values for different styles of camera movement
{
  FREELOOK,
  ORBIT,
  FPS,
  MANUAL
};

class CameraMan : public io::InputListener, public Ogre::FrameListener {
 public:
  CameraMan() {
    io::InputSequencer::GetSingleton().RegisterListener(this);
    Ogre::Root::getSingleton().addFrameListener(this);
  }
//----------------------------------------------------------------------------------------------------------------------
  ~CameraMan() {
    Ogre::Root::getSingleton().removeFrameListener(this);
  }

 private:
//----------------------------------------------------------------------------------------------------------------------
  void ManualStop() {
    if (style_ == CameraStyle::FREELOOK) {
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
  bool frameRenderingQueued(const Ogre::FrameEvent &event) override {
    float timeSinceLastFrame = event.timeSinceLastFrame;

    if (style_ == CameraStyle::FREELOOK) {
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
        node_->translate(velocity_ * timeSinceLastFrame);
      }
      if (target_) {
        Ogre::Vector3 move = target_->getPosition() - prev_pos_;
        prev_pos_ = target_->getPosition();
        node_->translate(move);
      }

    } else if (style_ == CameraStyle::FPS) {
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
      camera_yaw_node_->yaw(-dx_);

      // Pitches the camera according to the mouse relative movement.
      camera_pitch_node_->pitch(-dy_);

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
          rigid_body_->setFriction(0.0);
        } else {
          rigid_body_->setFriction(10.0);
        }
      }

      float speed = rigid_body_->getLinearVelocity().length();

      if (speed > (run_speed_ + run_speed_) / 2.0f) {
        animation_time_ += event.timeSinceLastFrame;
      }
      else {
        animation_time_ -= event.timeSinceLastFrame;
      }

      if (animation_time_ > anim_duration_) {
        animation_time_ = anim_duration_;
      } else if (animation_time_ < 0.0f) {
        animation_time_ = 0.0f;
      }

//    ogre_camera_->setFOVy(Ogre::Radian(1.0 + 0.25 * (animation_time_ / anim_duration_)));

      // Angle of rotation around the X-axis.
      pitchAngle = (2 * Ogre::Degree(Ogre::Math::ACos(camera_pitch_node_->getOrientation().w)).valueDegrees());

      // Just to determine the sign of the angle we pick up above, the
      // value itself does not interest us.
      pitchAngleSign = camera_pitch_node_->getOrientation().x;

      // Limit the pitch between -90 degress and +90 degrees, Quake3-style.
      if (pitchAngle > 90.0f) {
        if (pitchAngleSign > 0)
          // Set orientation to 90 degrees on X-axis.
          camera_pitch_node_->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f),
                                                              Ogre::Math::Sqrt(0.5f), 0, 0));
        else if (pitchAngleSign < 0)
          // Sets orientation to -90 degrees on X-axis.
          camera_pitch_node_->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f),
                                                              -Ogre::Math::Sqrt(0.5f), 0, 0));
      }

      dx_ = 0;
      dy_ = 0;

    } else if (style_ == CameraStyle::ORBIT) {
      Ogre::Vector3 move = node_->getPosition() - prev_pos_;
      prev_pos_ = node_->getPosition();
      node_->translate(move);

    }

    return true;
  }
//----------------------------------------------------------------------------------------------------------------------
  void Move(int x, int y, int dx, int dy, bool left, bool right, bool middle) override {
    if (!stop_) {
      if (style_ == CameraStyle::FREELOOK) {
        node_->yaw(Ogre::Degree(-dx));
        node_->pitch(Ogre::Degree(-dy));

        if (target_) {
          float dist = GetDistToTarget();
          Ogre::Vector3 delta = node_->getOrientation() * Ogre::Vector3(-dx, dy, 0);

          // the further the camera is, the faster it moves
          delta *= dist / 1000.0f;
          offset_ += delta;
        }
      } else if (style_ == CameraStyle::FPS) {
        dx_ = Ogre::Degree(dx);
        dy_ = Ogre::Degree(dy);
      } else if (style_ == CameraStyle::ORBIT) {
        float dist = GetDistToTarget();

        if (orbiting_)   // yaw around the target, and pitch locally
        {
          node_->setPosition(target_->_getDerivedPosition() + offset_);

          node_->yaw(Ogre::Degree(-dx * 0.25f), Ogre::Node::TS_PARENT);
          node_->pitch(Ogre::Degree(-dy * 0.25f));

          node_->translate(Ogre::Vector3(0, 0, dist), Ogre::Node::TS_LOCAL);
          // don't let the camera Go over the top or around the bottom of the target
        } else if (moving_)  // move the camera along the image plane
        {
          Ogre::Vector3 delta = node_->getOrientation() * Ogre::Vector3(-dx, dy, 0);

          // the further the camera is, the faster it moves
          delta *= dist / 1000.0f;
          offset_ += delta;

        }
      }
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  void KeyDown(SDL_Keycode sym) override {
    SDL_Scancode code = SDL_GetScancodeFromKey(sym);

    if (!stop_) {
      if (style_ == CameraStyle::FREELOOK) {
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
      } else if (style_ == CameraStyle::FPS) {
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
  void KeyUp(SDL_Keycode sym) override {
    SDL_Scancode code = SDL_GetScancodeFromKey(sym);

    if (!stop_) {
      if (style_ == CameraStyle::FREELOOK) {
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

      } else if (style_ == CameraStyle::FPS) {
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

 private:
  Ogre::SceneNode *node_ = nullptr;
  Ogre::SceneNode *camera_yaw_node_ = nullptr;
  Ogre::SceneNode *camera_pitch_node_ = nullptr;
  Ogre::SceneNode *camera_roll_node_ = nullptr;
  btRigidBody *rigid_body_ = nullptr;
  Ogre::Degree dx_, dy_;
  Ogre::Camera *camera_ = nullptr;
  CameraStyle style_ = CameraStyle::FPS;
  Ogre::SceneNode *target_ = nullptr;
  bool orbiting_ = false;
  bool moving_ = false;
  float top_speed_ = 10.0f;
  float run_speed_ = 20.0f;
  float animation_time_ = 0.0f;
  float anim_duration_ = .5f;//ms
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
  Ogre::Vector3 offset_ = Ogre::Vector3::ZERO;
  Ogre::Vector3 velocity_ = Ogre::Vector3::ZERO;
  Ogre::Vector3 prev_pos_ = Ogre::Vector3::ZERO;

 public:
  void SetRigidBody(btRigidBody *rigid_body) {
    rigid_body_ = rigid_body;
  }

  btRigidBody *GetRigidBody() const {
    return rigid_body_;
  }

  Ogre::SceneNode *GetCameraNode() const {
    if (style_ == CameraStyle::FPS) {
      return camera_roll_node_;
    } else {
      return node_;
    }
  }

  Ogre::Camera *GetCamera() const {
    return camera_;
  }

  void RegCamera(Ogre::SceneNode *ogre_camera_node, Ogre::Camera *camera) {
    node_ = ogre_camera_node;

    camera_ = camera;
    // Create the camera's yaw node as a child of camera's top node.
    camera_yaw_node_ = node_->createChildSceneNode();

    // Create the camera's pitch node as a child of camera's yaw node.
    camera_pitch_node_ = camera_yaw_node_->createChildSceneNode();

    // Create the camera's roll node as a child of camera's pitch node
    // and attach the camera to it.
    camera_roll_node_ = camera_pitch_node_->createChildSceneNode();
    camera_roll_node_->attachObject(camera_);
    if (style_ == CameraStyle::FPS) {
      ogre_camera_node->setOrientation(Ogre::Quaternion(90.0, 1.0, 0.0, 1.0));
    }
  }

  void UnregCamera() {
    camera_roll_node_->detachAllObjects();
    camera_pitch_node_->detachAllObjects();
    camera_yaw_node_->detachAllObjects();
    node_->detachAllObjects();
    node_ = nullptr;
    camera_yaw_node_ = nullptr;
    camera_pitch_node_ = nullptr;
    camera_roll_node_ = nullptr;
  }

  void SetStyle(CameraStyle style) {
    if (style_ != CameraStyle::ORBIT && style == CameraStyle::ORBIT) {
      node_->setFixedYawAxis(true, Ogre::Vector3::UNIT_Z); // also fix axis with lookAt calls

      // try to replicate the camera configuration
      float dist = GetDistToTarget();
      const Ogre::Quaternion &q = node_->getOrientation();
      SetYawPitchDist(q.getYaw(), q.getPitch(), dist == 0 ? 400 : dist); // enforce some distance
      node_->setAutoTracking(true, target_);
      moving_ = true;
    } else if (style_ != CameraStyle::FREELOOK && style == CameraStyle::FREELOOK) {
      node_->setFixedYawAxis(true); // also fix axis with lookAt calls
      node_->setAutoTracking(false);
    } else if (style_ != CameraStyle::MANUAL && style == CameraStyle::MANUAL) {
      ManualStop();
      node_->setAutoTracking(false);
    } else if (style_ != CameraStyle::FPS && style == CameraStyle::FPS) {
      node_->setFixedYawAxis(true, Ogre::Vector3::UNIT_Z); // also fix axis with lookAt calls
      node_->setAutoTracking(false);
    }

    style_ = style;
  }

  CameraStyle GetStyle() const {
    return style_;
  }

  float GetDistToTarget() {
    if (target_) {
      Ogre::Vector3 offset = node_->getPosition() - target_->_getDerivedPosition() - offset;
      return offset.length();
    } else {
      Ogre::Vector3 offset = node_->getPosition() - offset;
      return offset.length();
    }
  }

  void SetYawPitchDist(Ogre::Radian yaw, Ogre::Radian pitch, float dist) {
    if (target_) {
      node_->setPosition(target_->_getDerivedPosition());
      node_->setOrientation(target_->_getDerivedOrientation());
    }
    node_->yaw(yaw);
    node_->pitch(-pitch);
    node_->translate(Ogre::Vector3(0, dist, 0), Ogre::Node::TS_LOCAL);
  }
};
}
