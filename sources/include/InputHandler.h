// MIT License
//
// Copyright (c) 2021 Andrew Vasiliev
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once
#include <vector>

#include "Input.h"
#include "Singleton.h"
#include "System.h"

namespace xio {

class CommandObserver {
 public:
};

class MutedInputObserver : public InputObserverI {
 public:
  MutedInputObserver();
  virtual ~MutedInputObserver();
};

class InputHandler final : public InputObserver, public Singleton<InputHandler> {
 public:
  InputHandler();
  virtual ~InputHandler();

  void RegObserver(view_ptr<MutedInputObserver> p);
  void UnregObserver(view_ptr<MutedInputObserver> p);

  void Pause();
  void Resume();
  void Cleanup();
  void Update(float time);

  // These methods are used to turn off inputs in for some listeners by request

  // Keyboard
  void OnKeyDown(SDL_Keycode sym) override;
  void OnKeyUp(SDL_Keycode sym) override;
  void OnTextInput(const char *text) override;

  // Mouse
  void OnMouseMove(int dx, int dy) override;
  void OnMouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) override;
  void OnMouseWheel(int x, int y) override;
  void OnMouseLbDown(int x, int y) override;
  void OnMouseLbUp(int x, int y) override;
  void OnMouseRbDown(int x, int y) override;
  void OnMouseRbUp(int x, int y) override;
  void OnMouseMbDown(int x, int y) override;
  void OnMouseMbUp(int x, int y) override;

 private:
  std::vector<view_ptr<MutedInputObserver>> io_listeners;
  // std::vector<view_ptr<CommandObserver>> cmd_listeners;
  bool paused_ = false;
};

}  // namespace xio
