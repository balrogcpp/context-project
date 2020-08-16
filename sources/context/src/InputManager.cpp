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

#include "InputManager.hpp"

namespace Context {

static InputManager InputSingleton;
//----------------------------------------------------------------------------------------------------------------------
InputManager *InputManager::GetSingletonPtr() {
  return &InputSingleton;
}
//----------------------------------------------------------------------------------------------------------------------
InputManager &InputManager::GetSingleton() {
  return InputSingleton;
}
//----------------------------------------------------------------------------------------------------------------------
void InputManager::Reset() {
  waiting_for_unreg_.clear();
  kb_listeners.clear();
  ms_listeners.clear();
  joy_listeners.clear();
  other_listeners.clear();
  waiting_for_reg_.clear();

  waiting_garbage_ = false;
  waiting_reg_ = false;
}
//----------------------------------------------------------------------------------------------------------------------
void InputManager::Capture() {
  if (waiting_garbage_) {
    for (auto it : waiting_for_unreg_) {
      UnregisterListener(it);
    }

    if (capture_enable_) {
      waiting_for_unreg_.clear();
      waiting_garbage_ = false;
    }
  }

  if (waiting_reg_) {
    for (auto it : waiting_for_reg_) {
      RegisterListener(it);
    }

    if (capture_enable_) {
      waiting_for_reg_.clear();
      waiting_reg_ = false;
    }
  }

  lock_reg_ = true;

  if (capture_enable_) {
    InputSequencer::Capture();
  }

  lock_reg_ = false;
}
//----------------------------------------------------------------------------------------------------------------------
void InputManager::StopCapture() {
  capture_enable_ = false;
}
//----------------------------------------------------------------------------------------------------------------------
void InputManager::StartCapture() {
  capture_enable_ = true;
}
//----------------------------------------------------------------------------------------------------------------------
void InputManager::RegisterListener(InputListener *l) {
  if (!lock_reg_) {
    RegKbListener(l);
    RegMsListener(l);
    RegJoyListener(l);
    RegEventListener(l);
    capture_enable_ = true;
  } else {
    capture_enable_ = false;
    waiting_reg_ = true;
    waiting_for_reg_.push_back(l);
  }

}
//----------------------------------------------------------------------------------------------------------------------
void InputManager::UnregisterListener(InputListener *l) {
  if (!lock_reg_) {
    UnregKbListener(l);
    UnregMsListener(l);
    UnregJoyListener(l);
    UnregEventListener(l);
    capture_enable_ = true;
  } else {
    capture_enable_ = false;
    waiting_garbage_ = true;
    waiting_for_unreg_.push_back(l);
  }
}

} //namespace Context
