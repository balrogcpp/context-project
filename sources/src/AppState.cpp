//MIT License
//
//Copyright (c) 2020 Andrey Vasiliev
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#include "pcheader.h"
#include "AppState.h"

#include <OgreSceneLoaderManager.h>

using namespace std;

namespace xio {
//----------------------------------------------------------------------------------------------------------------------
AppState::AppState()
	:dirty_(false)
{}

//----------------------------------------------------------------------------------------------------------------------
AppState::~AppState(){}

//----------------------------------------------------------------------------------------------------------------------
void AppState::LoadFromFile(const string &file_name) {
  auto *scene = Ogre::Root::getSingleton().getSceneManager("Default");
  auto group = Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME;
  Ogre::SceneLoaderManager::getSingleton().load(file_name, group, scene->getRootSceneNode());
}

//----------------------------------------------------------------------------------------------------------------------
void AppState::SwitchNextState(unique_ptr<AppState> &&app_state) {
  next_ = move(app_state);
  dirty_ = true;
}

//----------------------------------------------------------------------------------------------------------------------
unique_ptr<AppState> AppState::GetNextState() {
  dirty_ = false;
  return move(next_);
}

//----------------------------------------------------------------------------------------------------------------------
bool AppState::IsDirty() const {
  return dirty_;
}

} //namespace
