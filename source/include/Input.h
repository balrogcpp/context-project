// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Exception.h"
#include "NoCopy.h"
#include "Singleton.h"
#include "view_ptr.h"
#include <exception>
#include <set>
#include <string>
extern "C" {
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
}

namespace glue {
class InputSequencer;
class InputObserver;
class WindowObserver;
}

namespace glue {

class InputSequencer : public LazySingleton<InputSequencer> {
 public:
  InputSequencer();
  virtual ~InputSequencer();

  void RegObserver(view_ptr<InputObserver> p);
  void UnregObserver(view_ptr<InputObserver> p);
  void RegWinObserver(view_ptr<WindowObserver> p);
  void UnregWinObserver(view_ptr<WindowObserver> p);
  void Capture();

 protected:
  std::set<view_ptr<InputObserver>> io_listeners;
  std::set<view_ptr<WindowObserver>> win_listeners;
  int HandleAppEvents(void *userdata, SDL_Event *event);
};

/// Interface for Physical input observer (Mouse, Keyboard and Joystick)
class InputObserverI {
 public:
  /// Callback on keyboard key down
  virtual void OnKeyDown(SDL_Keycode sym) = 0;
  /// Callback on keyboard key up
  virtual void OnKeyUp(SDL_Keycode sym) = 0;

  /// Callback on mouse move #1
  /// @param dx position change in pixels
  /// @param dy position change in pixels
  virtual void OnMouseMove(int dx, int dy) = 0;

  /// Callback on mouse move #2, called with #1 but with extra params
  /// @param x absolute position in pixels
  /// @param y absolute position in pixels
  /// @param dx position change in pixels
  /// @param dy position change in pixels
  /// @param left true is left button down
  /// @param right true is right button down
  /// @param middle true is middle button down
  virtual void OnMouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) = 0;

  /// Callback on Wheel Button Down
  /// @param x absolute position in pixels
  /// @param y absolute position in pixels
  virtual void OnMouseWheel(int x, int y) = 0;

  /// Callback on Left Button Down
  /// @param x absolute position in pixels
  /// @param y absolute position in pixels
  virtual void OnMouseLbDown(int x, int y) = 0;

  /// Callback on Left Button Up
  /// @param x absolute position in pixels
  /// @param y absolute position in pixels
  virtual void OnMouseLbUp(int x, int y) = 0;

  /// Callback on Right Button Down
  /// @param x absolute position in pixels
  /// @param y absolute position in pixels
  virtual void OnMouseRbDown(int x, int y) = 0;

  /// Callback on Right Button Up
  /// @param x absolute position in pixels
  /// @param y absolute position in pixels
  virtual void OnMouseRbUp(int x, int y) = 0;

  /// Callback on Middle Button Down
  /// @param x absolute position in pixels
  /// @param y absolute position in pixels
  virtual void OnMouseMbDown(int x, int y) = 0;

  /// Callback on Middle Button Up
  /// @param x absolute position in pixels
  /// @param y absolute position in pixels
  virtual void OnMouseMbUp(int x, int y) = 0;

  /// Callback on keyboard typing
  /// @param text typed text
  virtual void OnTextInput(const char *text) = 0;

  /// Callback on joystick axis event
  /// @param which joystick number
  /// @param axis which axis
  /// @param value
  virtual void OnJoystickAxis(int which, int axis, int value) = 0;

  /// Callback on joystick button Down
  /// @param which joystick number
  /// @param button button number
  virtual void OnJoystickBtDown(int which, int button) = 0;

  /// Callback on joystick button Up
  /// @param which joystick number
  /// @param button button number
  virtual void OnJoystickBtUp(int which, int button) = 0;
  /// Callback on joystick Hat
  /// @param which joystick number
  /// @param ball hat number
  /// @param value hat value
  virtual void OnJoystickHat(int which, int hat, int value) = 0;
  /// Callback on joystick Ball
  /// @param which joystick number
  /// @param ball ball number
  /// @param xrel ball x value
  /// @param yrel ball y value
  virtual void OnJoystickBall(int which, int ball, int xrel, int yrel) = 0;
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

/// Observer of Windows events
class WindowObserver : public NoCopy {
 public:
  /// Constructor
  WindowObserver();
  virtual ~WindowObserver();

  /// Callback called on any windows event
  /// @param event SDL_Event structure
  virtual void Event(const SDL_Event &event) {}
  /// Callback called on Quit
  virtual void Quit() {}
  /// Callback called when Window is not in focus
  virtual void Pause() {}
  /// Callback called when Window back into focus
  virtual void Resume() {}
  /// Not used yet
  virtual void Other(uint8_t type, int code, void *data1, void *data2) {}
};

}  // namespace glue
