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
#include <SDL2/SDL.h>
}

#include <set>

namespace Context {

class KeyboardListener {
 public:
  virtual void KeyDown(SDL_Keycode sym) {}
  virtual void KeyUp(SDL_Keycode sym) {}
};

class MouseListener {
 public:
  virtual void Move(int x, int y) {}
  virtual void Move(int x, int y, int dx, int dy, bool left, bool right, bool middle) {}
  virtual void Wheel(int x, int y) {}
  virtual void LbDown(int x, int y) {}
  virtual void LbUp(int x, int y) {}
  virtual void RbDown(int x, int y) {}
  virtual void RbUp(int x, int y) {}
  virtual void MbDown(int x, int y) {}
  virtual void MbUp(int x, int y) {}
};

class JoyListener {
 public:
  virtual void Axis(int which, int axis, int value) {}
  virtual void BtDown(int which, int button) {}
  virtual void BtUp(int which, int button) {}
  virtual void Hat(int which, int hat, int value) {}
  virtual void Ball(int which, int ball, int xrel, int yrel) {}
};

class OtherEventListener {
 public:
  virtual void Event(const SDL_Event &evt) {}
  virtual void Quit() {}
  virtual void Other(Uint8 type, int code, void *data1, void *data2) {}
};

class InputSequencer {
 protected:
  using KeyboardListenersList = std::vector<KeyboardListener *>;
  using MouseListenersList = std::vector<MouseListener *>;
  using JoyListenersList = std::vector<JoyListener *>;
  using OtherListenersList = std::vector<OtherEventListener *>;

 protected:
  KeyboardListenersList kb_listeners;
  MouseListenersList ms_listeners;
  JoyListenersList joy_listeners;
  OtherListenersList other_listeners;

 public:
  virtual void RegKbListener(KeyboardListener *l);
  virtual void UnregKbListener(KeyboardListener *l);
  virtual void RegMsListener(MouseListener *l);
  virtual void UnregMsListener(MouseListener *l);
  virtual void RegJoyListener(JoyListener *l);
  virtual void UnregJoyListener(JoyListener *l);
  virtual void RegEventListener(OtherEventListener *l);
  virtual void UnregEventListener(OtherEventListener *l);
  virtual void Capture();
};
}
