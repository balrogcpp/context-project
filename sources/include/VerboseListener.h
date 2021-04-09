//MIT License
//
//Copyright (c) 2021 Andrew Vasiliev
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

#pragma once
#include "Input.h"

namespace xio {

class VerboseListener final : public InputObserver {
 private:
  void OnKeyDown(SDL_Keycode sym) override;
  void OnKeyUp(SDL_Keycode sym) override;
  void OnMouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) override;
  void OnMouseWheel(int x, int y) override;
  void OnMouseLbDown(int x, int y) override;
  void OnMouseLbUp(int x, int y) override;
  void OnMouseRbDown(int x, int y) override;
  void OnMouseRbUp(int x, int y) override;
  void OnMouseMbDown(int x, int y) override;
  void OnMouseMbUp(int x, int y) override;
  void OnJoystickAxis(int which, int axis, int value) override;
  void OnJoystickBtDown(int which, int button) override;
  void OnJoystickBtUp(int which, int button) override;
  void OnJoystickHat(int which, int hat, int value) override;
  void OnJoystickBall(int which, int ball, int xrel, int yrel) override;

 public:
  bool verbose = true;
};

} //namespace
