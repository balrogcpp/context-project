/*
MIT License

Copyright (c) 2020 Andrey Vasiliev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "pcheader.hpp"

#include "AppState.h"
#include "ContextManager.h"

namespace Context {

//----------------------------------------------------------------------------------------------------------------------
AppState::AppState() {
}
//----------------------------------------------------------------------------------------------------------------------
AppState::~AppState() {
  if (registered_) {
    ContextManager::GetSingleton().GetOgreRootPtr()->removeFrameListener(this);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void AppState::SetupGlobals() {
  ogre_root = ContextManager::GetSingleton().GetOgreRootPtr();
  ogre_scene_manager_ = ContextManager::GetSingleton().GetOgreScenePtr();
  ogre_viewport_ = ContextManager::GetSingleton().GetOgreViewport();
  ogre_camera_ = ContextManager::GetSingleton().GetOgreCamera();
  camera_man_ = ContextManager::GetSingleton().GetCameraMan();
  io::InputSequencer::GetSingleton().RegisterListener(this);
  ContextManager::GetSingleton().GetOgreRootPtr()->addFrameListener(this);
  registered_ = true;
}
//----------------------------------------------------------------------------------------------------------------------
void AppState::ResetGlobals() {
  if (registered_) {
    ContextManager::GetSingleton().GetOgreRootPtr()->removeFrameListener(this);
    io::InputSequencer::GetSingleton().UnregisterListener(this);
  }
}
} //namespace Context
