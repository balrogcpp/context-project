// This source file is part of "glue project"
// Created by Andrew Vasiliev

#include "pcheader.h"
#include "CameraMan.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"


namespace glue {

//----------------------------------------------------------------------------------------------------------------------
CameraMan::CameraMan() {}

//----------------------------------------------------------------------------------------------------------------------
CameraMan::~CameraMan() {}

//----------------------------------------------------------------------------------------------------------------------
void CameraMan::ManualStop() {
  if (style_ == Style::FREELOOK) {
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
void CameraMan::Update(float time) {
  if (style_ == Style::FREELOOK) {
    top_speed_ = move_fast_ ? run_speed_ : const_speed_;

    Ogre::Vector3 accel = Ogre::Vector3::ZERO;
    Ogre::Matrix3 axes = camera_roll_node_->getLocalAxes();

    if (move_forward_) accel -= axes.GetColumn(2);
    if (move_back_) accel += axes.GetColumn(2);
    if (move_right_) accel += axes.GetColumn(0);
    if (move_left_) accel -= axes.GetColumn(0);
    if (move_up_) accel += axes.GetColumn(1);
    if (move_down_) accel -= axes.GetColumn(1);

    if (accel.squaredLength() != 0) {
      accel.normalise();
      velocity_ += accel * top_speed_ * time;
    } else {
      velocity_ -= velocity_ * time;
    }

    if (velocity_.squaredLength() > top_speed_ * top_speed_) {
      velocity_.normalise();
      velocity_ *= top_speed_;
    }

    if (velocity_ != Ogre::Vector3::ZERO) node_->translate(velocity_ * time);
    if (target_) {
      Ogre::Vector3 move = target_->getPosition() - prev_pos_;
      prev_pos_ = target_->getPosition();
      node_->translate(move);
    }
  } else if (style_ == Style::FPS) {
    top_speed_ = move_fast_ ? run_speed_ : const_speed_;

    velocity_ = {0, 0, 0};
    if (move_forward_) velocity_.z = -run_speed_;
    if (move_back_) velocity_.z = run_speed_;
    if (move_right_) velocity_.x = run_speed_;
    if (move_left_) velocity_.x = -run_speed_;
    if (move_up_) velocity_.y = run_speed_;
    if (move_down_) velocity_.y = -run_speed_;

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
    Ogre::Vector3 velocity = camera_yaw_node_->getOrientation() * camera_pitch_node_->getOrientation() * velocity_;

    if (rigid_) {
      if (!velocity_.isZeroLength()) {
        rigid_->setFriction(1.0);
        float speed = rigid_->getLinearVelocity().length();
        if (speed < const_speed_)
          rigid_->applyCentralForce(btVector3(velocity.x, 0, velocity.z).normalize() * 10000.0f);
      } else {
        rigid_->setFriction(10.0);
      }
    }

    float speed = rigid_->getLinearVelocity().length();
    if (speed > (run_speed_ + run_speed_) / 2.0f) {
      animation_time_ += time;
    } else {
      animation_time_ -= time;
    }

    if (animation_time_ > anim_duration_) {
      animation_time_ = anim_duration_;
    } else if (animation_time_ < 0.0f) {
      animation_time_ = 0.0f;
    }

    // Angle of rotation around the X-axis.
    pitchAngle = (2 * Ogre::Degree(Ogre::Math::ACos(camera_pitch_node_->getOrientation().w)).valueDegrees());

    // Just to determine the sign of the angle we pick up above, the
    // value itself does not interest us.
    pitchAngleSign = camera_pitch_node_->getOrientation().x;

    // Limit the pitch between -90 degress and +90 degrees, Quake3-style.
    if (pitchAngle > 90.0f) {
      if (pitchAngleSign > 0)
        // Set orientation to 90 degrees on X-axis.
        camera_pitch_node_->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f), Ogre::Math::Sqrt(0.5f), 0, 0));
      else if (pitchAngleSign < 0)
        // Sets orientation to -90 degrees on X-axis.
        camera_pitch_node_->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f), -Ogre::Math::Sqrt(0.5f), 0, 0));
    }
    dx_ = 0;
    dy_ = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CameraMan::OnMouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) {
  if (!stop_) {
    if (style_ == Style::FREELOOK) {
      node_->yaw(Ogre::Degree(-dx));
      node_->pitch(Ogre::Degree(-dy));
    } else if (style_ == Style::FPS) {
      dx_ = Ogre::Degree(dx);
      dy_ = Ogre::Degree(dy);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CameraMan::OnKeyDown(SDL_Keycode sym) {
  SDL_Scancode code = SDL_GetScancodeFromKey(sym);

  if (!stop_) {
    if (style_ == Style::FREELOOK) {
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
    } else if (style_ == Style::FPS) {
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
void CameraMan::OnKeyUp(SDL_Keycode sym) {
  SDL_Scancode code = SDL_GetScancodeFromKey(sym);

  if (!stop_) {
    if (style_ == Style::FREELOOK) {
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

    } else if (style_ == Style::FPS) {
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

//----------------------------------------------------------------------------------------------------------------------
void CameraMan::SetRigidBody(view_ptr<btRigidBody> rigid_body) { rigid_ = rigid_body; }

//----------------------------------------------------------------------------------------------------------------------
Ogre::SceneNode *CameraMan::GetCameraNode() const {
  if (style_ == Style::FPS) {
    return camera_roll_node_.get();
  } else {
    return node_.get();
  }
}

//----------------------------------------------------------------------------------------------------------------------
Ogre::Camera *CameraMan::GetCamera() const { return camera_.get(); }

//----------------------------------------------------------------------------------------------------------------------
void CameraMan::AttachNode(Ogre::SceneNode *parent, Ogre::SceneNode *proxy) {
  if (style_ == Style::FPS) {
    for (const auto it : parent->getAttachedObjects()) {
      parent->detachObject(it);
      camera_roll_node_->attachObject(it);
    }

    for (const auto it : parent->getChildren()) {
      if (it == camera_yaw_node_.get()) continue;

      parent->removeChild(it);
      camera_roll_node_->removeChild(it);
      camera_roll_node_->addChild(it);
    }
  } else if (style_ == Style::MANUAL) {
    if (!proxy || parent == proxy) return;

    for (const auto it : parent->getAttachedObjects()) {
      if (dynamic_cast<Ogre::Camera *>(it)) continue;

      parent->detachObject(it);
      proxy->attachObject(it);
    }

    for (const auto it : parent->getChildren()) {
      parent->removeChild(it);
      proxy->removeChild(it);
      proxy->addChild(it);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CameraMan::AttachCamera(Ogre::SceneNode *parent, Ogre::Camera *camera, Ogre::SceneNode *proxy) {
  node_ = parent;
  camera_ = camera;

  if (style_ == Style::FPS) {
    node_->setOrientation(Ogre::Quaternion(90.0, 1.0, 0.0, 1.0));

    // SetUp the camera's yaw node as a child of camera's top node.
    camera_yaw_node_ = parent->createChildSceneNode("CameraYaw");

    // SetUp the camera's pitch node as a child of camera's yaw node.
    camera_pitch_node_ = camera_yaw_node_->createChildSceneNode("CameraPitch");

    // SetUp the camera's roll node as a child of camera's pitch node
    // and attach the camera to it.
    camera_roll_node_ = camera_pitch_node_->createChildSceneNode("CameraRoll");
    camera_roll_node_->attachObject(camera_.get());
  } else {
    node_->attachObject(camera_.get());
  }

  if (style_ == Style::FREELOOK) {
    node_->setFixedYawAxis(true);  // also fix axis with lookAt calls
    node_->setAutoTracking(false);
  } else if (style_ == Style::MANUAL) {
    ManualStop();
    node_->setAutoTracking(false);
  } else if (style_ == Style::FPS) {
    node_->setFixedYawAxis(true, Ogre::Vector3::UNIT_Z);  // also fix axis with lookAt calls
    node_->setAutoTracking(false);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CameraMan::UnregCamera() {
  camera_roll_node_->detachAllObjects();
  camera_pitch_node_->detachAllObjects();
  camera_yaw_node_->detachAllObjects();
  node_->detachAllObjects();
  node_ = nullptr;
  camera_yaw_node_ = nullptr;
  camera_pitch_node_ = nullptr;
  camera_roll_node_ = nullptr;
}

//----------------------------------------------------------------------------------------------------------------------
void CameraMan::SetStyle(Style style) { style_ = style; }

//----------------------------------------------------------------------------------------------------------------------
CameraMan::Style CameraMan::GetStyle() const noexcept { return style_; }

}  // namespace glue
