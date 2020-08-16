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

#include "IO.hpp"

namespace Context {

class DummyListener : public io::InputListener {
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

  void Move(int x, int y, int dx, int dy, bool left, bool right, bool middle) final;

  void Wheel(int x, int y) final;

  void LbDown(int x, int y) final;

  void LbUp(int x, int y) final;

  void RbDown(int x, int y) final;

  void RbUp(int x, int y) final;

  void MbDown(int x, int y) final;

  void MbUp(int x, int y) final;

  void Axis(int which, int axis, int value) final;

  void BtDown(int which, int button) final;

  void BtUp(int which, int button) final;

  void Hat(int which, int hat, int value) final;

  void Ball(int which, int ball, int xrel, int yrel) final;

  void Event(const SDL_Event &evt) final;

  void Quit() final;

  void Other(Uint8 type, int code, void *data1, void *data2) final;

};

}
