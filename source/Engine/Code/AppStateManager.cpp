/// created by Andrey Vasiliev

#include "pch.h"
#include "AppStateManager.h"
#include "SDLListener.h"
#include <Ogre.h>

namespace gge {
AppStateManager::AppStateManager() {}
AppStateManager::~AppStateManager() {}
void AppStateManager::Init() {
  Ogre::Root::getSingleton().addFrameListener(this);
  RegAppState(std::make_shared<AppStateImpl>());
}

void AppStateManager::OnSetUp() {}
void AppStateManager::OnUpdate(float time) {}
void AppStateManager::OnClean() { Ogre::Root::getSingleton().addFrameListener(this); }
AppState *AppStateManager::GetActiveAppState() { return activeAppState.get(); }

void AppStateManager::RegAppState(std::shared_ptr<AppState> appState) {
  const std::string name = appState->GetName();
  ASSERTION(!appStateList.count(name), "[AppStateManager] appState with this name already registered");
  appStateList[name] = appState;
  if (!activeAppState) activeAppState = appState;
}

void AppStateManager::UnregAppState(std::shared_ptr<AppState> appState) {
  ASSERTION(appState, "[AppStateManager] appStatePtr is NULL");
  const std::string name = appState->GetName();
  if (appStateList.count(name)) appStateList.erase(name);
}

void AppStateManager::UnregAppState(const std::string &name) {
  if (appStateList.count(name)) appStateList.erase(name);
}

void AppStateManager::SetActiveAppState(std::shared_ptr<AppState> appState) {
  ASSERTION(appState, "[AppStateManager] appStatePtr is NULL");
  if (!appStateList.count(appState->GetName())) RegAppState(appState);
  InputSequencer::GetInstance().UnregWindowListener(activeAppState.get());
  InputSequencer::GetInstance().UnregDeviceListener(activeAppState.get());
  activeAppState->OnClean();
  activeAppState.reset();
  activeAppState = appState;
  activeAppState->OnSetUp();
  InputSequencer::GetInstance().RegWindowListener(activeAppState.get());
  InputSequencer::GetInstance().RegDeviceListener(activeAppState.get());
}

void AppStateManager::SetActiveAppState(const std::string &name) {
  ASSERTION(appStateList.count(name), "[AppStateManager] appState with this name not registered");
  const auto &appState = appStateList[name];
  ASSERTION(appState, "[AppStateManager] appStatePtr is NULL");
  InputSequencer::GetInstance().UnregWindowListener(activeAppState.get());
  InputSequencer::GetInstance().UnregDeviceListener(activeAppState.get());
  activeAppState->OnClean();
  activeAppState.reset();
  activeAppState = appState;
  activeAppState->OnSetUp();
  InputSequencer::GetInstance().RegWindowListener(activeAppState.get());
  InputSequencer::GetInstance().RegDeviceListener(activeAppState.get());
}

bool AppStateManager::frameRenderingQueued(const Ogre::FrameEvent &evt) {
  activeAppState->OnUpdate(evt.timeSinceLastFrame);
  return true;
};

bool AppStateManager::frameStarted(const Ogre::FrameEvent &evt) {
  activeAppState->BeforeRender(evt.timeSinceLastFrame);
  return true;
};

bool AppStateManager::frameEnded(const Ogre::FrameEvent &evt) {
  activeAppState->AfterRender(evt.timeSinceLastFrame);
  return true;
};
}  // namespace gge
