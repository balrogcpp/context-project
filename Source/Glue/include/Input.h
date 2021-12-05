// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Exception.h"
#include "NoCopy.h"
#include "Singleton.h"
#include <exception>
#include <set>
#include <string>
extern "C" {
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
}

namespace Glue {
class InputSequencer;
class InputObserverI;
class WindowObserverI;
class InputObserver;
class WindowObserver;
}  // namespace Glue

namespace Glue {

/// This Singleton class is main part of Observer implementation
/// Keeps listeners list, capture inputs and send messages every frame
class InputSequencer : public LazySingleton<InputSequencer> {
 public:
  InputSequencer();
  virtual ~InputSequencer();

  /// Register physical input listener
  void RegObserver(InputObserverI *p);

  /// Un-Register physical input listener
  void UnregObserver(InputObserverI *p);

  /// Register window input listener
  void RegWinObserver(WindowObserverI *p);

  /// Un-Register window input listener
  void UnregWinObserver(WindowObserverI *p);

  /// Called once per frame, sent callback message to listeners
  void Capture();

 protected:
  /// Listeners list (physical input)
  std::set<InputObserverI *> io_listeners;
  /// Listeners list (window input)
  std::set<WindowObserverI *> win_listeners;
  /// Required for Android/IOS development
  int HandleAppEvents(void *userdata, SDL_Event *event);
};

/// Interface for Physical input listener (Mouse, Keyboard and Joystick)
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

/// Physical input events listener
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

/// Interface for window listener
class WindowObserverI {
 public:
  /// Callback called on any windows event
  /// @param event SDL_Event structure
  virtual void Event(const SDL_Event &event) = 0;
  /// Callback called on Quit
  virtual void Quit() = 0;
  /// Callback called when Window is not in focus
  virtual void Pause() = 0;
  /// Callback called when Window back into focus
  virtual void Resume() = 0;
};

/// Window events listener
class WindowObserver : public WindowObserverI, public NoCopy {
 public:
  /// Constructor
  WindowObserver();
  virtual ~WindowObserver();

  virtual void Event(const SDL_Event &event) {}
  virtual void Quit() {}
  virtual void Pause() {}
  virtual void Resume() {}
};

}  // namespace Glue
