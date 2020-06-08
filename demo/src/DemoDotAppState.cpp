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

#include "Context.hpp"

#include "DemoDotAppState.h"
#include "DotSceneLoaderB.hpp"
#include "AppStateManager.hpp"
#include "MenuAppState.h"
#include "CameraMan.hpp"

namespace Demo {

using namespace Context;

DemoDotAppState::~DemoDotAppState() {

}

void DemoDotAppState::KeyDown(SDL_Keycode sym) {
  if (SDL_GetScancodeFromKey(sym) == SDL_SCANCODE_ESCAPE) {
    AppStateManager::GetSingleton().GoNextState();
  }
}

void DemoDotAppState::Reset() {
  sinbad_.reset();
}

void DemoDotAppState::Setup() {
  AppStateManager::GetSingleton().SetNextState(std::make_shared<MenuAppState>());
  ContextManager::GetSingleton().UpdateCursorStatus({false, true, true});

  ContextManager::GetSingleton().GetCameraMan()->SetStyle(CameraStyle::FPS);

  Ogre::SceneLoaderManager::getSingleton().load("test.scene",
                                                Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME,
                                                ogre_scene_manager_->getRootSceneNode());

  StaticForestManager::GetSingleton().Create();

  if (ConfigManager::GetSingleton().GetBool("sound_enable")) {
    auto *mSoundManager = OgreOggSound::OgreOggSoundManager::getSingletonPtr();

    mSoundManager->setResourceGroupName(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    mSoundManager->createSound("AmbientMusicDemo",
                               "22384__dobroide__20060824-forest03.ogg",
                               false,
                               true,
                               true,
                               ContextManager::GetSingleton().GetOgreScenePtr());

    if (mSoundManager->getSound("SceneManagerInstance1")) {
      mSoundManager->getSound("SceneManagerInstance1")->play();
    }
  }

  ogre_scene_manager_->setShadowColour(Ogre::ColourValue(0.0));
}
}