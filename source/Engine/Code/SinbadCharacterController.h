// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include "SDLListener.h"
#include <Ogre.h>

#define SCALE 0.2f
#define CHAR_HEIGHT SCALE * 5.0f  // height of character's center of mass above ground
#define CAM_HEIGHT SCALE * 2.0f   // height of camera above character's center of mass
#define RUN_SPEED SCALE * 30.0f   // character running speed in units per second
#define TURN_SPEED 500.0f         // character turning in degrees per second
#define ANIM_FADE_SPEED 7.5f      // animation crossfade speed in % of full weight per second
#define JUMP_ACCEL SCALE * 30.0f  // character jump acceleration in upward units per squared second
#define GRAVITY SCALE * 90.0f     // gravity in downward units per squared second

namespace Glue {
class SinbadCharacterController : public KeyboardListener, public MouseListener {
 public:
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

  explicit SinbadCharacterController(Ogre::Camera *camera);
  virtual ~SinbadCharacterController();
  void Update(float time);
  void OnKeyDown(SDL_Keycode key) override;
  void OnKeyUp(SDL_Keycode key) override;
  void OnMouseMove(int dx, int dy) override;
  void OnMouseWheel(int x, int y) override;
  void OnMouseLbDown(int x, int y) override;
  void OnMouseRbDown(int x, int y) override;

 protected:
  void SetupBody();
  void SetupAnimations();
  void SetupCamera(Ogre::Camera *camera);
  void UpdateBody(float FrameTime);
  void UpdateAnimations(float FrameTime);
  void FadeAnimations(float FrameTime);
  void UpdateCamera(float FrameTime);
  void UpdateCameraGoal(float deltaYaw, float deltaPitch, float deltaZoom);
  void SetBaseAnimation(AnimID ID, bool reset = false);
  void SetTopAnimation(AnimID ID, bool reset = false);

  Ogre::SceneNode *bodyNode = nullptr;
  Ogre::SceneNode *cameraPivot = nullptr;
  Ogre::SceneNode *cameraGoal = nullptr;
  Ogre::SceneNode *cameraNode = nullptr;
  float pivotPitch;
  Ogre::Entity *body = nullptr;
  Ogre::Entity *sword1 = nullptr;
  Ogre::Entity *sword2 = nullptr;
  Ogre::RibbonTrail *swordTrail = nullptr;
  Ogre::AnimationState *animList[NUM_ANIMS];  // master animation list
  AnimID baseAnimID;                          // current base (full- or lower-body) animation
  AnimID topAnimID;                           // current top (upper-body) animation
  bool fadingIn[NUM_ANIMS];                   // which animations are fading in
  bool fadingOut[NUM_ANIMS];                  // which animations are fading out
  bool swordsDrawn;
  Ogre::Vector3 keyDirection;   // player's local intended direction based on WASD keys
  Ogre::Vector3 goalDirection;  // actual intended direction in world-space
  float verticalVelocity;       // for jumping
  float timer;                  // general timer to see how long animations have been playing
};
}  // namespace Glue
