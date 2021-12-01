// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "AppState.h"

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
}

//----------------------------------------------------------------------------------------------------------------------
void AppState::ChangeState(unique_ptr<AppState> &&app_state) {
  next = move(app_state);
  dirty = true;
}

//----------------------------------------------------------------------------------------------------------------------
void AppState::ChangeState() { dirty = true; }

//----------------------------------------------------------------------------------------------------------------------
void AppState::AppendNextState(unique_ptr<AppState> &&next_state) { next = move(next_state); }

//----------------------------------------------------------------------------------------------------------------------
bool AppState::IsDirty() const { return dirty; }

}  // namespace glue
