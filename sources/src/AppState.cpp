// This source file is part of context-project
// Created by Andrew Vasiliev

#include "AppState.h"

//#include <OgreSceneLoaderManager.h>

#include "pcheader.h"

using namespace std;

namespace glue {

//----------------------------------------------------------------------------------------------------------------------
AppState::AppState() {}

//----------------------------------------------------------------------------------------------------------------------
AppState::~AppState() {}

//----------------------------------------------------------------------------------------------------------------------
void AppState::LoadFromFile(const string &file_name, const string &group) {
  auto *scene = Ogre::Root::getSingleton().getSceneManager("Default");
  auto *root = scene->getRootSceneNode();
  root->loadChildren(file_name);
  //Ogre::SceneLoaderManager::getSingleton().load(file_name, group, scene->getRootSceneNode());
}

//----------------------------------------------------------------------------------------------------------------------
void AppState::ChangeState(unique_ptr<AppState> &&app_state) {
  next_ = move(app_state);
  dirty_ = true;
}

//----------------------------------------------------------------------------------------------------------------------
void AppState::ChangeState() { dirty_ = true; }

//----------------------------------------------------------------------------------------------------------------------
void AppState::SetNextState(unique_ptr<AppState> &&next_state) { next_ = move(next_state); }

//----------------------------------------------------------------------------------------------------------------------
bool AppState::IsDirty() const { return dirty_; }

}  // namespace glue
