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

#pragma once

#include "AppState.h"
#include "Singleton.h"

namespace Context {

class AppStateManager : public Singleton {
 public:
//----------------------------------------------------------------------------------------------------------------------
  static AppStateManager &GetSingleton() {
    static AppStateManager singleton;
    return singleton;
  }

 private:
  bool waiting_ = false;
  std::unique_ptr<AppState> cur_state_;
  std::unique_ptr<AppState> next_state_;

 public:
//----------------------------------------------------------------------------------------------------------------------
  void SetInitialState(std::unique_ptr<AppState> &&state) {
    cur_state_ = move(state);
  }
//----------------------------------------------------------------------------------------------------------------------
  void SetNextState(std::unique_ptr<AppState> &&state) {
    next_state_ = move(state);
  }
//----------------------------------------------------------------------------------------------------------------------
  void GoNextState() {
    if (next_state_) {
      cur_state_ = move(next_state_);
      waiting_ = true;
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  void InitCurrState() {
    cur_state_->Init();
  }
//----------------------------------------------------------------------------------------------------------------------
  AppState* GetCurState() {
    return cur_state_.get();
  }

  AppState* GetNextState() {
    return next_state_.get();
  }

  bool IsWaiting() const noexcept {
    return waiting_;
  }

  void ClearWaiting() {
    waiting_ = false;
  }
};
}
