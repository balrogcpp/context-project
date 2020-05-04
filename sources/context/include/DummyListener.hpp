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

#include "InputListener.hpp"
#include "Io.hpp"

namespace Context {

class DummyListener : public Context::InputListener {
 public:

  DummyListener() = default;

 private:
  bool print_ = false;

 public:
  bool IsPrint() const {
    return print_;
  }

  void SetPrint(bool print) {
    print_ = print;
  }

 protected:
  void KeyDown(SDL_Keycode sym) final;

  void KeyUp(SDL_Keycode sym) final;

  void MouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) final;

  void MouseWheel(int x, int y) final;

  void LeftButtonDown(int x, int y) final;

  void LeftButtonUp(int x, int y) final;

  void RightButtonDown(int x, int y) final;

  void RightButtonUp(int x, int y) final;

  void MiddleButtonDown(int x, int y) final;

  void MiddleButtonUp(int x, int y) final;

  void JoyAxis(int which, int axis, int value) final;

  void JoyButtonDown(int which, int button) final;

  void JoyButtonUp(int which, int button) final;

  void JoyHat(int which, int hat, int value) final;

  void JoyBall(int which, int ball, int xrel, int yrel) final;

  void ControllerAxis(int which, int axis, int value) final;

  void ControllerButtonDown(int which, int button) final;

  void ControllerButtonUp(int which, int button) final;

  void ControllerHat(int which, int hat, int value) final;

  void ControllerBall(int which, int ball, int xrel, int yrel) final;

  void Event(const SDL_Event &evt) final;

  void Quit() final;

  void User(Uint8 type, int code, void *data1, void *data2) final;

};

}
