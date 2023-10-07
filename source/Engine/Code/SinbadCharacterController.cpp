// This source file is part of gge Engine. Created by Andrey Vasiliev

#include "pch.h"
#include "SinbadCharacterController.h"
#include "TerrainManager.h"

using namespace std;

namespace gge {
SinbadCharacterController::SinbadCharacterController(Ogre::Camera *camera) : baseAnimID(ANIM_NONE), topAnimID(ANIM_NONE) {
  SetupCamera(camera);
  SetupBody();
  // GetAudio().AddListener(CameraNode);
  // GetAudio().AddSound("walk_grass", "Footsteps-in-grass-fast.ogg", BodyNode, true);
  //  GetAudio().SetSoundVolume("walk_grass", 2.0);
  SetupAnimations();
}

SinbadCharacterController::~SinbadCharacterController() {}

void SinbadCharacterController::Update(Ogre::Real time) {
  UpdateBody(time);
  UpdateAnimations(time);
  UpdateCamera(time);
}

void SinbadCharacterController::OnKeyEvent(SDL_Scancode key, bool pressed) {
  if (pressed) {
    if (key == SDL_SCANCODE_Q && (topAnimID == ANIM_IDLE_TOP || topAnimID == ANIM_RUN_TOP)) {
      // take swords out (or put them back, since it's the same animation but
      // reversed)
      SetTopAnimation(ANIM_DRAW_SWORDS, true);
      timer = 0;
    } else if (key == SDL_SCANCODE_E && !swordsDrawn) {
      if (topAnimID == ANIM_IDLE_TOP || topAnimID == ANIM_RUN_TOP) {
        // start dancing
        SetBaseAnimation(ANIM_DANCE, true);
        SetTopAnimation(ANIM_NONE);
        // disable hand animation because the dance controls hands
        animList[ANIM_HANDS_RELAXED]->setEnabled(false);
      } else if (baseAnimID == ANIM_DANCE) {
        // stop dancing
        SetBaseAnimation(ANIM_IDLE_BASE);
        SetTopAnimation(ANIM_IDLE_TOP);
        // re-enable hand animation
        animList[ANIM_HANDS_RELAXED]->setEnabled(true);
      }
    }

    // keep track of the player's intended direction
    else if (key == SDL_SCANCODE_W)
      keyDirection.z = -1;
    else if (key == SDL_SCANCODE_A)
      keyDirection.x = -1;
    else if (key == SDL_SCANCODE_S)
      keyDirection.z = 1;
    else if (key == SDL_SCANCODE_D)
      keyDirection.x = 1;

    else if (key == SDLK_SPACE && (topAnimID == ANIM_IDLE_TOP || topAnimID == ANIM_RUN_TOP)) {
      // jump if on ground
      SetBaseAnimation(ANIM_JUMP_START, true);
      SetTopAnimation(ANIM_NONE);
      timer = 0;
    }

    if (!keyDirection.isZeroLength() && baseAnimID == ANIM_IDLE_BASE) {
      // start running if not already moving and the player wants to move
      SetBaseAnimation(ANIM_RUN_BASE, true);
      if (topAnimID == ANIM_IDLE_TOP) SetTopAnimation(ANIM_RUN_TOP, true);
    }

  } else {
    // keep track of the player's intended direction
    if (key == SDL_SCANCODE_W && keyDirection.z == -1)
      keyDirection.z = 0;
    else if (key == SDL_SCANCODE_A && keyDirection.x == -1)
      keyDirection.x = 0;
    else if (key == SDL_SCANCODE_S && keyDirection.z == 1)
      keyDirection.z = 0;
    else if (key == SDL_SCANCODE_D && keyDirection.x == 1)
      keyDirection.x = 0;

    if (keyDirection.isZeroLength() && baseAnimID == ANIM_RUN_BASE) {
      // stop running if already moving and the player doesn't want to move
      SetBaseAnimation(ANIM_IDLE_BASE);
      if (topAnimID == ANIM_RUN_TOP) SetTopAnimation(ANIM_IDLE_TOP);
    }
  }
}

void SinbadCharacterController::OnMouseMotion(int dx, int dy) {
  // update camera goal based on mouse movement
  UpdateCameraGoal(-0.05f * dx, -0.05f * dy, 0);
}

void SinbadCharacterController::OnMouseWheel(int x, int y) {
  // update camera goal based on mouse movement
  UpdateCameraGoal(0, 0, -0.05f * y);
}

void SinbadCharacterController::OnMouseButton(int button, bool pressed) {
  if (pressed) {
    if (button == 0) {
      if (swordsDrawn && (topAnimID == ANIM_IDLE_TOP || topAnimID == ANIM_RUN_TOP)) {
        // if swords are out, and character's not doing something weird, then SLICE!
        SetTopAnimation(ANIM_SLICE_VERTICAL, true);
        timer = 0;
      }
    } else if (button == 1) {
      if (swordsDrawn && (topAnimID == ANIM_IDLE_TOP || topAnimID == ANIM_RUN_TOP)) {
        // if swords are out, and character's not doing something weird, then SLICE!
        SetTopAnimation(ANIM_SLICE_HORIZONTAL, true);
        timer = 0;
      }
    }
  }
}

void SinbadCharacterController::SetupBody() {
  auto *sceneMgr = cameraNode->getCreator();

  // create main model
  bodyNode = sceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3::UNIT_Y * CHAR_HEIGHT);
  body = sceneMgr->createEntity("SinbadBody", "Sinbad.mesh");
  bodyNode->attachObject(body);
  bodyNode->scale(Ogre::Vector3(SCALE));

  // create swords and attach to sheath
  sword1 = sceneMgr->createEntity("SinbadSword1", "Sword.mesh");
  sword2 = sceneMgr->createEntity("SinbadSword2", "Sword.mesh");
  body->attachObjectToBone("Sheath.L", sword1);
  body->attachObjectToBone("Sheath.R", sword2);

  // create a couple of ribbon trails for the swords, just for fun
  //  Ogre::NameValuePairList params;
  //  params["numberOfChains"] = "2";
  //  params["maxElements"] = "80";
  //  mSwordTrail = (Ogre::RibbonTrail *)sceneMgr->createMovableObject("RibbonTrail", &params);
  //  mSwordTrail->setMaterialName("Examples/LightRibbonTrail");
  //  GetComponent<SceneManager>().AddMaterial("Examples/LightRibbonTrail");
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

  keyDirection = Ogre::Vector3::ZERO;
  verticalVelocity = 0;
}

void SinbadCharacterController::SetupAnimations() {
  // this is very important due to the nature of the exported animations
  body->getSkeleton()->setBlendMode(Ogre::ANIMBLEND_CUMULATIVE);

  Ogre::String animNames[NUM_ANIMS] = {"IdleBase",      "IdleTop",         "RunBase", "RunTop",    "HandsClosed", "HandsRelaxed", "DrawSwords",
                                       "SliceVertical", "SliceHorizontal", "Dance",   "JumpStart", "JumpLoop",    "JumpEnd"};

  // populate our animation list
  for (int i = 0; i < NUM_ANIMS; i++) {
    animList[i] = body->getAnimationState(animNames[i]);
    animList[i]->setLoop(true);
    fadingIn[i] = false;
    fadingOut[i] = false;
  }

  // start off in the idle state (top and bottom together)
  SetBaseAnimation(ANIM_IDLE_BASE);
  SetTopAnimation(ANIM_IDLE_TOP);

  // relax the hands since we're not holding anything
  animList[ANIM_HANDS_RELAXED]->setEnabled(true);

  swordsDrawn = false;
}

void SinbadCharacterController::SetupCamera(Ogre::Camera *camera) {
  // create a pivot at roughly the character's shoulder
  cameraPivot = camera->getSceneManager()->getRootSceneNode()->createChildSceneNode();
  // this is where the camera should be soon, and it spins around the pivot
  cameraGoal = cameraPivot->createChildSceneNode(Ogre::Vector3(0, SCALE * 5, SCALE * 15));
  // this is where the camera actually is
  cameraNode = camera->getParentSceneNode();
  cameraNode->setPosition(cameraPivot->getPosition() + cameraGoal->getPosition());

  cameraPivot->setFixedYawAxis(true);
  cameraGoal->setFixedYawAxis(true);
  cameraNode->setFixedYawAxis(true);

  pivotPitch = 0;
}

void SinbadCharacterController::UpdateBody(float FrameTime) {
  goalDirection = Ogre::Vector3::ZERO;  // we will calculate this
  float x = bodyNode->getPosition().x;
  float z = bodyNode->getPosition().z;
  //  float y = GetComponent<SceneManager>().GetHeight(x, z) + CHAR_HEIGHT;
  float y = CHAR_HEIGHT;
  bodyNode->setPosition(x, y + GetComponent<TerrainManager>().GetHeight(x, z), z);

  if (keyDirection != Ogre::Vector3::ZERO && baseAnimID != ANIM_DANCE) {
    // calculate actually goal direction in world based on player's key
    // directions
    goalDirection += keyDirection.z * cameraNode->getOrientation().zAxis();
    goalDirection += keyDirection.x * cameraNode->getOrientation().xAxis();
    goalDirection.y = 0;
    goalDirection.normalise();

    Ogre::Quaternion toGoal = bodyNode->getOrientation().zAxis().getRotationTo(goalDirection);

    // calculate how much the character has to turn to face goal direction
    float yawToGoal = toGoal.getYaw().valueDegrees();
    // this is how much the character CAN turn this frame
    float yawAtSpeed = yawToGoal / Ogre::Math::Abs(yawToGoal) * FrameTime * TURN_SPEED;
    // reduce "turnability" if we're in midair
    if (baseAnimID == ANIM_JUMP_LOOP) yawAtSpeed *= 0.2f;

    // turn as much as we can, but not more than we need to
    if (yawToGoal < 0)
      yawToGoal = min<float>(0, max<float>(yawToGoal, yawAtSpeed));
    else if (yawToGoal > 0)
      yawToGoal = max<float>(0, min<float>(yawToGoal, yawAtSpeed));

    bodyNode->yaw(Ogre::Degree(yawToGoal));

    // move in current body direction (not the goal direction)
    bodyNode->translate(0, 0, FrameTime * RUN_SPEED * animList[baseAnimID]->getWeight(), Ogre::Node::TS_LOCAL);
  }

  if (baseAnimID == ANIM_JUMP_LOOP) {
    // if we're jumping, add a vertical offset too, and apply gravity
    bodyNode->translate(0, verticalVelocity * FrameTime, 0, Ogre::Node::TS_LOCAL);
    verticalVelocity -= GRAVITY * FrameTime;

    Ogre::Vector3 pos = bodyNode->getPosition();
    if (pos.y <= 0 + CHAR_HEIGHT) {
      // if we've hit the ground, change to landing state
      pos.y = 0 + CHAR_HEIGHT;
      bodyNode->setPosition(pos);
      SetBaseAnimation(ANIM_JUMP_END, true);
      timer = 0;
    }
    if (pos.y <= CHAR_HEIGHT) {
      // if we've hit the ground, change to landing state
      pos.y = CHAR_HEIGHT;
      bodyNode->setPosition(pos);
      SetBaseAnimation(ANIM_JUMP_END, true);
      timer = 0;
    }
  }
}

void SinbadCharacterController::UpdateAnimations(float FrameTime) {
  float baseAnimSpeed = 1;
  float topAnimSpeed = 1;

  timer += FrameTime;

  if (topAnimID == ANIM_DRAW_SWORDS) {
    // flip the draw swords animation if we need to put it back
    topAnimSpeed = swordsDrawn ? -1 : 1;

    // half-way through the animation is when the hand grasps the handles...
    if (timer >= animList[topAnimID]->getLength() / 2 && timer - FrameTime < animList[topAnimID]->getLength() / 2) {
      // so transfer the swords from the sheaths to the hands
      body->detachAllObjectsFromBone();
      body->attachObjectToBone(swordsDrawn ? "Sheath.L" : "Handle.L", sword1);
      body->attachObjectToBone(swordsDrawn ? "Sheath.R" : "Handle.R", sword2);
      // change the hand state to grab or let go
      animList[ANIM_HANDS_CLOSED]->setEnabled(!swordsDrawn);
      animList[ANIM_HANDS_RELAXED]->setEnabled(swordsDrawn);

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

    if (timer >= animList[topAnimID]->getLength()) {
      // animation is finished, so return to what we were doing before
      if (baseAnimID == ANIM_IDLE_BASE)
        SetTopAnimation(ANIM_IDLE_TOP);
      else {
        SetTopAnimation(ANIM_RUN_TOP);
        animList[ANIM_RUN_TOP]->setTimePosition(animList[ANIM_RUN_BASE]->getTimePosition());
      }
      swordsDrawn = !swordsDrawn;
    }
  } else if (topAnimID == ANIM_SLICE_VERTICAL || topAnimID == ANIM_SLICE_HORIZONTAL) {
    if (timer >= animList[topAnimID]->getLength()) {
      // animation is finished, so return to what we were doing before
      if (baseAnimID == ANIM_IDLE_BASE)
        SetTopAnimation(ANIM_IDLE_TOP);
      else {
        SetTopAnimation(ANIM_RUN_TOP);
        animList[ANIM_RUN_TOP]->setTimePosition(animList[ANIM_RUN_BASE]->getTimePosition());
      }
    }

    // don't sway hips from side to side when slicing. that's just embarrassing.
    if (baseAnimID == ANIM_IDLE_BASE) baseAnimSpeed = 0;
  } else if (baseAnimID == ANIM_JUMP_START) {
    if (timer >= animList[baseAnimID]->getLength()) {
      // takeoff animation finished, so time to leave the ground!
      SetBaseAnimation(ANIM_JUMP_LOOP, true);
      // apply a jump acceleration to the character
      verticalVelocity = JUMP_ACCEL;
    }
  } else if (baseAnimID == ANIM_JUMP_END) {
    if (timer >= animList[baseAnimID]->getLength()) {
      // safely landed, so go back to running or idling
      if (keyDirection == Ogre::Vector3::ZERO) {
        SetBaseAnimation(ANIM_IDLE_BASE);
        SetTopAnimation(ANIM_IDLE_TOP);
      } else {
        SetBaseAnimation(ANIM_RUN_BASE, true);
        SetTopAnimation(ANIM_RUN_TOP, true);
      }
    }
  }

  // increment the current base and top animation times
  if (baseAnimID != ANIM_NONE) animList[baseAnimID]->addTime(FrameTime * baseAnimSpeed);
  if (topAnimID != ANIM_NONE) animList[topAnimID]->addTime(FrameTime * topAnimSpeed);

  // apply smooth transitioning between our animations
  FadeAnimations(FrameTime);
}

void SinbadCharacterController::FadeAnimations(float FrameTime) {
  for (int i = 0; i < NUM_ANIMS; i++) {
    if (fadingIn[i]) {
      // slowly fade this animation in until it has full weight
      float newWeight = animList[i]->getWeight() + FrameTime * ANIM_FADE_SPEED;
      animList[i]->setWeight(Ogre::Math::Clamp<float>(newWeight, 0, 1));
      if (newWeight >= 1) fadingIn[i] = false;
    } else if (fadingOut[i]) {
      // slowly fade this animation out until it has no weight, and then disable
      // it
      float newWeight = animList[i]->getWeight() - FrameTime * ANIM_FADE_SPEED;
      animList[i]->setWeight(Ogre::Math::Clamp<float>(newWeight, 0, 1));
      if (newWeight <= 0) {
        animList[i]->setEnabled(false);
        fadingOut[i] = false;
      }
    }
  }
}

void SinbadCharacterController::UpdateCamera(float FrameTime) {
  // place the camera pivot roughly at the character's shoulder
  cameraPivot->setPosition(bodyNode->getPosition() + Ogre::Vector3::UNIT_Y * CAM_HEIGHT);
  // move the camera smoothly to the goal
  Ogre::Vector3 goalOffset = cameraGoal->_getDerivedPosition() - cameraNode->getPosition();
  cameraNode->translate(goalOffset * FrameTime * 9.0f);
  // always look at the pivot
  cameraNode->lookAt(cameraPivot->_getDerivedPosition(), Ogre::Node::TS_PARENT);
}

void SinbadCharacterController::UpdateCameraGoal(float deltaYaw, float deltaPitch, float deltaZoom) {
  cameraPivot->yaw(Ogre::Degree(deltaYaw), Ogre::Node::TS_PARENT);

  // bound the pitch
  if (!(pivotPitch + deltaPitch > 25 && deltaPitch > 0) && !(pivotPitch + deltaPitch < -60 && deltaPitch < 0)) {
    cameraPivot->pitch(Ogre::Degree(deltaPitch), Ogre::Node::TS_LOCAL);
    pivotPitch += deltaPitch;
  }

  float dist = cameraGoal->_getDerivedPosition().distance(cameraPivot->_getDerivedPosition());
  float distChange = deltaZoom * dist;

  // bound the zoom
  if (!(dist + distChange < SCALE * 8 && distChange < 0) && !(dist + distChange > SCALE * 40 && distChange > 0)) {
    cameraGoal->translate(0, 0, distChange, Ogre::Node::TS_LOCAL);
  }
}

void SinbadCharacterController::SetBaseAnimation(AnimID ID, bool reset) {
  if (baseAnimID != ANIM_NONE) {
    // if we have an old animation, fade it out
    fadingIn[baseAnimID] = false;
    fadingOut[baseAnimID] = true;
  }

  baseAnimID = ID;

  if (ID != ANIM_NONE) {
    // if we have a new animation, enable it and fade it in
    animList[ID]->setEnabled(true);
    animList[ID]->setWeight(0);
    fadingOut[ID] = false;
    fadingIn[ID] = true;
    if (reset) animList[ID]->setTimePosition(0);
  }
}

void SinbadCharacterController::SetTopAnimation(AnimID ID, bool reset) {
  if (topAnimID != ANIM_NONE) {
    // if we have an old animation, fade it out
    fadingIn[topAnimID] = false;
    fadingOut[topAnimID] = true;
  }

  topAnimID = ID;

  if (ID != ANIM_NONE) {
    // if we have a new animation, enable it and fade it in
    animList[ID]->setEnabled(true);
    animList[ID]->setWeight(0);
    fadingOut[ID] = false;
    fadingIn[ID] = true;
    if (reset) animList[ID]->setTimePosition(0);
  }

  // if (ID == ANIM_RUN_TOP)
  //  GetAudio().PlaySound("walk_grass");
  // else
  //  GetAudio().StopSound("walk_grass");
}
}  // namespace gge
