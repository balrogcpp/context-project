// This source file is part of Glue Engine. Created by Andrey Vasiliev

#include "PCHeader.h"
#include "Engine.h"
#include "SinbadCharacterController.h"

using namespace std;

namespace Glue {

Ogre::SceneNode *SinbadCharacterController::GetBodyNode() const { return BodyNode; }

SinbadCharacterController::SinbadCharacterController(Ogre::Camera *cam) : BaseAnimID(ANIM_NONE), TopAnimID(ANIM_NONE) {
  SetupBody();
  SetupCamera(cam);
  //GetAudio().AddSound("walk_grass", "Footsteps-in-grass-fast.ogg", BodyNode, true);
  //GetAudio().SetSoundVolume("walk_grass", 2.0);
  //GetAudio().AddListener(CameraNode);
  SetupAnimations();
}

SinbadCharacterController::~SinbadCharacterController() {}

void SinbadCharacterController::Update(float deltaTime) {
  UpdateBody(deltaTime);
  UpdateAnimations(deltaTime);
  UpdateCamera(deltaTime);
}

void SinbadCharacterController::OnKeyDown(SDL_Keycode key) {
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

void SinbadCharacterController::OnKeyUp(SDL_Keycode key) {
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

void SinbadCharacterController::OnMouseMove(int dx, int dy) {
  // update camera goal based on mouse movement
  UpdateCameraGoal(-0.05f * dx, -0.05f * dy, 0);
}

void SinbadCharacterController::OnMouseWheel(int x, int y) {
  // update camera goal based on mouse movement
  UpdateCameraGoal(0, 0, -0.05f * y);
}

void SinbadCharacterController::OnMouseLbDown(int x, int y) {
  if (SwordsDrawn && (TopAnimID == ANIM_IDLE_TOP || TopAnimID == ANIM_RUN_TOP)) {
    // if swords are out, and character's not doing something weird, then SLICE!
    SetTopAnimation(ANIM_SLICE_VERTICAL, true);
    Timer = 0;
  }
}

void SinbadCharacterController::OnMouseRbDown(int x, int y) {
  if (SwordsDrawn && (TopAnimID == ANIM_IDLE_TOP || TopAnimID == ANIM_RUN_TOP)) {
    // if swords are out, and character's not doing something weird, then SLICE!
    SetTopAnimation(ANIM_SLICE_HORIZONTAL, true);
    Timer = 0;
  }
}

void SinbadCharacterController::SetupBody() {
  auto *sceneMgr = OgreSceneManager();

  // create main model
  BodyNode = sceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3::UNIT_Y * CHAR_HEIGHT);
  BodyEnt = sceneMgr->createEntity("SinbadBody", "Sinbad.mesh");
  GetScene().AddEntity(BodyEnt);
  BodyNode->attachObject(BodyEnt);
  BodyNode->scale(Ogre::Vector3{SCALE});

  // create swords and attach to sheath
  Sword1 = sceneMgr->createEntity("SinbadSword1", "Sword.mesh");
  Sword2 = sceneMgr->createEntity("SinbadSword2", "Sword.mesh");
  GetScene().AddEntity(Sword1);
  GetScene().AddEntity(Sword2);
  BodyEnt->attachObjectToBone("Sheath.L", Sword1);
  BodyEnt->attachObjectToBone("Sheath.R", Sword2);

  // create a couple of ribbon trails for the swords, just for fun
  //  Ogre::NameValuePairList params;
  //  params["numberOfChains"] = "2";
  //  params["maxElements"] = "80";
  //  mSwordTrail = (Ogre::RibbonTrail *)sceneMgr->createMovableObject("RibbonTrail", &params);
  //  mSwordTrail->setMaterialName("Examples/LightRibbonTrail");
  //  GetScene().AddMaterial("Examples/LightRibbonTrail");
  //  mSwordTrail->setTrailLength(SCALE * 20);
  //  mSwordTrail->setVisible(false);
  //  sceneMgr->getRootSceneNode()->attachObject(mSwordTrail);
  //
  //  for (int i = 0; i < 2; i++) {
  //    mSwordTrail->setInitialColour(i, 1, 0.8, 0);
  //    mSwordTrail->setColourChange(i, 0.75, 1.25, 1.25, 1.25);
  //    mSwordTrail->setWidthChange(i, SCALE * 1);
  //    mSwordTrail->setInitialWidth(i, SCALE * 0.5);
  //  }

  KeyDirection = Ogre::Vector3::ZERO;
  VerticalVelocity = 0;
}

void SinbadCharacterController::SetupAnimations() {
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

void SinbadCharacterController::SetupCamera(Ogre::Camera *CameraPtr) {
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

void SinbadCharacterController::UpdateBody(float FrameTime) {
  GoalDirection = Ogre::Vector3::ZERO;  // we will calculate this
  float x = BodyNode->getPosition().x;
  float z = BodyNode->getPosition().z;
  float y = GetScene().GetHeight(x, z) + CHAR_HEIGHT;
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
    if (pos.y <= GetScene().GetHeight(x, z) + CHAR_HEIGHT) {
      // if we've hit the ground, change to landing state
      pos.y = GetScene().GetHeight(x, z) + CHAR_HEIGHT;
      BodyNode->setPosition(pos);
      SetBaseAnimation(ANIM_JUMP_END, true);
      Timer = 0;
    }
  }
}

void SinbadCharacterController::UpdateAnimations(float FrameTime) {
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

      // toggle sword trails
      //      if (mSwordsDrawn) {
      //        mSwordTrail->setVisible(false);
      //        mSwordTrail->removeNode(mSword1->getParentNode());
      //        mSwordTrail->removeNode(mSword2->getParentNode());
      //      } else {
      //        mSwordTrail->setVisible(true);
      //        mSwordTrail->addNode(mSword1->getParentNode());
      //        mSwordTrail->addNode(mSword2->getParentNode());
      //      }
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

void SinbadCharacterController::FadeAnimations(float FrameTime) {
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

void SinbadCharacterController::UpdateCamera(float FrameTime) {
  // place the camera pivot roughly at the character's shoulder
  CameraPivot->setPosition(BodyNode->getPosition() + Ogre::Vector3::UNIT_Y * CAM_HEIGHT);
  // move the camera smoothly to the goal
  Ogre::Vector3 goalOffset = CameraGoal->_getDerivedPosition() - CameraNode->getPosition();
  CameraNode->translate(goalOffset * FrameTime * 9.0f);
  // always look at the pivot
  CameraNode->lookAt(CameraPivot->_getDerivedPosition(), Ogre::Node::TS_PARENT);
}

void SinbadCharacterController::UpdateCameraGoal(float deltaYaw, float deltaPitch, float deltaZoom) {
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

void SinbadCharacterController::SetBaseAnimation(AnimID ID, bool reset) {
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

void SinbadCharacterController::SetTopAnimation(AnimID ID, bool reset) {
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

//  if (ID == ANIM_RUN_TOP)
//    GetAudio().PlaySound("walk_grass");
//  else
//    GetAudio().StopSound("walk_grass");
}

}  // namespace Glue
