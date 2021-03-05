//MIT License
//
//Copyright (c) 2021 Andrey Vasiliev
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

#include "StateManager.h"

using namespace std;

namespace xio {
StateManager::StateManager() {

}

//----------------------------------------------------------------------------------------------------------------------
StateManager::~StateManager() {

}

//----------------------------------------------------------------------------------------------------------------------
void StateManager::InitCurState() {
  cur_state_->Init();
  Ogre::Root::getSingleton().addFrameListener(cur_state_.get());
}

//----------------------------------------------------------------------------------------------------------------------
void StateManager::InitNextState() {
  if (cur_state_) {
	cur_state_->Cleanup();
	Ogre::Root::getSingleton().removeFrameListener(cur_state_.get());
  }

  cur_state_ = move(cur_state_->next_);
  cur_state_->Init();
  Ogre::Root::getSingleton().addFrameListener(cur_state_.get());
}

//----------------------------------------------------------------------------------------------------------------------
void StateManager::SetInitialState(unique_ptr<AppState> &&next_state) {
  cur_state_ = move(next_state);
  Ogre::Root::getSingleton().addFrameListener(cur_state_.get());
}

//----------------------------------------------------------------------------------------------------------------------
void StateManager::Update(float time) {
	cur_state_->Update(time);
}

//----------------------------------------------------------------------------------------------------------------------
bool StateManager::IsActive() const {
	return static_cast<bool>(cur_state_);
}

//---------------------------------------------------------------------------------------------------------------------
bool StateManager::IsDirty() const {
	return cur_state_->IsDirty();
}

//---------------------------------------------------------------------------------------------------------------------
void StateManager::Pause() {
  cur_state_->Pause();
}

//---------------------------------------------------------------------------------------------------------------------
void StateManager::Resume() {
  cur_state_->Resume();
}

} //namespace
