// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "StateManager.h"

using namespace std;

namespace Glue {

StateManager::StateManager() {}

StateManager::~StateManager() {}

void StateManager::InitCurState() {
  CurrentState->SetUp();
  Ogre::Root::getSingleton().addFrameListener(CurrentState.get());
}

void StateManager::InitNextState() {
  if (CurrentState) {
    CurrentState->Cleanup();
    Ogre::Root::getSingleton().removeFrameListener(CurrentState.get());
  }

  CurrentState = move(CurrentState->NextState);

  if (CurrentState) {
    CurrentState->SetUp();
    Ogre::Root::getSingleton().addFrameListener(CurrentState.get());
  }
}

void StateManager::SetInitialState(unique_ptr<AppState> &&InitialState) {
  CurrentState = move(InitialState);
  Ogre::Root::getSingleton().addFrameListener(CurrentState.get());
}

void StateManager::Update(float PassedTime) {
  if (CurrentState) {
    CurrentState->Update(PassedTime);
  }
}

bool StateManager::IsActive() const { return static_cast<bool>(CurrentState); }

bool StateManager::IsDirty() const { return CurrentState->IsDirty(); }

void StateManager::Pause() { CurrentState->Pause(); }

void StateManager::Resume() { CurrentState->Resume(); }

}  // namespace Glue
