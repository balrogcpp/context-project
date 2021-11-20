// This source file is part of "glue project"
// Created by Andrew Vasiliev

#pragma once
#include "InputHandler.h"
#include "Object.h"
#include <OgreVector.h>
extern "C" {
#include <SDL2/SDL_keycode.h>
}

namespace Ogre {
class SceneNode;
class Entity;
class Camera;
class RibbonTrail;
class AnimationState;
}  // namespace Ogre

#define SCALE 0.2f
#define CHAR_HEIGHT SCALE * 5.0f  // height of character's center of mass above ground
#define CAM_HEIGHT SCALE * 2.0f   // height of camera above character's center of mass
//#define RUN_SPEED SCALE*17.0f           // character running speed in units
// per second
#define RUN_SPEED SCALE * 30.0f   // character running speed in units per second
#define TURN_SPEED 500.0f         // character turning in degrees per second
#define ANIM_FADE_SPEED 7.5f      // animation crossfade speed in % of full weight per second
#define JUMP_ACCEL SCALE * 30.0f  // character jump acceleration in upward units per squared second
#define GRAVITY SCALE * 90.0f     // gravity in downward units per squared second

namespace glue {

class SinbadCharacterController : public glue::Object, public glue::MutedInputObserver {
 protected:
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
  Ogre::SceneNode *GetBodyNode() const;
  explicit SinbadCharacterController(Ogre::Camera *cam);
  void Update(float deltaTime) override;
  void OnKeyDown(SDL_Keycode key) override;
  void OnKeyUp(SDL_Keycode key) override;
  void OnMouseMove(int dx, int dy) override;
  void OnMouseWheel(int x, int y) override;
  void OnMouseLbDown(int x, int y) override;
  void OnMouseRbDown(int x, int y) override;

 protected:
  void setupBody(Ogre::SceneManager *sceneMgr);
  void setupAnimations();
  void setupCamera(Ogre::Camera *cam);
  void updateBody(float deltaTime);
  void updateAnimations(float deltaTime);
  void fadeAnimations(float deltaTime);
  void updateCamera(float deltaTime);
  void updateCameraGoal(float deltaYaw, float deltaPitch, float deltaZoom);
  void setBaseAnimation(AnimID id, bool reset = false);
  void setTopAnimation(AnimID id, bool reset = false);

  Ogre::SceneNode *mBodyNode;
  Ogre::SceneNode *mCameraPivot;
  Ogre::SceneNode *mCameraGoal;
  Ogre::SceneNode *mCameraNode;
  float mPivotPitch;
  Ogre::Entity *mBodyEnt;
  Ogre::Entity *mSword1;
  Ogre::Entity *mSword2;
  Ogre::RibbonTrail *mSwordTrail;
  Ogre::AnimationState *mAnims[NUM_ANIMS];  // master animation list
  AnimID mBaseAnimID;                       // current base (full- or lower-body) animation
  AnimID mTopAnimID;                        // current top (upper-body) animation
  bool mFadingIn[NUM_ANIMS];                // which animations are fading in
  bool mFadingOut[NUM_ANIMS];               // which animations are fading out
  bool mSwordsDrawn;
  Ogre::Vector3 mKeyDirection;   // player's local intended direction based on WASD keys
  Ogre::Vector3 mGoalDirection;  // actual intended direction in world-space
  float mVerticalVelocity;       // for jumping
  float mTimer;                  // general timer to see how long animations have been playing
};

}  // namespace glue
