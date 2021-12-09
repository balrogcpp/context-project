// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "AppStateManager.h"

using namespace std;

namespace Glue {

AppStateManager::AppStateManager() {}

AppStateManager::~AppStateManager() {}

void AppStateManager::InitCurState() {
  CurrentState->SetUp();
  Ogre::Root::getSingleton().addFrameListener(CurrentState.get());
}

void AppStateManager::InitNextState() {
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

void AppStateManager::SetInitialState(unique_ptr<AppState> &&InitialState) {
  CurrentState = move(InitialState);
  Ogre::Root::getSingleton().addFrameListener(CurrentState.get());
}

void AppStateManager::Update(float PassedTime) {
  if (CurrentState) {
    CurrentState->Update(PassedTime);
  }
}

bool AppStateManager::IsActive() const { return static_cast<bool>(CurrentState); }

bool AppStateManager::IsDirty() const { return CurrentState->IsDirty(); }

void AppStateManager::Pause() { CurrentState->Pause(); }

void AppStateManager::Resume() { CurrentState->Resume(); }

}  // namespace Glue
