// This source file is part of "glue project"
// Created by Andrew Vasiliev

#pragma once
#include "Exception.h"
#include "NoCopy.h"
#include "Singleton.h"
#include "view_ptr.h"
#include <string>
#include <exception>
#include <set>
extern "C" {
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
}

namespace glue {
class InputSequencer;
class InputObserver;
class WindowObserver;

//----------------------------------------------------------------------------------------------------------------------
class InputSequencer : public LazySingleton<InputSequencer> {
 public:
  InputSequencer();
  virtual ~InputSequencer();

  void RegObserver(view_ptr<InputObserver> p);
  void UnregObserver(view_ptr<InputObserver> p);
  void RegWinObserver(view_ptr<WindowObserver> p);
  void UnregWinObserver(view_ptr<WindowObserver> p);
  void Capture();

 private:
  std::set<view_ptr<InputObserver>> io_listeners;
  std::set<view_ptr<WindowObserver>> win_listeners;
  int HandleAppEvents(void *userdata, SDL_Event *event);
};

class InputObserverI {
 public:
  // Keyboard
  virtual void OnKeyDown(SDL_Keycode sym) = 0;
  virtual void OnKeyUp(SDL_Keycode sym) = 0;

  // Mouse
  virtual void OnMouseMove(int dx, int dy) {}
  virtual void OnMouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) {}
  virtual void OnMouseWheel(int x, int y) {}
  virtual void OnMouseLbDown(int x, int y) {}
  virtual void OnMouseLbUp(int x, int y) {}
  virtual void OnMouseRbDown(int x, int y) {}
  virtual void OnMouseRbUp(int x, int y) {}
  virtual void OnMouseMbDown(int x, int y) {}
  virtual void OnMouseMbUp(int x, int y) {}
  virtual void OnTextInput(const char *text) {}

  // Joystick
  virtual void OnJoystickAxis(int which, int axis, int value) {}
  virtual void OnJoystickBtDown(int which, int button) {}
  virtual void OnJoystickBtUp(int which, int button) {}
  virtual void OnJoystickHat(int which, int hat, int value) {}
  virtual void OnJoystickBall(int which, int ball, int xrel, int yrel) {}
};

//----------------------------------------------------------------------------------------------------------------------
class InputObserver : public InputObserverI, public NoCopy {
 public:
  InputObserver();
  virtual ~InputObserver();

  // Keyboard
  void OnKeyDown(SDL_Keycode sym) override {}
  void OnKeyUp(SDL_Keycode sym) override {}

  // Mouse
  void OnMouseMove(int dx, int dy) override {}
  void OnMouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) override {}
  void OnMouseWheel(int x, int y) override {}
  void OnMouseLbDown(int x, int y) override {}
  void OnMouseLbUp(int x, int y) override {}
  void OnMouseRbDown(int x, int y) override {}
  void OnMouseRbUp(int x, int y) override {}
  void OnMouseMbDown(int x, int y) override {}
  void OnMouseMbUp(int x, int y) override {}
  void OnTextInput(const char *text) override {}

  // Joystick
  void OnJoystickAxis(int which, int axis, int value) override {}
  void OnJoystickBtDown(int which, int button) override {}
  void OnJoystickBtUp(int which, int button) override {}
  void OnJoystickHat(int which, int hat, int value) override {}
  void OnJoystickBall(int which, int ball, int xrel, int yrel) override {}
};

//----------------------------------------------------------------------------------------------------------------------
class WindowObserver : public NoCopy {
 public:
  WindowObserver();
  virtual ~WindowObserver();

  virtual void Event(const SDL_Event &evt) {}
  virtual void Quit() {}
  virtual void Pause() {}
  virtual void Resume() {}
  virtual void Other(uint8_t type, int code, void *data1, void *data2) {}
};

}  // namespace glue
