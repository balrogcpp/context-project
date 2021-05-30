//MIT License
//
//Copyright (c) 2021 Andrew Vasiliev
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#include "pcheader.h"
#include "SinbadCharacterController.h"
#include "MeshUtils.h"
#include "ComponentLocator.h"

using namespace std;
using namespace xio;

Ogre::SceneNode *SinbadCharacterController::GetBodyNode() const {
  return mBodyNode;
}

//----------------------------------------------------------------------------------------------------------------------
SinbadCharacterController::SinbadCharacterController(Ogre::Camera *cam)
	: mBaseAnimID(ANIM_NONE), mTopAnimID(ANIM_NONE) {
  setupBody(cam->getSceneManager());
  setupCamera(cam);
  setupAnimations();
}

//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::Update(Ogre::Real deltaTime) {
  updateBody(deltaTime);
  updateAnimations(deltaTime);
  updateCamera(deltaTime);
}

//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::OnKeyDown(SDL_Keycode key) {
  if (key=='q' && (mTopAnimID==ANIM_IDLE_TOP || mTopAnimID==ANIM_RUN_TOP)) {
// take swords out (or put them back, since it's the same animation but reversed)
	setTopAnimation(ANIM_DRAW_SWORDS, true);
	mTimer = 0;
  } else if (key=='e' && !mSwordsDrawn) {
	if (mTopAnimID==ANIM_IDLE_TOP || mTopAnimID==ANIM_RUN_TOP) {
// start dancing
	  setBaseAnimation(ANIM_DANCE, true);
	  setTopAnimation(ANIM_NONE);
// disable hand animation because the dance controls hands
	  mAnims[ANIM_HANDS_RELAXED]->setEnabled(false);
	} else if (mBaseAnimID==ANIM_DANCE) {
// stop dancing
	  setBaseAnimation(ANIM_IDLE_BASE);
	  setTopAnimation(ANIM_IDLE_TOP);
// re-enable hand animation
	  mAnims[ANIM_HANDS_RELAXED]->setEnabled(true);
	}
  }

// keep track of the player's intended direction
  else if (key=='w') mKeyDirection.z = -1;
  else if (key=='a') mKeyDirection.x = -1;
  else if (key=='s') mKeyDirection.z = 1;
  else if (key=='d') mKeyDirection.x = 1;

  else if (key==SDLK_SPACE && (mTopAnimID==ANIM_IDLE_TOP || mTopAnimID==ANIM_RUN_TOP)) {
// jump if on ground
	setBaseAnimation(ANIM_JUMP_START, true);
	setTopAnimation(ANIM_NONE);
	mTimer = 0;
  }

  if (!mKeyDirection.isZeroLength() && mBaseAnimID==ANIM_IDLE_BASE) {
// start running if not already moving and the player wants to move
	setBaseAnimation(ANIM_RUN_BASE, true);
	if (mTopAnimID==ANIM_IDLE_TOP) setTopAnimation(ANIM_RUN_TOP, true);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::OnKeyUp(SDL_Keycode key) {
// keep track of the player's intended direction
  if (key=='w' && mKeyDirection.z==-1) mKeyDirection.z = 0;
  else if (key=='a' && mKeyDirection.x==-1) mKeyDirection.x = 0;
  else if (key=='s' && mKeyDirection.z==1) mKeyDirection.z = 0;
  else if (key=='d' && mKeyDirection.x==1) mKeyDirection.x = 0;

  if (mKeyDirection.isZeroLength() && mBaseAnimID==ANIM_RUN_BASE) {
// stop running if already moving and the player doesn't want to move
	setBaseAnimation(ANIM_IDLE_BASE);
	if (mTopAnimID==ANIM_RUN_TOP) setTopAnimation(ANIM_IDLE_TOP);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::OnMouseMove(int dx, int dy) {
// update camera goal based on mouse movement
  updateCameraGoal(-0.05f*dx, -0.05f*dy, 0);
}

void SinbadCharacterController::OnMouseWheel(int x, int y) {
// update camera goal based on mouse movement
  updateCameraGoal(0, 0, -0.05f*y);
}

//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::OnMouseLbDown(int x, int y) {
  if (mSwordsDrawn && (mTopAnimID==ANIM_IDLE_TOP || mTopAnimID==ANIM_RUN_TOP)) {
// if swords are out, and character's not doing something weird, then SLICE!
	setTopAnimation(ANIM_SLICE_VERTICAL, true);
	mTimer = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::OnMouseRbDown(int x, int y) {
  if (mSwordsDrawn && (mTopAnimID==ANIM_IDLE_TOP || mTopAnimID==ANIM_RUN_TOP)) {
// if swords are out, and character's not doing something weird, then SLICE!
	setTopAnimation(ANIM_SLICE_HORIZONTAL, true);
	mTimer = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::setupBody(Ogre::SceneManager *sceneMgr) {
  // create main model
  mBodyNode = sceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3::UNIT_Y*CHAR_HEIGHT);
  mBodyEnt = sceneMgr->createEntity("SinbadBody", "Sinbad.mesh");
  mBodyNode->attachObject(mBodyEnt);
  mBodyNode->scale(Ogre::Vector3{SCALE});

  // create swords and attach to sheath
  mSword1 = sceneMgr->createEntity("SinbadSword1", "Sword.mesh");
  mSword2 = sceneMgr->createEntity("SinbadSword2", "Sword.mesh");
  mBodyEnt->attachObjectToBone("Sheath.L", mSword1);
  mBodyEnt->attachObjectToBone("Sheath.R", mSword2);

  UpdateMeshMaterial("Sinbad.mesh");
  UpdateMeshMaterial("Sword.mesh");

  // create a couple of ribbon trails for the swords, just for fun
  Ogre::NameValuePairList params;
  params["numberOfChains"] = "2";
  params["maxElements"] = "80";
  mSwordTrail = (Ogre::RibbonTrail *)sceneMgr->createMovableObject("RibbonTrail", &params);
  mSwordTrail->setMaterialName("Examples/LightRibbonTrail");
  mSwordTrail->setTrailLength(SCALE*20);
  mSwordTrail->setVisible(false);
  sceneMgr->getRootSceneNode()->attachObject(mSwordTrail);

  for (int i = 0; i < 2; i++) {
	mSwordTrail->setInitialColour(i, 1, 0.8, 0);
	mSwordTrail->setColourChange(i, 0.75, 1.25, 1.25, 1.25);
	mSwordTrail->setWidthChange(i, SCALE*1);
	mSwordTrail->setInitialWidth(i, SCALE*0.5);
  }

  mKeyDirection = Ogre::Vector3::ZERO;
  mVerticalVelocity = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::setupAnimations() {
  // this is very important due to the nature of the exported animations
  mBodyEnt->getSkeleton()->setBlendMode(Ogre::ANIMBLEND_CUMULATIVE);

  Ogre::String animNames[NUM_ANIMS] =
	  {"IdleBase", "IdleTop", "RunBase", "RunTop", "HandsClosed", "HandsRelaxed", "DrawSwords",
	   "SliceVertical", "SliceHorizontal", "Dance", "JumpStart", "JumpLoop", "JumpEnd"};

  // populate our animation list
  for (int i = 0; i < NUM_ANIMS; i++) {
	mAnims[i] = mBodyEnt->getAnimationState(animNames[i]);
	mAnims[i]->setLoop(true);
	mFadingIn[i] = false;
	mFadingOut[i] = false;
  }

  // start off in the idle state (top and bottom together)
  setBaseAnimation(ANIM_IDLE_BASE);
  setTopAnimation(ANIM_IDLE_TOP);

  // relax the hands since we're not holding anything
  mAnims[ANIM_HANDS_RELAXED]->setEnabled(true);

  mSwordsDrawn = false;
}

//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::setupCamera(Ogre::Camera *cam) {
  // create a pivot at roughly the character's shoulder
  mCameraPivot = cam->getSceneManager()->getRootSceneNode()->createChildSceneNode();
  // this is where the camera should be soon, and it spins around the pivot
  mCameraGoal = mCameraPivot->createChildSceneNode(Ogre::Vector3(0, SCALE*5, SCALE*15));
  // this is where the camera actually is
  mCameraNode = cam->getParentSceneNode();
  mCameraNode->setPosition(mCameraPivot->getPosition() + mCameraGoal->getPosition());

  mCameraPivot->setFixedYawAxis(true);
  mCameraGoal->setFixedYawAxis(true);
  mCameraNode->setFixedYawAxis(true);

  mPivotPitch = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::updateBody(Ogre::Real deltaTime) {
  mGoalDirection = Ogre::Vector3::ZERO;   // we will calculate this
  float x = mBodyNode->getPosition().x;
  float z = mBodyNode->getPosition().z;
  float y = GetLoader().GetTerrain().GetHeigh(x, z) + CHAR_HEIGHT;
  mBodyNode->setPosition(x, y, z);

  if (mKeyDirection!=Ogre::Vector3::ZERO && mBaseAnimID!=ANIM_DANCE) {
	// calculate actually goal direction in world based on player's key directions
	mGoalDirection += mKeyDirection.z*mCameraNode->getOrientation().zAxis();
	mGoalDirection += mKeyDirection.x*mCameraNode->getOrientation().xAxis();
	mGoalDirection.y = 0;
	mGoalDirection.normalise();

	Ogre::Quaternion toGoal = mBodyNode->getOrientation().zAxis().getRotationTo(mGoalDirection);

	// calculate how much the character has to turn to face goal direction
	Ogre::Real yawToGoal = toGoal.getYaw().valueDegrees();
	// this is how much the character CAN turn this frame
	Ogre::Real yawAtSpeed = yawToGoal/Ogre::Math::Abs(yawToGoal)*deltaTime*TURN_SPEED;
	// reduce "turnability" if we're in midair
	if (mBaseAnimID==ANIM_JUMP_LOOP) yawAtSpeed *= 0.2f;

	// turn as much as we can, but not more than we need to
	if (yawToGoal < 0)
	  yawToGoal = min<Ogre::Real>(0, max<Ogre::Real>(yawToGoal, yawAtSpeed));
	else if (yawToGoal > 0)
	  yawToGoal = max<Ogre::Real>(0, min<Ogre::Real>(yawToGoal, yawAtSpeed));

	mBodyNode->yaw(Ogre::Degree(yawToGoal));

	// move in current body direction (not the goal direction)
	mBodyNode->translate(0, 0, deltaTime*RUN_SPEED*mAnims[mBaseAnimID]->getWeight(), Ogre::Node::TS_LOCAL);
  }

  if (mBaseAnimID==ANIM_JUMP_LOOP) {
	// if we're jumping, add a vertical offset too, and apply gravity
	mBodyNode->translate(0, mVerticalVelocity*deltaTime, 0, Ogre::Node::TS_LOCAL);
	mVerticalVelocity -= GRAVITY*deltaTime;

	Ogre::Vector3 pos = mBodyNode->getPosition();
	if (pos.y <= GetLoader().GetTerrain().GetHeigh(x, z) + CHAR_HEIGHT) {
	  // if we've hit the ground, change to landing state
	  pos.y = GetLoader().GetTerrain().GetHeigh(x, z) + CHAR_HEIGHT;
	  mBodyNode->setPosition(pos);
	  setBaseAnimation(ANIM_JUMP_END, true);
	  mTimer = 0;
	}
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::updateAnimations(Ogre::Real deltaTime) {
  Ogre::Real baseAnimSpeed = 1;
  Ogre::Real topAnimSpeed = 1;

  mTimer += deltaTime;

  if (mTopAnimID==ANIM_DRAW_SWORDS) {
	// flip the draw swords animation if we need to put it back
	topAnimSpeed = mSwordsDrawn ? -1 : 1;

	// half-way through the animation is when the hand grasps the handles...
	if (mTimer >= mAnims[mTopAnimID]->getLength()/2 &&
		mTimer - deltaTime < mAnims[mTopAnimID]->getLength()/2) {
	  // so transfer the swords from the sheaths to the hands
	  mBodyEnt->detachAllObjectsFromBone();
	  mBodyEnt->attachObjectToBone(mSwordsDrawn ? "Sheath.L" : "Handle.L", mSword1);
	  mBodyEnt->attachObjectToBone(mSwordsDrawn ? "Sheath.R" : "Handle.R", mSword2);
	  // change the hand state to grab or let go
	  mAnims[ANIM_HANDS_CLOSED]->setEnabled(!mSwordsDrawn);
	  mAnims[ANIM_HANDS_RELAXED]->setEnabled(mSwordsDrawn);

	  // toggle sword trails
	  if (mSwordsDrawn) {
		mSwordTrail->setVisible(false);
		mSwordTrail->removeNode(mSword1->getParentNode());
		mSwordTrail->removeNode(mSword2->getParentNode());
	  } else {
		mSwordTrail->setVisible(true);
		mSwordTrail->addNode(mSword1->getParentNode());
		mSwordTrail->addNode(mSword2->getParentNode());
	  }
	}

	if (mTimer >= mAnims[mTopAnimID]->getLength()) {
	  // animation is finished, so return to what we were doing before
	  if (mBaseAnimID==ANIM_IDLE_BASE) setTopAnimation(ANIM_IDLE_TOP);
	  else {
		setTopAnimation(ANIM_RUN_TOP);
		mAnims[ANIM_RUN_TOP]->setTimePosition(mAnims[ANIM_RUN_BASE]->getTimePosition());
	  }
	  mSwordsDrawn = !mSwordsDrawn;
	}
  } else if (mTopAnimID==ANIM_SLICE_VERTICAL || mTopAnimID==ANIM_SLICE_HORIZONTAL) {
	if (mTimer >= mAnims[mTopAnimID]->getLength()) {
	  // animation is finished, so return to what we were doing before
	  if (mBaseAnimID==ANIM_IDLE_BASE) setTopAnimation(ANIM_IDLE_TOP);
	  else {
		setTopAnimation(ANIM_RUN_TOP);
		mAnims[ANIM_RUN_TOP]->setTimePosition(mAnims[ANIM_RUN_BASE]->getTimePosition());
	  }
	}

	// don't sway hips from side to side when slicing. that's just embarrassing.
	if (mBaseAnimID==ANIM_IDLE_BASE) baseAnimSpeed = 0;
  } else if (mBaseAnimID==ANIM_JUMP_START) {
	if (mTimer >= mAnims[mBaseAnimID]->getLength()) {
	  // takeoff animation finished, so time to leave the ground!
	  setBaseAnimation(ANIM_JUMP_LOOP, true);
	  // apply a jump acceleration to the character
	  mVerticalVelocity = JUMP_ACCEL;
	}
  } else if (mBaseAnimID==ANIM_JUMP_END) {
	if (mTimer >= mAnims[mBaseAnimID]->getLength()) {
	  // safely landed, so go back to running or idling
	  if (mKeyDirection==Ogre::Vector3::ZERO) {
		setBaseAnimation(ANIM_IDLE_BASE);
		setTopAnimation(ANIM_IDLE_TOP);
	  } else {
		setBaseAnimation(ANIM_RUN_BASE, true);
		setTopAnimation(ANIM_RUN_TOP, true);
	  }
	}
  }

  // increment the current base and top animation times
  if (mBaseAnimID!=ANIM_NONE) mAnims[mBaseAnimID]->addTime(deltaTime*baseAnimSpeed);
  if (mTopAnimID!=ANIM_NONE) mAnims[mTopAnimID]->addTime(deltaTime*topAnimSpeed);

  // apply smooth transitioning between our animations
  fadeAnimations(deltaTime);
}

//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::fadeAnimations(Ogre::Real deltaTime) {
  for (int i = 0; i < NUM_ANIMS; i++) {
	if (mFadingIn[i]) {
	  // slowly fade this animation in until it has full weight
	  Ogre::Real newWeight = mAnims[i]->getWeight() + deltaTime*ANIM_FADE_SPEED;
	  mAnims[i]->setWeight(Ogre::Math::Clamp<Ogre::Real>(newWeight, 0, 1));
	  if (newWeight >= 1) mFadingIn[i] = false;
	} else if (mFadingOut[i]) {
	  // slowly fade this animation out until it has no weight, and then disable it
	  Ogre::Real newWeight = mAnims[i]->getWeight() - deltaTime*ANIM_FADE_SPEED;
	  mAnims[i]->setWeight(Ogre::Math::Clamp<Ogre::Real>(newWeight, 0, 1));
	  if (newWeight <= 0) {
		mAnims[i]->setEnabled(false);
		mFadingOut[i] = false;
	  }
	}
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::updateCamera(Ogre::Real deltaTime) {
  // place the camera pivot roughly at the character's shoulder
  mCameraPivot->setPosition(mBodyNode->getPosition() + Ogre::Vector3::UNIT_Y*CAM_HEIGHT);
  // move the camera smoothly to the goal
  Ogre::Vector3 goalOffset = mCameraGoal->_getDerivedPosition() - mCameraNode->getPosition();
  mCameraNode->translate(goalOffset*deltaTime*9.0f);
  // always look at the pivot
  mCameraNode->lookAt(mCameraPivot->_getDerivedPosition(), Ogre::Node::TS_PARENT);
}

//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::updateCameraGoal(Ogre::Real deltaYaw, Ogre::Real deltaPitch, Ogre::Real deltaZoom) {
  mCameraPivot->yaw(Ogre::Degree(deltaYaw), Ogre::Node::TS_PARENT);

  // bound the pitch
  if (!(mPivotPitch + deltaPitch > 25 && deltaPitch > 0) &&
	  !(mPivotPitch + deltaPitch < -60 && deltaPitch < 0)) {
	mCameraPivot->pitch(Ogre::Degree(deltaPitch), Ogre::Node::TS_LOCAL);
	mPivotPitch += deltaPitch;
  }

  Ogre::Real dist = mCameraGoal->_getDerivedPosition().distance(mCameraPivot->_getDerivedPosition());
  Ogre::Real distChange = deltaZoom*dist;

  // bound the zoom
  if (!(dist + distChange < SCALE*8 && distChange < 0) && !(dist + distChange > SCALE*40 && distChange > 0)) {
	mCameraGoal->translate(0, 0, distChange, Ogre::Node::TS_LOCAL);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::setBaseAnimation(AnimID id, bool reset) {
  if (mBaseAnimID!=ANIM_NONE) {
	// if we have an old animation, fade it out
	mFadingIn[mBaseAnimID] = false;
	mFadingOut[mBaseAnimID] = true;
  }

  mBaseAnimID = id;

  if (id!=ANIM_NONE) {
	// if we have a new animation, enable it and fade it in
	mAnims[id]->setEnabled(true);
	mAnims[id]->setWeight(0);
	mFadingOut[id] = false;
	mFadingIn[id] = true;
	if (reset) mAnims[id]->setTimePosition(0);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::setTopAnimation(AnimID id, bool reset) {
  if (mTopAnimID!=ANIM_NONE) {
	// if we have an old animation, fade it out
	mFadingIn[mTopAnimID] = false;
	mFadingOut[mTopAnimID] = true;
  }

  mTopAnimID = id;

  if (id!=ANIM_NONE) {
	// if we have a new animation, enable it and fade it in
	mAnims[id]->setEnabled(true);
	mAnims[id]->setWeight(0);
	mFadingOut[id] = false;
	mFadingIn[id] = true;
	if (reset) mAnims[id]->setTimePosition(0);
  }
}
