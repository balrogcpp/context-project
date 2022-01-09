// This source file is part of "glue project". Created by Andrey Vasiliev

#include "PCHeader.h"
#include "AppState.h"

using namespace std;

namespace Glue {

AppState::AppState() {
  RegMyself();
}

AppState::~AppState() {
  UnRegMyself();
}

void AppState::LoadFromFile(const string &FileName, const string &GroupName) {
  auto *scene = Ogre::Root::getSingleton().getSceneManager("Default");
  auto *root = scene->getRootSceneNode();
  root->loadChildren(FileName);
}

void AppState::ChangeState(unique_ptr<AppState> &&AppStatePtr) {
  NextState = move(AppStatePtr);
  Dirty = true;
}

void AppState::ChangeState() { Dirty = true; }

void AppState::AppendNextState(unique_ptr<AppState> &&NextStatePtr) { NextState = move(NextStatePtr); }

bool AppState::IsDirty() const { return Dirty; }

}  // namespace Glue
