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

#include "InputManager.hpp"
#include "InputListener.hpp"
#include <Ogre.h>

namespace Context {
class SinbadCharacterController : public InputListener, public Ogre::FrameListener {
 private:
  // all the animations our character has, and a null ID
  // some of these affect separate body parts and will be blended together
  enum AnimID {
    ANIM_IDLE_BASE,
    ANIM_IDLE_TOP,
    ANIM_RUN_BASE,
    ANIM_RUN_TOP,
    ANIM_HANDS_CLOSED,
    ANIM_HANDS_RELAXED,
    ANIM_DRAW_SWORDS,
    ANIM_SLICE_VERTICAL,
    ANIM_SLICE_HORIZONTAL,
    ANIM_DANCE,
    ANIM_JUMP_START,
    ANIM_JUMP_LOOP,
    ANIM_JUMP_END,
    NUM_ANIMS,
    ANIM_NONE = NUM_ANIMS
  };

 public:

  SinbadCharacterController(Ogre::Camera *cam);

  virtual ~SinbadCharacterController();

  void AddTime(float deltaTime);

  bool frameRenderingQueued(const Ogre::FrameEvent &evt);

  void KeyDown(SDL_Keycode key) override;

  void KeyUp(SDL_Keycode key) override;

  void MouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) override;

  void MouseWheel(int x, int y) override;

  void LeftButtonDown(int x, int y) override;

  void RightButtonDown(int x, int y) override;

  void SetupBody(Ogre::SceneManager *sceneMgr);

  void SetupAnimations();

  void SetupCamera(Ogre::Camera *cam);

  void UpdateBody(float deltaTime);

  void UpdateAnimations(float deltaTime);

  void FadeAnimations(float deltaTime);

  void UpdateCamera(float deltaTime);

  void UpdateCameraGoal(float deltaYaw, float deltaPitch, float deltaZoom);

  void SetBaseAnimation(AnimID id, bool reset = false);

  void SetTopAnimation(AnimID id, bool reset = false);

  Ogre::SceneNode *body_node_ = nullptr;
  Ogre::SceneNode *camera_pivot_ = nullptr;
  Ogre::SceneNode *camera_goal_ = nullptr;
  Ogre::SceneNode *camera_node_ = nullptr;
  float pivot_pitch_ = 0.0f;
  float pivot_yaw_ = 0.0f;
  float delta_yaw_ = 0.0f;
  float delta_pitch_ = 0.0f;
  float delta_zoom_ = 0.0f;
  Ogre::Entity *body_ent_ = nullptr;
  Ogre::Entity *sword1_ = nullptr;
  Ogre::Entity *sword2_ = nullptr;
  Ogre::RibbonTrail *sword_trail_ = nullptr;
  Ogre::AnimationState *anims_[NUM_ANIMS];    // master animation list
  AnimID base_anim_;                   // current base (full- or lower-body) animation
  AnimID top_anim_;                    // current top (upper-body) animation
  bool anim_fading_in_[NUM_ANIMS];            // which animations are fading in
  bool anim_fading_out_[NUM_ANIMS];           // which animations are fading out
  bool sword_drawn_ = false;
  Ogre::Vector3 key_dirction_ = {0, 0, 0};      // player's local intended direction based on WASD keys
  Ogre::Vector3 goal_direction_ = {0, 0, 0};     // actual intended direction in world-space
  float vertical_velocity_ = 0.0f;     // for jumping
  float timer_ = 0.0f;                // general timer to see how long animations have been playing
};
}
