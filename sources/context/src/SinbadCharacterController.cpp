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

#include "SinbadCharacterController.hpp"

#include "DotSceneLoaderB.hpp"

using namespace Ogre;

const static float scale = 0.2f;
const static float CHAR_HEIGHT = scale*2.0f;         // height of character's center of mass above ground
const static float CAM_HEIGHT = scale*2.0f;           // height of camera above character's center of mass
const static float RUN_SPEED = scale*17;           // character running speed in units per second
const static float TURN_SPEED = 500.0f;      // character turning in degrees per second
const static float ANIM_FADE_SPEED = 7.5f;   // animation crossfade speed in % of full weight per second
const static float JUMP_ACCEL = scale*30.0f;       // character jump acceleration in upward units per squared second
const static float GRAVITY = scale * 90.0f;          // gravity in downward units per squared second

namespace Context {
//----------------------------------------------------------------------------------------------------------------------
SinbadCharacterController::SinbadCharacterController(Camera *cam)
    : base_anim_(ANIM_NONE), top_anim_(ANIM_NONE) {
  SetupBody(cam->getSceneManager());
  SetupCamera(cam);
  SetupAnimations();
  InputManager::GetSingleton().RegisterListener(this);
  ContextManager::GetSingleton().GetOgreRootPtr()->addFrameListener(this);
}
//----------------------------------------------------------------------------------------------------------------------
SinbadCharacterController::~SinbadCharacterController() {
  InputManager::GetSingleton().UnregisterListener(this);
  ContextManager::GetSingleton().GetOgreRootPtr()->removeFrameListener(this);
}
//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::AddTime(float deltaTime) {
  UpdateBody(deltaTime);
  UpdateAnimations(deltaTime);
  UpdateCamera(deltaTime);
}
//----------------------------------------------------------------------------------------------------------------------
bool SinbadCharacterController::frameRenderingQueued(const Ogre::FrameEvent &evt) {
  AddTime(evt.timeSinceLastFrame);

  return true;
}
//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::key_down(SDL_Keycode key) {
  SDL_Scancode code = SDL_GetScancodeFromKey(key);
//        Keycode key = evt.keysym.sym;
  if (code == SDL_SCANCODE_Q && (top_anim_ == ANIM_IDLE_TOP || top_anim_ == ANIM_RUN_TOP)) {
    // take swords out (or put them back, since it's the same animation but reversed)
    SetTopAnimation(ANIM_DRAW_SWORDS, true);
    timer_ = 0;
  } else if (code == SDL_SCANCODE_E && !sword_drawn_) {
    if (top_anim_ == ANIM_IDLE_TOP || top_anim_ == ANIM_RUN_TOP) {
      // start dancing
      SetBaseAnimation(ANIM_DANCE, true);
      SetTopAnimation(ANIM_NONE);
      // disable hand animation because the dance controls hands
      anims_[ANIM_HANDS_RELAXED]->setEnabled(false);
    } else if (base_anim_ == ANIM_DANCE) {
      // stop dancing
      SetBaseAnimation(ANIM_IDLE_BASE);
      SetTopAnimation(ANIM_IDLE_TOP);
      // re-enable hand animation
      anims_[ANIM_HANDS_RELAXED]->setEnabled(true);
    }
  }

    // keep track of the player's intended direction
  else if (code == SDL_SCANCODE_W) key_direction_.z = -1;
  else if (code == SDL_SCANCODE_A) key_direction_.x = -1;
  else if (code == SDL_SCANCODE_S) key_direction_.z = 1;
  else if (code == SDL_SCANCODE_D) key_direction_.x = 1;

  else if (code == SDL_SCANCODE_SPACE && (top_anim_ == ANIM_IDLE_TOP || top_anim_ == ANIM_RUN_TOP)) {
    // jump if on ground
    SetBaseAnimation(ANIM_JUMP_START, true);
    SetTopAnimation(ANIM_NONE);
    timer_ = 0;
  }

  if (!key_direction_.isZeroLength() && base_anim_ == ANIM_IDLE_BASE) {
    // start running if not already moving and the player wants to move
    SetBaseAnimation(ANIM_RUN_BASE, true);
    if (top_anim_ == ANIM_IDLE_TOP) SetTopAnimation(ANIM_RUN_TOP, true);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::key_up(SDL_Keycode key) {
  SDL_Scancode code = SDL_GetScancodeFromKey(key);
  // keep track of the player's intended direction
  if (code == SDL_SCANCODE_W && key_direction_.z == -1) key_direction_.z = 0;
  else if (code == SDL_SCANCODE_A && key_direction_.x == -1) key_direction_.x = 0;
  else if (code == SDL_SCANCODE_S && key_direction_.z == 1) key_direction_.z = 0;
  else if (code == SDL_SCANCODE_D && key_direction_.x == 1) key_direction_.x = 0;

  if (key_direction_.isZeroLength() && base_anim_ == ANIM_RUN_BASE) {
    // stop running if already moving and the player doesn't want to move
    SetBaseAnimation(ANIM_IDLE_BASE);
    if (top_anim_ == ANIM_RUN_TOP) SetTopAnimation(ANIM_IDLE_TOP);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::move(int x, int y, int dx, int dy, bool left, bool right, bool middle) {
  // Update camera goal based on mouse movement
  UpdateCameraGoal(-0.05f * dx, -0.05f * dy, 0);
}
//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::wheel(int x, int y) {
//        // Update camera goal based on mouse movement
  UpdateCameraGoal(0, 0, -0.05f * y);
}
//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::lb_down(int x, int y) {
  if (sword_drawn_ && (top_anim_ == ANIM_IDLE_TOP || top_anim_ == ANIM_RUN_TOP)) {
    // if swords are out, and character's not doing something weird, then SLICE!
    SetTopAnimation(ANIM_SLICE_VERTICAL, true);
    timer_ = 0;
  }
}
//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::rb_down(int x, int y) {
  if (sword_drawn_ && (top_anim_ == ANIM_IDLE_TOP || top_anim_ == ANIM_RUN_TOP)) {
    // if swords are out, and character's not doing something weird, then SLICE!
    SetTopAnimation(ANIM_SLICE_HORIZONTAL, true);
    timer_ = 0;
  }
}
//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::SetupBody(SceneManager *sceneMgr) {
  // Create main model
  body_node_ = sceneMgr->getRootSceneNode()->createChildSceneNode(Vector3::UNIT_Y * CHAR_HEIGHT);
  body_ent_ = sceneMgr->createEntity("SinbadBody", "Sinbad.mesh");
  body_node_->attachObject(body_ent_);
  body_node_->scale(Ogre::Vector3(scale));

  // Create swords and attach to sheath
  LogManager::getSingleton().logMessage("Creating swords");
  sword1_ = sceneMgr->createEntity("SinbadSword1", "Sword.mesh");
  sword2_ = sceneMgr->createEntity("SinbadSword2", "Sword.mesh");
  body_ent_->attachObjectToBone("Sheath.L", sword1_);
  body_ent_->attachObjectToBone("Sheath.R", sword2_);

  LogManager::getSingleton().logMessage("Creating the chains");
  // Create a couple of ribbon trails for the swords, just for fun
  NameValuePairList params;
  params["numberOfChains"] = "2";
  params["maxElements"] = "80";
  sword_trail_ = (RibbonTrail *) sceneMgr->createMovableObject("RibbonTrail", &params);
  sword_trail_->setMaterialName("Examples/LightRibbonTrail");
  sword_trail_->setTrailLength(20);
  sword_trail_->setVisible(false);
  sceneMgr->getRootSceneNode()->attachObject(sword_trail_);

  for (int i = 0; i < 2; i++) {
    sword_trail_->setInitialColour(i, 1, 0.8, 0);
    sword_trail_->setColourChange(i, 0.75, 1.25, 1.25, 1.25);
    sword_trail_->setWidthChange(i, 1);
    sword_trail_->setInitialWidth(i, 0.5);
  }

  key_direction_ = Vector3::ZERO;
  vertical_velocity_ = 0;
}
//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::SetupAnimations() {
  // this is very important due to the nature of the exported animations
  body_ent_->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);

  String animNames[NUM_ANIMS] =
      {"IdleBase", "IdleTop", "RunBase", "RunTop", "HandsClosed", "HandsRelaxed", "DrawSwords",
       "SliceVertical", "SliceHorizontal", "Dance", "JumpStart", "JumpLoop", "JumpEnd"};

  // populate our animation list
  for (int i = 0; i < NUM_ANIMS; i++) {
    anims_[i] = body_ent_->getAnimationState(animNames[i]);
    anims_[i]->setLoop(true);
    anim_fading_in_[i] = false;
    anim_fading_out_[i] = false;
  }

  // start off in the idle state (top and bottom together)
  SetBaseAnimation(ANIM_IDLE_BASE);
  SetTopAnimation(ANIM_IDLE_TOP);

  // relax the hands since we're not holding anything
  anims_[ANIM_HANDS_RELAXED]->setEnabled(true);

  sword_drawn_ = false;
}
//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::SetupCamera(Camera *cam) {
  // this is where the camera actually is
  camera_node_ = cam->getParentSceneNode();
  // Create a pivot at roughly the character's shoulder
  camera_pivot_ = cam->getSceneManager()->getRootSceneNode()->createChildSceneNode();
  // this is where the camera should be soon, and it spins around the pivot
//  camera_goal_ = camera_pivot_->createChildSceneNode(Vector3(0, 0,  15));
  camera_goal_ = camera_pivot_->createChildSceneNode(Vector3(0, 0,  2.0));
  camera_node_->setPosition(camera_pivot_->getPosition() + camera_goal_->getPosition());

  camera_pivot_->setFixedYawAxis(true);
  camera_goal_->setFixedYawAxis(true);
  camera_node_->setFixedYawAxis(true);

  pivot_pitch_ = 0;
}
//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::UpdateBody(float deltaTime) {
  goal_direction_ = Vector3::ZERO;   // we will calculate this
  float x = body_node_->getPosition().x;
  float z = body_node_->getPosition().z;
  float y = DotSceneLoaderB::GetSingleton().GetHeigh(x, z) + CHAR_HEIGHT;
  body_node_->setPosition(x, y, z);

  if (key_direction_ != Vector3::ZERO && base_anim_ != ANIM_DANCE) {
    // calculate actually goal direction in world based on player's key directions
    goal_direction_ += key_direction_.z * camera_node_->getOrientation().zAxis();
    goal_direction_ += key_direction_.x * camera_node_->getOrientation().xAxis();
    goal_direction_.y = 0;
    goal_direction_.normalise();

    Quaternion toGoal = body_node_->getOrientation().zAxis().getRotationTo(goal_direction_);

    // calculate how much the character has to turn to face goal direction
    float yawToGoal = toGoal.getYaw().valueDegrees();
    // this is how much the character CAN turn this frame
    float yawAtSpeed = yawToGoal / Math::Abs(yawToGoal) * deltaTime * TURN_SPEED;
    // reduce "turnability" if we're in midair
    if (base_anim_ == ANIM_JUMP_LOOP) yawAtSpeed *= 0.2f;

    // turn as much as we can, but not more than we need to
    if (yawToGoal < 0)
      yawToGoal = std::min<float>(0, std::max<float>(yawToGoal, yawAtSpeed));
    else if (yawToGoal > 0)
      yawToGoal = std::max<float>(0, std::min<float>(yawToGoal, yawAtSpeed));

    body_node_->yaw(Degree(yawToGoal));

    // move in current body direction (not the goal direction)
    body_node_->translate(0, 0, deltaTime * RUN_SPEED * anims_[base_anim_]->getWeight(), Node::TS_LOCAL);
  }

  if (base_anim_ == ANIM_JUMP_LOOP) {
    // if we're jumping, add a vertical offset too, and apply gravity
    body_node_->translate(0, vertical_velocity_ * deltaTime, 0, Node::TS_LOCAL);
    vertical_velocity_ -= GRAVITY * deltaTime;

    Vector3 pos = body_node_->getPosition();
    if (pos.y <= y) {
      // if we've hit the ground, change to landing state
      pos.y = y;
      body_node_->setPosition(pos);
      SetBaseAnimation(ANIM_JUMP_END, true);
      timer_ = 0;
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::UpdateAnimations(float deltaTime) {
  float baseAnimSpeed = 1;
  float topAnimSpeed = 1;

  timer_ += deltaTime;

  if (top_anim_ == ANIM_DRAW_SWORDS) {
    // flip the draw swords animation if we need to put it back
    topAnimSpeed = sword_drawn_ ? -1 : 1;

    // half-way through the animation is when the hand grasps the handles...
    if (timer_ >= anims_[top_anim_]->getLength() / 2 &&
        timer_ - deltaTime < anims_[top_anim_]->getLength() / 2) {
      // so transfer the swords from the sheaths to the hands
      body_ent_->detachAllObjectsFromBone();
      body_ent_->attachObjectToBone(sword_drawn_ ? "Sheath.L" : "Handle.L", sword1_);
      body_ent_->attachObjectToBone(sword_drawn_ ? "Sheath.R" : "Handle.R", sword2_);
      // change the hand state to grab or let Go
      anims_[ANIM_HANDS_CLOSED]->setEnabled(!sword_drawn_);
      anims_[ANIM_HANDS_RELAXED]->setEnabled(sword_drawn_);

      // toggle sword trails
      if (sword_drawn_) {
        sword_trail_->setVisible(false);
        sword_trail_->removeNode(sword1_->getParentNode());
        sword_trail_->removeNode(sword2_->getParentNode());
      } else {
        sword_trail_->setVisible(true);
        sword_trail_->addNode(sword1_->getParentNode());
        sword_trail_->addNode(sword2_->getParentNode());
      }
    }

    if (timer_ >= anims_[top_anim_]->getLength()) {
      // animation is finished, so return to what we were doing before
      if (base_anim_ == ANIM_IDLE_BASE) SetTopAnimation(ANIM_IDLE_TOP);
      else {
        SetTopAnimation(ANIM_RUN_TOP);
        anims_[ANIM_RUN_TOP]->setTimePosition(anims_[ANIM_RUN_BASE]->getTimePosition());
      }
      sword_drawn_ = !sword_drawn_;
    }
  } else if (top_anim_ == ANIM_SLICE_VERTICAL || top_anim_ == ANIM_SLICE_HORIZONTAL) {
    if (timer_ >= anims_[top_anim_]->getLength()) {
      // animation is finished, so return to what we were doing before
      if (base_anim_ == ANIM_IDLE_BASE) SetTopAnimation(ANIM_IDLE_TOP);
      else {
        SetTopAnimation(ANIM_RUN_TOP);
        anims_[ANIM_RUN_TOP]->setTimePosition(anims_[ANIM_RUN_BASE]->getTimePosition());
      }
    }

    // don't sway hips from side to side when slicing. that's just embarrassing.
    if (base_anim_ == ANIM_IDLE_BASE) baseAnimSpeed = 0;
  } else if (base_anim_ == ANIM_JUMP_START) {
    if (timer_ >= anims_[base_anim_]->getLength()) {
      // takeoff animation finished, so time to leave the ground!
      SetBaseAnimation(ANIM_JUMP_LOOP, true);
      // apply a jump acceleration to the character
      vertical_velocity_ = JUMP_ACCEL;
    }
  } else if (base_anim_ == ANIM_JUMP_END) {
    if (timer_ >= anims_[base_anim_]->getLength()) {
      // safely landed, so Go back to running or idling
      if (key_direction_ == Vector3::ZERO) {
        SetBaseAnimation(ANIM_IDLE_BASE);
        SetTopAnimation(ANIM_IDLE_TOP);
      } else {
        SetBaseAnimation(ANIM_RUN_BASE, true);
        SetTopAnimation(ANIM_RUN_TOP, true);
      }
    }
  }

  // increment the current base and top animation times
  if (base_anim_ != ANIM_NONE) anims_[base_anim_]->addTime(deltaTime * baseAnimSpeed);
  if (top_anim_ != ANIM_NONE) anims_[top_anim_]->addTime(deltaTime * topAnimSpeed);

  // apply smooth transitioning between our animations
  FadeAnimations(deltaTime);
}
//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::FadeAnimations(float deltaTime) {
  for (int i = 0; i < NUM_ANIMS; i++) {
    if (anim_fading_in_[i]) {
      // slowly fade this animation in until it has full weight
      float newWeight = anims_[i]->getWeight() + deltaTime * ANIM_FADE_SPEED;
      anims_[i]->setWeight(Math::Clamp<float>(newWeight, 0, 1));
      if (newWeight >= 1) anim_fading_in_[i] = false;
    } else if (anim_fading_out_[i]) {
      // slowly fade this animation out until it has no weight, and then disable it
      float newWeight = anims_[i]->getWeight() - deltaTime * ANIM_FADE_SPEED;
      anims_[i]->setWeight(Math::Clamp<float>(newWeight, 0, 1));
      if (newWeight <= 0) {
        anims_[i]->setEnabled(false);
        anim_fading_out_[i] = false;
      }
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::UpdateCamera(float deltaTime) {
  // place the camera pivot roughly at the character's shoulder
  camera_pivot_->setPosition(body_node_->getPosition() + Vector3::UNIT_Y * CAM_HEIGHT);

  // move the camera smoothly to the goal
  Vector3 goalOffset = camera_goal_->_getDerivedPosition() - camera_node_->getPosition();
  camera_node_->translate(goalOffset * deltaTime);

  // always look at the pivot
//  camera_node_->lookAt(camera_pivot_->_getDerivedPosition(), Node::TS_PARENT);

//  camera_node_->setPosition(body_node_->getPosition());
  camera_node_->lookAt(body_node_->_getDerivedPosition(), Node::TS_WORLD);
}
//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::UpdateCameraGoal(float deltaYaw, float deltaPitch, float deltaZoom) {
  camera_pivot_->yaw(Degree(deltaYaw), Node::TS_PARENT);

  // bound the pitch
  if (!(pivot_pitch_ + deltaPitch > 0 && deltaPitch > 0) &&
      !(pivot_pitch_ + deltaPitch < -30 && deltaPitch < 0)) {
    camera_pivot_->pitch(Degree(deltaPitch), Node::TS_LOCAL);
    pivot_pitch_ += deltaPitch;
  }

  float dist = camera_goal_->_getDerivedPosition().distance(camera_pivot_->_getDerivedPosition());
  float distChange = deltaZoom * dist;

  // bound the zoom
  if (!(dist + distChange < scale*8 && distChange < 0) &&
      !(dist + distChange > scale*25 && distChange > 0)) {
    camera_goal_->translate(0, 0, distChange, Node::TS_LOCAL);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::SetBaseAnimation(AnimID id, bool reset) {
  if (base_anim_ != ANIM_NONE) {
    // if we have an old animation, fade it out
    anim_fading_in_[base_anim_] = false;
    anim_fading_out_[base_anim_] = true;
  }

  base_anim_ = id;

  if (id != ANIM_NONE) {
    // if we have a new animation, enable it and fade it in
    anims_[id]->setEnabled(true);
    anims_[id]->setWeight(0);
    anim_fading_out_[id] = false;
    anim_fading_in_[id] = true;
    if (reset) anims_[id]->setTimePosition(0);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void SinbadCharacterController::SetTopAnimation(AnimID id, bool reset) {
  if (top_anim_ != ANIM_NONE) {
    // if we have an old animation, fade it out
    anim_fading_in_[top_anim_] = false;
    anim_fading_out_[top_anim_] = true;
  }

  top_anim_ = id;

  if (id != ANIM_NONE) {
    // if we have a new animation, enable it and fade it in
    anims_[id]->setEnabled(true);
    anims_[id]->setWeight(0);
    anim_fading_out_[id] = false;
    anim_fading_in_[id] = true;
    if (reset) anims_[id]->setTimePosition(0);
  }
}
} //namespace Context