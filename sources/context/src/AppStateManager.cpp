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

#include "pcheader.h"

#include "ContextManager.h"
#include "AppStateManager.h"
#include "DotSceneLoaderB.h"
#include "StaticForest.h"
#include "PhysicsManager.h"
#include "SoundManager.h"
#include "ConfiguratorJson.h"
#include "IO.h"

namespace Context {

AppStateManager AppStateManager::AppStateSingleton;

//----------------------------------------------------------------------------------------------------------------------
void AppStateManager::SetInitialState(const std::shared_ptr<AppState> &state) {
  if (prev_state_) {
    io::InputSequencer::GetSingleton().UnregisterListener(prev_state_.get());
    ContextManager::GetSingleton().GetOgreRootPtr()->removeFrameListener(prev_state_.get());
  }

  if (prev_state_) {
    if (prev_state_) {
      prev_state_->Reset();
      prev_state_.reset();
    }
  }

  if (state) {
    cur_state_ = state;
  }
}
//----------------------------------------------------------------------------------------------------------------------
void AppStateManager::SetCurrentState(const std::shared_ptr<AppState> &state) {
  if (prev_state_) {
    io::InputSequencer::GetSingleton().UnregisterListener(prev_state_.get());
    ContextManager::GetSingleton().GetOgreRootPtr()->removeFrameListener(prev_state_.get());
  }

  if (prev_state_) {
    if (prev_state_) {
      prev_state_->Reset();
    }
  }

  if (state) {
    cur_state_ = state;
    cur_state_->Setup();
  }
}
//----------------------------------------------------------------------------------------------------------------------
void AppStateManager::SetNextState(const std::shared_ptr<AppState> &state) {
  if (state) {
    next_state_ = state;
  }
}
//----------------------------------------------------------------------------------------------------------------------
void AppStateManager::GoNextState() {
  if (cur_state_) {
    cur_state_->Reset();
    cur_state_->ResetGlobals();
    cur_state_.reset();
  }

  if (next_state_) {
    cur_state_ = next_state_;
    next_state_.reset();
    waiting_ = true;
  }

}
//----------------------------------------------------------------------------------------------------------------------
void AppStateManager::CleanupResources() {
  if (waiting_) {
    StaticForest::GetSingleton().Reset();
    DotSceneLoaderB::GetSingleton().Reset();

    if (ConfiguratorJson::GetSingleton().GetBool("physics_enable")) {
      PhysicsManager::GetSingleton().Reset();
    }

    ContextManager::GetSingleton().GetOgreScenePtr()->setShadowTechnique(Ogre::SHADOWTYPE_NONE);

    ContextManager::GetSingleton().GetOgreScenePtr()->destroyAllEntities();
    ContextManager::GetSingleton().GetOgreScenePtr()->destroyAllLights();
    ContextManager::GetSingleton().GetOgreScenePtr()->destroyAllParticleSystems();
    ContextManager::GetSingleton().GetOgreScenePtr()->destroyAllAnimations();
    ContextManager::GetSingleton().GetOgreScenePtr()->destroyAllAnimationStates();
    ContextManager::GetSingleton().GetOgreScenePtr()->destroyAllStaticGeometry();
    ContextManager::GetSingleton().GetOgreScenePtr()->destroyAllRibbonTrails();
    ContextManager::GetSingleton().GetOgreScenePtr()->destroyAllManualObjects();
    ContextManager::GetSingleton().GetOgreScenePtr()->destroyAllInstanceManagers();
    ContextManager::GetSingleton().GetOgreScenePtr()->destroyAllBillboardChains();
    ContextManager::GetSingleton().GetOgreScenePtr()->destroyAllBillboardSets();
    ContextManager::GetSingleton().GetOgreScenePtr()->destroyAllMovableObjects();
//    ContextManager::GetSingleton().GetOgreScenePtr()->destroyAllCameras();

    ContextManager::GetSingleton().GetOgreScenePtr()->getRootSceneNode()->removeAndDestroyAllChildren();

    ContextManager::GetSingleton().SetupOgreScenePreconditions();
//    CubeMapCamera::GetSingleton().Setup();
  }

  if (waiting_) {
    cur_state_->SetupGlobals();
    cur_state_->Setup();
    waiting_ = false;
    PhysicsManager::GetSingleton().Start();
  }
}
//----------------------------------------------------------------------------------------------------------------------
void AppStateManager::Reset() {
//  if (prev_state_) {
//    prev_state_.reset();
//  }

//  if (cur_state_) {
//    cur_state_.reset();
//  }

  if (next_state_) {
    prev_state_.reset();
  }

  StaticForest::GetSingleton().Reset();
  DotSceneLoaderB::GetSingleton().Reset();
  ContextManager::GetSingleton().GetOgreScenePtr()->destroyAllEntities();
  ContextManager::GetSingleton().GetOgreScenePtr()->destroyAllLights();
  ContextManager::GetSingleton().GetOgreScenePtr()->destroyAllParticleSystems();
  ContextManager::GetSingleton().GetOgreScenePtr()->destroyAllAnimations();
  ContextManager::GetSingleton().GetOgreScenePtr()->destroyAllAnimationStates();
  ContextManager::GetSingleton().GetOgreScenePtr()->destroyAllStaticGeometry();
  ContextManager::GetSingleton().GetOgreScenePtr()->destroyAllManualObjects();
  ContextManager::GetSingleton().GetOgreScenePtr()->destroyAllInstanceManagers();
  ContextManager::GetSingleton().GetOgreScenePtr()->destroyAllBillboardChains();
  ContextManager::GetSingleton().GetOgreScenePtr()->destroyAllBillboardSets();
  ContextManager::GetSingleton().GetOgreScenePtr()->destroyAllMovableObjects();
  ContextManager::GetSingleton().GetOgreScenePtr()->destroyAllCameras();
  ContextManager::GetSingleton().GetOgreScenePtr()->getRootSceneNode()->removeAndDestroyAllChildren();
  Ogre::ResourceGroupManager::getSingleton().unloadResourceGroup(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
}
//----------------------------------------------------------------------------------------------------------------------
void AppStateManager::ResetGlobals() {
//  if (prev_state_) {
//    prev_state_.reset();
//  }

  if (cur_state_) {
    cur_state_.reset();
  }

  if (next_state_) {
    next_state_.reset();
  }

}
//----------------------------------------------------------------------------------------------------------------------
AppState *AppStateManager::GetCurState() {
  return cur_state_.get();
}
//----------------------------------------------------------------------------------------------------------------------
AppState *AppStateManager::GetPrevState() {
  return prev_state_.get();
}
//----------------------------------------------------------------------------------------------------------------------
AppState *AppStateManager::GetNextState() {
  return next_state_.get();
}

} //namespace Context
