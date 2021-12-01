// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "StateManager.h"

using namespace std;

namespace glue {

//----------------------------------------------------------------------------------------------------------------------
StateManager::StateManager() {}

//----------------------------------------------------------------------------------------------------------------------
StateManager::~StateManager() {}

//----------------------------------------------------------------------------------------------------------------------
void StateManager::InitCurState() {
  cur_state->SetUp();
  Ogre::Root::getSingleton().addFrameListener(cur_state.get());
}

//----------------------------------------------------------------------------------------------------------------------
void StateManager::InitNextState() {
  if (cur_state) {
    cur_state->Cleanup();
    Ogre::Root::getSingleton().removeFrameListener(cur_state.get());
  }

  cur_state = move(cur_state->next);

  if (cur_state) {
    cur_state->SetUp();
    Ogre::Root::getSingleton().addFrameListener(cur_state.get());
  }
}

//----------------------------------------------------------------------------------------------------------------------
void StateManager::SetInitialState(unique_ptr<AppState> &&next_state) {
  cur_state = move(next_state);
  Ogre::Root::getSingleton().addFrameListener(cur_state.get());
}

//----------------------------------------------------------------------------------------------------------------------
void StateManager::Update(float time) {
  if (cur_state) {
    cur_state->Update(time);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool StateManager::IsActive() const { return static_cast<bool>(cur_state); }

//---------------------------------------------------------------------------------------------------------------------
bool StateManager::IsDirty() const { return cur_state->IsDirty(); }

//---------------------------------------------------------------------------------------------------------------------
void StateManager::Pause() { cur_state->Pause(); }

//---------------------------------------------------------------------------------------------------------------------
void StateManager::Resume() { cur_state->Resume(); }

}  // namespace glue
