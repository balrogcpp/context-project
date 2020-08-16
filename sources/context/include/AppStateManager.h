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

namespace Context {

class AppStateManager {
 public:
  static AppStateManager AppStateSingleton;

 public:

  static AppStateManager *GetSingletonPtr() {
    return &AppStateSingleton;
  }

  static AppStateManager &GetSingleton() {
    return AppStateSingleton;
  }

 public:

  void SetInitialState(const std::shared_ptr<AppState> &state);

  void SetCurrentState(const std::shared_ptr<AppState> &state);

  void SetNextState(const std::shared_ptr<AppState> &state);

  void GoNextState();

  void CleanupResources();

  void Reset();

  void ResetGlobals();

  AppState *GetCurState();

  AppState *GetPrevState();

  AppState *GetNextState();

 public:
  bool garbage_ = false;

  bool waiting_ = false;

  std::shared_ptr<AppState> cur_state_;

  std::shared_ptr<AppState> prev_state_;

  std::shared_ptr<AppState> next_state_;
};

}
