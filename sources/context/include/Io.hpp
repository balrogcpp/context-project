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

struct KeyboardEvent {
  SDL_Keycode code;
};

class KeyboardListener {
 public:
  virtual ~KeyboardListener() = default;
  virtual void KeyDown(SDL_Keycode sym);
  virtual void KeyUp(SDL_Keycode sym);
};
// class KeyboardListener

#pragma pack(push, 1)
struct MouseEvent {
  int x;
  int y;
  int dx;
  int dy;
  bool left;
  bool right;
  bool middle;
};
#pragma pack(pop)

/**
 * @brief
 */
class MouseListener {
 public:
  virtual ~MouseListener() = default;
  virtual void MouseMove(int x, int y);
  virtual void MouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle);
  virtual void MouseWheel(int x, int y);
  virtual void LeftButtonDown(int x, int y);
  virtual void LeftButtonUp(int x, int y);
  virtual void RightButtonDown(int x, int y);
  virtual void RightButtonUp(int x, int y);
  virtual void MiddleButtonDown(int x, int y);
  virtual void MiddleButtonUp(int x, int y);
  virtual void Universal(const MouseEvent &evt);
};
// class MouseListener

#pragma pack(push, 1)
struct JoyEvent {
  int which;
  int ball;
  int xrel;
  int yrel;
};
#pragma pack(pop)

class JoyListener {
 public:

  virtual ~JoyListener() = default;
  virtual void JoyAxis(int which, int axis, int value);
  virtual void JoyButtonDown(int which, int button);
  virtual void JoyButtonUp(int which, int button);
  virtual void JoyHat(int which, int hat, int value);
  virtual void JoyBall(int which, int ball, int xrel, int yrel);
};
// class JoyListener

#pragma pack(push, 1)
struct ControllerEvent {
  int which;
  int ball;
  int xrel;
  int yrel;
};
#pragma pack(pop)

class ControllerListener {
 public:
  virtual ~ControllerListener() = default;
  virtual void ControllerAxis(int which, int axis, int value);
  virtual void ControllerButtonDown(int which, int button);
  virtual void ControllerButtonUp(int which, int button);
  virtual void ControllerHat(int which, int hat, int value);
  virtual void ControllerBall(int which, int ball, int xrel, int yrel);
};
// class ControllerListener

class OtherEventListener {
 public:
  virtual ~OtherEventListener() = default;
  virtual void Event(const SDL_Event &evt);
  virtual void Quit();
  virtual void User(Uint8 type, int code, void *data1, void *data2);
};
// class OtherEventListener


enum class EventType {
  MOUSE,
  KEYBOARD,
  JOY,
  CONTROLLER
};

#pragma pack(push, 1)
struct IoEvent {
  EventType type;
  union {
    MouseEvent mouseEvt;
    ControllerEvent controllerEvt;
    JoyEvent joyEvt;
    KeyboardEvent keyboardEvent;
  };
};
#pragma pack(pop)

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
  virtual void RegKeyboardListener(KeyboardListener *l);
  virtual void UnregKeyboardListener(KeyboardListener *l);
  virtual void RegMouseListener(MouseListener *l);
  virtual void UnregMouseListener(MouseListener *l);
  virtual void RegJoyListener(JoyListener *l);
  virtual void UnregJoyListener(JoyListener *l);
  virtual void RegControllerListener(ControllerListener *l);
  virtual void UnregControllerListener(ControllerListener *l);
  virtual void RegEventListener(OtherEventListener *l);
  virtual void UnregEventListener(OtherEventListener *l);
  virtual void Capture();
};
// class InputManager

}
