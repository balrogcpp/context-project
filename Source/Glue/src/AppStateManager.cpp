// This source file is part of "glue project". Created by Andrey Vasiliev

#include "PCHeader.h"
#include "AppStateManager.h"
#include "Input/InputSequencer.h"

using namespace std;

namespace Glue {

AppStateManager::AppStateManager() {}

AppStateManager::~AppStateManager() {}

void AppStateManager::InitCurState() {
  CurrentState->SetUp();
  InputSequencer::GetInstance().RegObserver(CurrentState.get());
  Ogre::Root::getSingleton().addFrameListener(CurrentState.get());
}

void AppStateManager::InitNextState() {
  if (CurrentState) {
    CurrentState->Cleanup();
    InputSequencer::GetInstance().UnRegObserver(CurrentState.get());
    Ogre::Root::getSingleton().removeFrameListener(CurrentState.get());
  }

  CurrentState = move(CurrentState->NextState);

  if (CurrentState) {
    CurrentState->SetUp();
    InputSequencer::GetInstance().RegObserver(CurrentState.get());
    Ogre::Root::getSingleton().addFrameListener(CurrentState.get());
  }
}

void AppStateManager::SetInitialState(unique_ptr<AppState> &&InitialState) {
  CurrentState = move(InitialState);
  InputSequencer::GetInstance().RegObserver(CurrentState.get());
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
