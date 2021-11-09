// This source file is part of "glue project"
// Created by Andrew Vasiliev

#include "pcheader.h"
#include "StateManager.h"

using namespace std;

namespace glue {

//----------------------------------------------------------------------------------------------------------------------
StateManager::StateManager() {}

//----------------------------------------------------------------------------------------------------------------------
StateManager::~StateManager() {}

//----------------------------------------------------------------------------------------------------------------------
void StateManager::InitCurState() {
  cur_state_->Init();
  Ogre::Root::getSingleton().addFrameListener(cur_state_.get());
}

//----------------------------------------------------------------------------------------------------------------------
void StateManager::InitNextState() {
  if (cur_state_) {
    cur_state_->Cleanup();
    Ogre::Root::getSingleton().removeFrameListener(cur_state_.get());
  }

  cur_state_ = move(cur_state_->next_);

  if (cur_state_) {
    cur_state_->Init();
    Ogre::Root::getSingleton().addFrameListener(cur_state_.get());
  }
}

//----------------------------------------------------------------------------------------------------------------------
void StateManager::SetInitialState(unique_ptr<AppState> &&next_state) {
  cur_state_ = move(next_state);
  Ogre::Root::getSingleton().addFrameListener(cur_state_.get());
}

//----------------------------------------------------------------------------------------------------------------------
void StateManager::Update(float time) {
  if (cur_state_) cur_state_->Update(time);
}

//----------------------------------------------------------------------------------------------------------------------
bool StateManager::IsActive() const { return static_cast<bool>(cur_state_); }

//---------------------------------------------------------------------------------------------------------------------
bool StateManager::IsDirty() const { return cur_state_->IsDirty(); }

//---------------------------------------------------------------------------------------------------------------------
void StateManager::Pause() { cur_state_->Pause(); }

//---------------------------------------------------------------------------------------------------------------------
void StateManager::Resume() { cur_state_->Resume(); }

}  // namespace glue
