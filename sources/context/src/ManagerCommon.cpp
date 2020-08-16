//Cpp file for dummy context2_deps target
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

#include "ManagerCommon.hpp"
#include "IO.hpp"
#include "ContextManager.hpp"

namespace Context {

void ManagerCommon::SetupGlobal() {
  SetOgreScene(ContextManager::GetSingleton().GetOgreScenePtr());
  SetCameraMan(ContextManager::GetSingleton().GetCameraMan());
  SetOgreCamera(ContextManager::GetSingleton().GetOgreCamera());
  SetOgreViewport(ContextManager::GetSingleton().GetOgreViewport());
  SetOgreRoot(ContextManager::GetSingleton().GetOgreRootPtr());
  SetOgreRootNode(ContextManager::GetSingleton().GetOgreScenePtr()->getRootSceneNode());

  ContextManager::GetSingleton().GetOgreRootPtr()->addFrameListener(this);
  io::InputManager::GetSingleton().RegisterListener(this);

  registered_ = true;
}

void ManagerCommon::ResetGlobal() {
  if (registered_) {
    ContextManager::GetSingleton().GetOgreRootPtr()->removeFrameListener(this);
    io::InputManager::GetSingleton().UnregisterListener(this);
  }
}

void ManagerCommon::SetOgreScene(Ogre::SceneManager *scene_ptr) {
  this->ogre_scene_manager_ = scene_ptr;
}

void ManagerCommon::SetCameraMan(std::shared_ptr<CameraMan> camera_ptr) {
  this->camera_man_ = camera_ptr;
}

void ManagerCommon::SetOgreCamera(Ogre::Camera *ogreCamera) {
  this->ogre_camera_ = ogreCamera;
}

void ManagerCommon::SetOgreViewport(Ogre::Viewport *ogreViewport) {
  this->ogre_viewport_ = ogreViewport;
}

void ManagerCommon::SetOgreRoot(Ogre::Root *ogreRoot) {
  ManagerCommon::ogre_root_ = ogreRoot;
}
void ManagerCommon::SetOgreRootNode(Ogre::SceneNode *ogre_root_node) {
  ogre_root_node_ = ogre_root_node;
}

}
