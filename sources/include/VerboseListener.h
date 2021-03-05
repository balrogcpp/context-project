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

#pragma once
#include "Input.h"

namespace xio {

class VerboseListener final : public InputObserver {
 private:
  void OnKeyDown(SDL_Keycode sym) final;
  void OnKeyUp(SDL_Keycode sym) final;
  void OnMouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) final;
  void OnMouseWheel(int x, int y) final;
  void OnMouseLbDown(int x, int y) final;
  void OnMouseLbUp(int x, int y) final;
  void OnMouseRbDown(int x, int y) final;
  void OnMouseRbUp(int x, int y) final;
  void OnMouseMbDown(int x, int y) final;
  void OnMouseMbUp(int x, int y) final;
  void OnJoystickAxis(int which, int axis, int value) final;
  void OnJoystickBtDown(int which, int button) final;
  void OnJoystickBtUp(int which, int button) final;
  void OnJoystickHat(int which, int hat, int value) final;
  void OnJoystickBall(int which, int ball, int xrel, int yrel) final;

 private:
  bool verbose_ = false;

 public:
  bool IsVerbose() const noexcept {
	return verbose_;
  }
  void SetVerbose(bool print) noexcept {
	verbose_ = print;
  }
};
}