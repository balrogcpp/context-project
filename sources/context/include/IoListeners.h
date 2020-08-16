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

extern "C" {
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_events.h>
}

#include <cstdint>

namespace io {
//----------------------------------------------------------------------------------------------------------------------
class KeyboardListener {
 public:
  virtual void KeyDown(SDL_Keycode sym) {}
  virtual void KeyUp(SDL_Keycode sym) {}
};
//----------------------------------------------------------------------------------------------------------------------
class MouseListener {
 public:
  virtual void Move(int32_t x, int32_t y) {}
  virtual void Move(int32_t x, int32_t y, int32_t dx, int32_t dy, bool left, bool right, bool middle) {}
  virtual void Wheel(int32_t x, int32_t y) {}
  virtual void LbDown(int32_t x, int32_t y) {}
  virtual void LbUp(int32_t x, int32_t y) {}
  virtual void RbDown(int32_t x, int32_t y) {}
  virtual void RbUp(int32_t x, int32_t y) {}
  virtual void MbDown(int32_t x, int32_t y) {}
  virtual void MbUp(int32_t x, int32_t y) {}
};
//----------------------------------------------------------------------------------------------------------------------
class JoyListener {
 public:
  virtual void Axis(int32_t which, int32_t axis, int32_t value) {}
  virtual void BtDown(int32_t which, int32_t button) {}
  virtual void BtUp(int32_t which, int32_t button) {}
  virtual void Hat(int32_t which, int32_t hat, int32_t value) {}
  virtual void Ball(int32_t which, int32_t ball, int32_t xrel, int32_t yrel) {}
};
//----------------------------------------------------------------------------------------------------------------------
class OtherEventListener {
 public:
  virtual void Event(const SDL_Event &evt) {}
  virtual void Quit() {}
  virtual void Other(Uint8 type, int32_t code, void *data1, void *data2) {}
};
//----------------------------------------------------------------------------------------------------------------------
class InputListener :
    public JoyListener,
    public KeyboardListener,
    public MouseListener,
    public OtherEventListener {
};
} //namespace io