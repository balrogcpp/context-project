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
  virtual ~KeyboardListener() = default;
  virtual void key_down(SDL_Keycode sym);
  virtual void key_up(SDL_Keycode sym);
};

class MouseListener {
 public:
  virtual ~MouseListener() = default;
  virtual void move(int x, int y);
  virtual void move(int x, int y, int dx, int dy, bool left, bool right, bool middle);
  virtual void wheel(int x, int y);
  virtual void lb_down(int x, int y);
  virtual void lb_up(int x, int y);
  virtual void rb_down(int x, int y);
  virtual void rb_up(int x, int y);
  virtual void mb_down(int x, int y);
  virtual void mb_up(int x, int y);
};

class JoyListener {
 public:

  virtual ~JoyListener() = default;
  virtual void axis(int which, int axis, int value);
  virtual void bt_down(int which, int button);
  virtual void bt_up(int which, int button);
  virtual void hat(int which, int hat, int value);
  virtual void ball(int which, int ball, int xrel, int yrel);
};

class ControllerListener {
 public:
  virtual ~ControllerListener() = default;
  virtual void axis(int which, int axis, int value);
  virtual void bt_down(int which, int button);
  virtual void bt_up(int which, int button);
  virtual void hat(int which, int hat, int value);
  virtual void ball(int which, int ball, int xrel, int yrel);
};

class OtherEventListener {
 public:
  virtual ~OtherEventListener() = default;
  virtual void event(const SDL_Event &evt);
  virtual void quit();
  virtual void user(Uint8 type, int code, void *data1, void *data2);
};

class InputSequencer {
 protected:
  using KeyboardListenersList = std::set<KeyboardListener *>;
  using MouseListenersList = std::set<MouseListener *>;
  using JoyListenersList = std::set<JoyListener *>;
  using EventsListenersList = std::set<OtherEventListener *>;
  using ControllerListenersList = std::set<ControllerListener *>;

 protected:
  KeyboardListenersList kb_listeners;
  MouseListenersList ms_listeners;
  JoyListenersList joy_listeners;
  ControllerListenersList cont_listeners;
  EventsListenersList other_listeners;

 public:
  virtual ~InputSequencer() = default;
  virtual void reg_kb_listener(KeyboardListener *l);
  virtual void unreg_kb_listener(KeyboardListener *l);
  virtual void reg_ms_listener(MouseListener *l);
  virtual void unreg_ms_listener(MouseListener *l);
  virtual void reg_joy_listener(JoyListener *l);
  virtual void unreg_joy_listener(JoyListener *l);
  virtual void reg_con_listener(ControllerListener *l);
  virtual void unreg_con_listener(ControllerListener *l);
  virtual void reg_event_listener(OtherEventListener *l);
  virtual void unreg_event_listener(OtherEventListener *l);
  virtual void capture();
};
// class InputManager

}
