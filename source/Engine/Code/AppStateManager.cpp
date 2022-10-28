/// created by Andrey Vasiliev

#include "pch.h"
#include "AppStateManager.h"

namespace Glue {
AppStateManager::AppStateManager() {}
AppStateManager::~AppStateManager() {}
void AppStateManager::Init() {
  Ogre::Root::getSingleton().addFrameListener(this);
  RegAppState(std::make_shared<AppStateImpl>());
}

void AppStateManager::OnSetUp() {}
void AppStateManager::OnUpdate(float time) {}
void AppStateManager::OnClean() { Ogre::Root::getSingleton().addFrameListener(this); }

void AppStateManager::RegAppState(std::shared_ptr<AppState> appState) {
  const std::string name = appState->GetName();
  OgreAssert(!appStateList.count(name), std::string("[AppStateManager] appState with name " + name + " already registered").c_str());
  appStateList[name] = appState;
  if (!activeAppState) activeAppState = appState;
}
void AppStateManager::UnregAppState(std::shared_ptr<AppState> appState) {
  OgreAssert(appState, "[AppStateManager] appStatePtr is NULL");
  const std::string name = appState->GetName();
  if (appStateList.count(name)) appStateList.erase(name);
}
void AppStateManager::UnregAppState(const std::string &name) {
  if (appStateList.count(name)) appStateList.erase(name);
}

void AppStateManager::SetActiveAppState(std::shared_ptr<AppState> appState) {
  OgreAssert(appState, "[AppStateManager] appStatePtr is NULL");
  if (!appStateList.count(appState->GetName())) RegAppState(appState);
  activeAppState->OnClean();
  activeAppState.reset();
  activeAppState = appState;
  activeAppState->OnSetUp();
}
void AppStateManager::SetActiveAppState(const std::string &name) {
  OgreAssert(appStateList.count(name), std::string("[AppStateManager] appState with name " + name + " not registered").c_str());
  const auto &appStatePtr = appStateList[name];
  OgreAssert(appStatePtr, "[AppStateManager] appStatePtr is NULL");
  activeAppState->OnClean();
  activeAppState.reset();
  activeAppState = appStatePtr;
  activeAppState->OnSetUp();
}
std::shared_ptr<AppState> AppStateManager::GetActiveAppState() { return activeAppState; }

bool AppStateManager::frameStarted(const Ogre::FrameEvent &evt) {
  activeAppState->OnUpdate(evt.timeSinceLastFrame);
  return true;
};
bool AppStateManager::frameRenderingQueued(const Ogre::FrameEvent &evt) { return true; };
bool AppStateManager::frameEnded(const Ogre::FrameEvent &evt) { return true; };
}  // namespace Glue
