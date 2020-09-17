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

#include "Input.h"

#include <iostream>

namespace xio {

class VerboseListener final : public InputObserver {
 private:
//----------------------------------------------------------------------------------------------------------------------
  void KeyDown(SDL_Keycode sym) final {
    if (print_) {
      std::cout << __func__;
      std::cout << ' ';
      std::cout << "sym ";
      std::cout << sym;
      std::cout << '\n';
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  void KeyUp(SDL_Keycode sym) final {
    if (print_) {
      std::cout << __func__;
      std::cout << ' ';
      std::cout << sym;
      std::cout << '\n';
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  void Move(int x, int y, int dx, int dy, bool left, bool right, bool middle) final {
    if (print_) {
      std::cout << __func__;
      std::cout << ' ';
      std::cout << " x ";
      std::cout << x << ' ';
      std::cout << "y" << ' ';
      std::cout << y << ' ';
      std::cout << "dx" << ' ';
      std::cout << dx << ' ';
      std::cout << "dy" << ' ';
      std::cout << dy << ' ';
      std::cout << "left" << ' ';
      std::cout << left << ' ';
      std::cout << "right" << ' ';
      std::cout << right << ' ';
      std::cout << "middle" << ' ';
      std::cout << middle << ' ';
      std::cout << '\n';
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  void Wheel(int x, int y) final {
    if (print_) {
      std::cout << __func__;
      std::cout << "Up" << ' ';
      std::cout << x << ' ';
      std::cout << "Down" << ' ';
      std::cout << y << '\n';
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  void LbDown(int x, int y) final {
    if (print_) {
      std::cout << __func__;
      std::cout << ' ';
      std::cout << "x" << ' ';
      std::cout << x << ' ';
      std::cout << "y" << ' ';
      std::cout << y << ' ';
      std::cout << '\n';
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  void LbUp(int x, int y) final {
    if (print_) {
      std::cout << __func__;
      std::cout << ' ';
      std::cout << "x" << ' ';
      std::cout << x << ' ';
      std::cout << "y" << ' ';
      std::cout << y << ' ';
      std::cout << '\n';
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  void RbDown(int x, int y) final {
    if (print_) {
      std::cout << __func__;
      std::cout << ' ';
      std::cout << "x" << ' ';
      std::cout << x << ' ';
      std::cout << "y" << ' ';
      std::cout << y << ' ';
      std::cout << '\n';
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  void RbUp(int x, int y) final {
    if (print_) {
      std::cout << __func__;
      std::cout << ' ';
      std::cout << "x" << ' ';
      std::cout << x << ' ';
      std::cout << "y" << ' ';
      std::cout << y << ' ';
      std::cout << '\n';
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  void MbDown(int x, int y) final {
    if (print_) {
      std::cout << __func__;
      std::cout << ' ';
      std::cout << "x" << ' ';
      std::cout << x << ' ';
      std::cout << "y" << ' ';
      std::cout << y << ' ';
      std::cout << '\n';
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  void MbUp(int x, int y) final {
    if (print_) {
      std::cout << __func__;
      std::cout << ' ';
      std::cout << "x" << ' ';
      std::cout << x << ' ';
      std::cout << "y" << ' ';
      std::cout << y << ' ';
      std::cout << '\n';
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  void Axis(int which, int axis, int value) final {
    if (print_) {
      std::cout << __func__;
      std::cout << ' ';
      std::cout << "which" << ' ';
      std::cout << which << ' ';
      std::cout << "axis" << ' ';
      std::cout << axis << ' ';
      std::cout << "value" << ' ';
      std::cout << value << ' ';
      std::cout << '\n';
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  void BtDown(int which, int button) final {
    if (print_) {
      std::cout << __func__;
      std::cout << ' ';
      std::cout << "which" << ' ';
      std::cout << which << ' ';
      std::cout << "button" << ' ';
      std::cout << button << ' ';
      std::cout << '\n';
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  void BtUp(int which, int button) final {
    if (print_) {
      std::cout << __func__;
      std::cout << ' ';
      std::cout << "which" << ' ';
      std::cout << which << ' ';
      std::cout << "button" << ' ';
      std::cout << button << ' ';
      std::cout << '\n';
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  void Hat(int which, int hat, int value) final {
    if (print_) {
      std::cout << __func__;
      std::cout << ' ';
      std::cout << "which" << ' ';
      std::cout << which << ' ';
      std::cout << "hat" << ' ';
      std::cout << hat << ' ';
      std::cout << "value" << ' ';
      std::cout << value << ' ';
      std::cout << '\n';
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  void Ball(int which, int ball, int xrel, int yrel) final {
    if (print_) {
      std::cout << __func__;
      std::cout << ' ';
      std::cout << "which" << ' ';
      std::cout << which << ' ';
      std::cout << "ball" << ' ';
      std::cout << ball << ' ';
      std::cout << "yrel" << ' ';
      std::cout << yrel << ' ';
      std::cout << '\n';
    }
  }

 private:
  bool print_ = false;

 public:
  bool IsPrint() const noexcept {
    return print_;
  }
//----------------------------------------------------------------------------------------------------------------------
  void SetPrint(bool print) noexcept {
    print_ = print;
  }
};
}