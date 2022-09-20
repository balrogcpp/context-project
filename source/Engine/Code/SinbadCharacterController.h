/// created by Andrey Vasiliev

#pragma once
#include "Engine.h"
#include "Observer.h"
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
#define RUN_SPEED SCALE * 30.0f   // character running speed in units per second
#define TURN_SPEED 500.0f         // character turning in degrees per second
#define ANIM_FADE_SPEED 7.5f      // animation crossfade speed in % of full weight per second
#define JUMP_ACCEL SCALE * 30.0f  // character jump acceleration in upward units per squared second
#define GRAVITY SCALE * 90.0f     // gravity in downward units per squared second

namespace Glue {

class SinbadCharacterController : public InputObserver {
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

  SinbadCharacterController(Ogre::Camera *cam) : BaseAnimID(ANIM_NONE), TopAnimID(ANIM_NONE) {
    SetupCamera(cam);
    SetupBody();
    SetupAnimations();
  }

  ~SinbadCharacterController() {}

  void Update(float deltaTime) {
    UpdateBody(deltaTime);
    UpdateAnimations(deltaTime);
    UpdateCamera(deltaTime);
  }

  void OnKeyDown(SDL_Keycode key) {
    if (SDL_GetScancodeFromKey(key) == SDL_SCANCODE_Q && (TopAnimID == ANIM_IDLE_TOP || TopAnimID == ANIM_RUN_TOP)) {
      // take swords out (or put them back, since it's the same animation but
      // reversed)
      SetTopAnimation(ANIM_DRAW_SWORDS, true);
      Timer = 0;
    } else if (SDL_GetScancodeFromKey(key) == SDL_SCANCODE_E && !SwordsDrawn) {
      if (TopAnimID == ANIM_IDLE_TOP || TopAnimID == ANIM_RUN_TOP) {
        // start dancing
        SetBaseAnimation(ANIM_DANCE, true);
        SetTopAnimation(ANIM_NONE);
        // disable hand animation because the dance controls hands
        Anims[ANIM_HANDS_RELAXED]->setEnabled(false);
      } else if (BaseAnimID == ANIM_DANCE) {
        // stop dancing
        SetBaseAnimation(ANIM_IDLE_BASE);
        SetTopAnimation(ANIM_IDLE_TOP);
        // re-enable hand animation
        Anims[ANIM_HANDS_RELAXED]->setEnabled(true);
      }
    }

    // keep track of the player's intended direction
    else if (SDL_GetScancodeFromKey(key) == SDL_SCANCODE_W)
      KeyDirection.z = -1;
    else if (SDL_GetScancodeFromKey(key) == SDL_SCANCODE_A)
      KeyDirection.x = -1;
    else if (SDL_GetScancodeFromKey(key) == SDL_SCANCODE_S)
      KeyDirection.z = 1;
    else if (SDL_GetScancodeFromKey(key) == SDL_SCANCODE_D)
      KeyDirection.x = 1;

    else if (key == SDLK_SPACE && (TopAnimID == ANIM_IDLE_TOP || TopAnimID == ANIM_RUN_TOP)) {
      // jump if on ground
      SetBaseAnimation(ANIM_JUMP_START, true);
      SetTopAnimation(ANIM_NONE);
      Timer = 0;
    }

    if (!KeyDirection.isZeroLength() && BaseAnimID == ANIM_IDLE_BASE) {
      // start running if not already moving and the player wants to move
      SetBaseAnimation(ANIM_RUN_BASE, true);
      if (TopAnimID == ANIM_IDLE_TOP) SetTopAnimation(ANIM_RUN_TOP, true);
    }
  }

  void OnKeyUp(SDL_Keycode key) {
    // keep track of the player's intended direction
    if (SDL_GetScancodeFromKey(key) == SDL_SCANCODE_W && KeyDirection.z == -1)
      KeyDirection.z = 0;
    else if (SDL_GetScancodeFromKey(key) == SDL_SCANCODE_A && KeyDirection.x == -1)
      KeyDirection.x = 0;
    else if (SDL_GetScancodeFromKey(key) == SDL_SCANCODE_S && KeyDirection.z == 1)
      KeyDirection.z = 0;
    else if (SDL_GetScancodeFromKey(key) == SDL_SCANCODE_D && KeyDirection.x == 1)
      KeyDirection.x = 0;

    if (KeyDirection.isZeroLength() && BaseAnimID == ANIM_RUN_BASE) {
      // stop running if already moving and the player doesn't want to move
      SetBaseAnimation(ANIM_IDLE_BASE);
      if (TopAnimID == ANIM_RUN_TOP) SetTopAnimation(ANIM_IDLE_TOP);
    }
  }

  void OnMouseMove(int dx, int dy) {
    // update camera goal based on mouse movement
    UpdateCameraGoal(-0.05f * dx, -0.05f * dy, 0);
  }

  void OnMouseWheel(int x, int y) {
    // update camera goal based on mouse movement
    UpdateCameraGoal(0, 0, -0.05f * y);
  }

  void OnMouseLbDown(int x, int y) {
    if (SwordsDrawn && (TopAnimID == ANIM_IDLE_TOP || TopAnimID == ANIM_RUN_TOP)) {
      // if swords are out, and character's not doing something weird, then SLICE!
      SetTopAnimation(ANIM_SLICE_VERTICAL, true);
      Timer = 0;
    }
  }

  void OnMouseRbDown(int x, int y) {
    if (SwordsDrawn && (TopAnimID == ANIM_IDLE_TOP || TopAnimID == ANIM_RUN_TOP)) {
      // if swords are out, and character's not doing something weird, then SLICE!
      SetTopAnimation(ANIM_SLICE_HORIZONTAL, true);
      Timer = 0;
    }
  }

  void SetupBody() {
    auto *sceneMgr = CameraNode->getCreator();

    // create main model
    BodyNode = sceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3::UNIT_Y * CHAR_HEIGHT);
    BodyEnt = sceneMgr->createEntity("SinbadBody", "Sinbad.mesh");
    GetEngine().AddEntity(BodyEnt);
    BodyNode->attachObject(BodyEnt);
    BodyNode->scale(Ogre::Vector3(SCALE));

    // create swords and attach to sheath
    Sword1 = sceneMgr->createEntity("SinbadSword1", "Sword.mesh");
    Sword2 = sceneMgr->createEntity("SinbadSword2", "Sword.mesh");
    GetEngine().AddEntity(Sword1);
    GetEngine().AddEntity(Sword2);
    BodyEnt->attachObjectToBone("Sheath.L", Sword1);
    BodyEnt->attachObjectToBone("Sheath.R", Sword2);

    KeyDirection = Ogre::Vector3::ZERO;
    VerticalVelocity = 0;
  }

  void SetupAnimations() {
    // this is very important due to the nature of the exported animations
    BodyEnt->getSkeleton()->setBlendMode(Ogre::ANIMBLEND_CUMULATIVE);

    Ogre::String animNames[NUM_ANIMS] = {"IdleBase",      "IdleTop",         "RunBase", "RunTop",    "HandsClosed", "HandsRelaxed", "DrawSwords",
                                         "SliceVertical", "SliceHorizontal", "Dance",   "JumpStart", "JumpLoop",    "JumpEnd"};

    // populate our animation list
    for (int i = 0; i < NUM_ANIMS; i++) {
      Anims[i] = BodyEnt->getAnimationState(animNames[i]);
      Anims[i]->setLoop(true);
      FadingIn[i] = false;
      FadingOut[i] = false;
    }

    // start off in the idle state (top and bottom together)
    SetBaseAnimation(ANIM_IDLE_BASE);
    SetTopAnimation(ANIM_IDLE_TOP);

    // relax the hands since we're not holding anything
    Anims[ANIM_HANDS_RELAXED]->setEnabled(true);

    SwordsDrawn = false;
  }

  void SetupCamera(Ogre::Camera *CameraPtr) {
    // create a pivot at roughly the character's shoulder
    CameraPivot = CameraPtr->getSceneManager()->getRootSceneNode()->createChildSceneNode();
    // this is where the camera should be soon, and it spins around the pivot
    CameraGoal = CameraPivot->createChildSceneNode(Ogre::Vector3(0, SCALE * 5, SCALE * 15));
    // this is where the camera actually is
    CameraNode = CameraPtr->getParentSceneNode();
    CameraNode->setPosition(CameraPivot->getPosition() + CameraGoal->getPosition());

    CameraPivot->setFixedYawAxis(true);
    CameraGoal->setFixedYawAxis(true);
    CameraNode->setFixedYawAxis(true);

    PivotPitch = 0;
  }

  void UpdateBody(float FrameTime) {
    using std::min, std::max;
    GoalDirection = Ogre::Vector3::ZERO;  // we will calculate this
    float x = BodyNode->getPosition().x;
    float z = BodyNode->getPosition().z;
    float y = GetEngine().GetHeight(x, z) + CHAR_HEIGHT;
    BodyNode->setPosition(x, y, z);

    if (KeyDirection != Ogre::Vector3::ZERO && BaseAnimID != ANIM_DANCE) {
      // calculate actually goal direction in world based on player's key
      // directions
      GoalDirection += KeyDirection.z * CameraNode->getOrientation().zAxis();
      GoalDirection += KeyDirection.x * CameraNode->getOrientation().xAxis();
      GoalDirection.y = 0;
      GoalDirection.normalise();

      Ogre::Quaternion toGoal = BodyNode->getOrientation().zAxis().getRotationTo(GoalDirection);

      // calculate how much the character has to turn to face goal direction
      float yawToGoal = toGoal.getYaw().valueDegrees();
      // this is how much the character CAN turn this frame
      float yawAtSpeed = yawToGoal / Ogre::Math::Abs(yawToGoal) * FrameTime * TURN_SPEED;
      // reduce "turnability" if we're in midair
      if (BaseAnimID == ANIM_JUMP_LOOP) yawAtSpeed *= 0.2f;

      // turn as much as we can, but not more than we need to
      if (yawToGoal < 0)
        yawToGoal = min<float>(0, max<float>(yawToGoal, yawAtSpeed));
      else if (yawToGoal > 0)
        yawToGoal = max<float>(0, min<float>(yawToGoal, yawAtSpeed));

      BodyNode->yaw(Ogre::Degree(yawToGoal));

      // move in current body direction (not the goal direction)
      BodyNode->translate(0, 0, FrameTime * RUN_SPEED * Anims[BaseAnimID]->getWeight(), Ogre::Node::TS_LOCAL);
    }

    if (BaseAnimID == ANIM_JUMP_LOOP) {
      // if we're jumping, add a vertical offset too, and apply gravity
      BodyNode->translate(0, VerticalVelocity * FrameTime, 0, Ogre::Node::TS_LOCAL);
      VerticalVelocity -= GRAVITY * FrameTime;

      Ogre::Vector3 pos = BodyNode->getPosition();
      if (pos.y <= 0 + CHAR_HEIGHT) {
        // if we've hit the ground, change to landing state
        pos.y = 0 + CHAR_HEIGHT;
        BodyNode->setPosition(pos);
        SetBaseAnimation(ANIM_JUMP_END, true);
        Timer = 0;
      }
      if (pos.y <= GetEngine().GetHeight(x, z) + CHAR_HEIGHT) {
        // if we've hit the ground, change to landing state
        pos.y = GetEngine().GetHeight(x, z) + CHAR_HEIGHT;
        BodyNode->setPosition(pos);
        SetBaseAnimation(ANIM_JUMP_END, true);
        Timer = 0;
      }
    }
  }

  void UpdateAnimations(float FrameTime) {
    float baseAnimSpeed = 1;
    float topAnimSpeed = 1;

    Timer += FrameTime;

    if (TopAnimID == ANIM_DRAW_SWORDS) {
      // flip the draw swords animation if we need to put it back
      topAnimSpeed = SwordsDrawn ? -1 : 1;

      // half-way through the animation is when the hand grasps the handles...
      if (Timer >= Anims[TopAnimID]->getLength() / 2 && Timer - FrameTime < Anims[TopAnimID]->getLength() / 2) {
        // so transfer the swords from the sheaths to the hands
        BodyEnt->detachAllObjectsFromBone();
        BodyEnt->attachObjectToBone(SwordsDrawn ? "Sheath.L" : "Handle.L", Sword1);
        BodyEnt->attachObjectToBone(SwordsDrawn ? "Sheath.R" : "Handle.R", Sword2);
        // change the hand state to grab or let go
        Anims[ANIM_HANDS_CLOSED]->setEnabled(!SwordsDrawn);
        Anims[ANIM_HANDS_RELAXED]->setEnabled(SwordsDrawn);
      }

      if (Timer >= Anims[TopAnimID]->getLength()) {
        // animation is finished, so return to what we were doing before
        if (BaseAnimID == ANIM_IDLE_BASE)
          SetTopAnimation(ANIM_IDLE_TOP);
        else {
          SetTopAnimation(ANIM_RUN_TOP);
          Anims[ANIM_RUN_TOP]->setTimePosition(Anims[ANIM_RUN_BASE]->getTimePosition());
        }
        SwordsDrawn = !SwordsDrawn;
      }
    } else if (TopAnimID == ANIM_SLICE_VERTICAL || TopAnimID == ANIM_SLICE_HORIZONTAL) {
      if (Timer >= Anims[TopAnimID]->getLength()) {
        // animation is finished, so return to what we were doing before
        if (BaseAnimID == ANIM_IDLE_BASE)
          SetTopAnimation(ANIM_IDLE_TOP);
        else {
          SetTopAnimation(ANIM_RUN_TOP);
          Anims[ANIM_RUN_TOP]->setTimePosition(Anims[ANIM_RUN_BASE]->getTimePosition());
        }
      }

      // don't sway hips from side to side when slicing. that's just embarrassing.
      if (BaseAnimID == ANIM_IDLE_BASE) baseAnimSpeed = 0;
    } else if (BaseAnimID == ANIM_JUMP_START) {
      if (Timer >= Anims[BaseAnimID]->getLength()) {
        // takeoff animation finished, so time to leave the ground!
        SetBaseAnimation(ANIM_JUMP_LOOP, true);
        // apply a jump acceleration to the character
        VerticalVelocity = JUMP_ACCEL;
      }
    } else if (BaseAnimID == ANIM_JUMP_END) {
      if (Timer >= Anims[BaseAnimID]->getLength()) {
        // safely landed, so go back to running or idling
        if (KeyDirection == Ogre::Vector3::ZERO) {
          SetBaseAnimation(ANIM_IDLE_BASE);
          SetTopAnimation(ANIM_IDLE_TOP);
        } else {
          SetBaseAnimation(ANIM_RUN_BASE, true);
          SetTopAnimation(ANIM_RUN_TOP, true);
        }
      }
    }

    // increment the current base and top animation times
    if (BaseAnimID != ANIM_NONE) Anims[BaseAnimID]->addTime(FrameTime * baseAnimSpeed);
    if (TopAnimID != ANIM_NONE) Anims[TopAnimID]->addTime(FrameTime * topAnimSpeed);

    // apply smooth transitioning between our animations
    FadeAnimations(FrameTime);
  }

  void FadeAnimations(float FrameTime) {
    for (int i = 0; i < NUM_ANIMS; i++) {
      if (FadingIn[i]) {
        // slowly fade this animation in until it has full weight
        float newWeight = Anims[i]->getWeight() + FrameTime * ANIM_FADE_SPEED;
        Anims[i]->setWeight(Ogre::Math::Clamp<float>(newWeight, 0, 1));
        if (newWeight >= 1) FadingIn[i] = false;
      } else if (FadingOut[i]) {
        // slowly fade this animation out until it has no weight, and then disable
        // it
        float newWeight = Anims[i]->getWeight() - FrameTime * ANIM_FADE_SPEED;
        Anims[i]->setWeight(Ogre::Math::Clamp<float>(newWeight, 0, 1));
        if (newWeight <= 0) {
          Anims[i]->setEnabled(false);
          FadingOut[i] = false;
        }
      }
    }
  }

  void UpdateCamera(float FrameTime) {
    // place the camera pivot roughly at the character's shoulder
    CameraPivot->setPosition(BodyNode->getPosition() + Ogre::Vector3::UNIT_Y * CAM_HEIGHT);
    // move the camera smoothly to the goal
    Ogre::Vector3 goalOffset = CameraGoal->_getDerivedPosition() - CameraNode->getPosition();
    CameraNode->translate(goalOffset * FrameTime * 9.0f);
    // always look at the pivot
    CameraNode->lookAt(CameraPivot->_getDerivedPosition(), Ogre::Node::TS_PARENT);
  }

  void UpdateCameraGoal(float deltaYaw, float deltaPitch, float deltaZoom) {
    CameraPivot->yaw(Ogre::Degree(deltaYaw), Ogre::Node::TS_PARENT);

    // bound the pitch
    if (!(PivotPitch + deltaPitch > 25 && deltaPitch > 0) && !(PivotPitch + deltaPitch < -60 && deltaPitch < 0)) {
      CameraPivot->pitch(Ogre::Degree(deltaPitch), Ogre::Node::TS_LOCAL);
      PivotPitch += deltaPitch;
    }

    float dist = CameraGoal->_getDerivedPosition().distance(CameraPivot->_getDerivedPosition());
    float distChange = deltaZoom * dist;

    // bound the zoom
    if (!(dist + distChange < SCALE * 8 && distChange < 0) && !(dist + distChange > SCALE * 40 && distChange > 0)) {
      CameraGoal->translate(0, 0, distChange, Ogre::Node::TS_LOCAL);
    }
  }

  void SetBaseAnimation(AnimID ID, bool reset = false) {
    if (BaseAnimID != ANIM_NONE) {
      // if we have an old animation, fade it out
      FadingIn[BaseAnimID] = false;
      FadingOut[BaseAnimID] = true;
    }

    BaseAnimID = ID;

    if (ID != ANIM_NONE) {
      // if we have a new animation, enable it and fade it in
      Anims[ID]->setEnabled(true);
      Anims[ID]->setWeight(0);
      FadingOut[ID] = false;
      FadingIn[ID] = true;
      if (reset) Anims[ID]->setTimePosition(0);
    }
  }

  void SetTopAnimation(AnimID ID, bool reset = false) {
    if (TopAnimID != ANIM_NONE) {
      // if we have an old animation, fade it out
      FadingIn[TopAnimID] = false;
      FadingOut[TopAnimID] = true;
    }

    TopAnimID = ID;

    if (ID != ANIM_NONE) {
      // if we have a new animation, enable it and fade it in
      Anims[ID]->setEnabled(true);
      Anims[ID]->setWeight(0);
      FadingOut[ID] = false;
      FadingIn[ID] = true;
      if (reset) Anims[ID]->setTimePosition(0);
    }
  }

  Ogre::SceneNode *BodyNode = nullptr;
  Ogre::SceneNode *CameraPivot = nullptr;
  Ogre::SceneNode *CameraGoal = nullptr;
  Ogre::SceneNode *CameraNode = nullptr;
  float PivotPitch;
  Ogre::Entity *BodyEnt = nullptr;
  Ogre::Entity *Sword1 = nullptr;
  Ogre::Entity *Sword2 = nullptr;
  Ogre::RibbonTrail *SwordTrail = nullptr;
  Ogre::AnimationState *Anims[NUM_ANIMS];  // master animation list
  AnimID BaseAnimID;                       // current base (full- or lower-body) animation
  AnimID TopAnimID;                        // current top (upper-body) animation
  bool FadingIn[NUM_ANIMS];                // which animations are fading in
  bool FadingOut[NUM_ANIMS];               // which animations are fading out
  bool SwordsDrawn;
  Ogre::Vector3 KeyDirection;   // player's local intended direction based on WASD keys
  Ogre::Vector3 GoalDirection;  // actual intended direction in world-space
  float VerticalVelocity;       // for jumping
  float Timer;                  // general timer to see how long animations have been playing
};

}  // namespace Glue
