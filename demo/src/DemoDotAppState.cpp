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

#include "Application.h"
#include "DemoDotAppState.h"
#include "CameraMan.h"
#include <OgreSceneLoaderManager.h>

namespace Demo {

using namespace Context;

DemoDotAppState::DemoDotAppState() {}
DemoDotAppState::~DemoDotAppState() {}

void DemoDotAppState::KeyDown(SDL_Keycode sym) {
  if (SDL_GetScancodeFromKey(sym) == SDL_SCANCODE_ESCAPE) {
//    Application::Instance().GoNextState();
  }
}

void DemoDotAppState::Clear() {
}

void DemoDotAppState::Init() {
//  Application::Instance().SetNextState(std::make_unique<DemoDotAppState>());
//  ContextManager::Instance().UpdateCursorStatus({false, true, true});

//  Application::Instance().GetCameraMan()->SetStyle(CameraStyle::FPS);

//  Context::DotSceneLoaderB::Instance().load("test.scene",
//                                              Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME,
//                                              ogre_scene_manager_->getRootSceneNode());

  Ogre::SceneLoaderManager::getSingleton().load("test.scene",
                                                Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME,
                                                scene_->getRootSceneNode());

  forest_.Create();
//  auto *mSoundManager = OgreOggSound::OgreOggSoundManager::getSingletonPtr();
//
//  mSoundManager->setResourceGroupName(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
//  mSoundManager->createSound("AmbientMusicDemo",
//                             "22384__dobroide__20060824-forest03.ogg",
//                             false,
//                             true,
//                             true,
//                             scene_);
//
//  if (mSoundManager->getSound("SceneManagerInstance1")) {
//    mSoundManager->getSound("SceneManagerInstance1")->play();
//  }

  scene_->setShadowColour(Ogre::ColourValue(0.2));
}
}