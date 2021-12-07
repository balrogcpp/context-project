// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "LazySingleton.h"
#include "Exception.h"
#include "NoCopy.h"
#include <exception>
#include <set>
#include <string>
extern "C" {
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
}

namespace Glue {
class InputSequencer;
class IInputObserver;
class IWindowObserver;
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
  void RegObserver(IInputObserver *p);

  /// Un-Register physical input listener
  void UnregObserver(IInputObserver *p);

  /// Register SDLWindowPtr input listener
  void RegWinObserver(IWindowObserver *p);

  /// Un-Register SDLWindowPtr input listener
  void UnregWinObserver(IWindowObserver *p);

  /// Called once per frame, sent callback message to listeners
  void Capture();

 protected:
  /// Listeners list (physical input)
  std::set<IInputObserver *> io_listeners;
  /// Listeners list (SDLWindowPtr input)
  std::set<IWindowObserver *> win_listeners;
  /// Required for Android/IOS development
  int HandleAppEvents(void *userdata, SDL_Event *event);
};

/// Interface for Physical input listener (Mouse, Keyboard and Gamepad)
class IInputObserver {
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
  virtual void OnGamepadAxis(int which, int axis, int value) = 0;

  /// Callback on joystick button Down
  /// @param which joystick number
  /// @param button button number
  virtual void OnGamepadBtDown(int which, int button) = 0;

  /// Callback on joystick button Up
  /// @param which joystick number
  /// @param button button number
  virtual void OnGamepadBtUp(int which, int button) = 0;
  /// Callback on joystick Hat
  /// @param which joystick number
  /// @param ball hat number
  /// @param value hat value
  virtual void OnGamepadHat(int which, int hat, int value) = 0;
  /// Callback on joystick Ball
  /// @param which joystick number
  /// @param ball ball number
  /// @param xrel ball x value
  /// @param yrel ball y value
  virtual void OnGamepadBall(int which, int ball, int xrel, int yrel) = 0;
};

/// Physical input events listener
class InputObserver : public IInputObserver, public NoCopy {
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

  // Gamepad
  void OnGamepadAxis(int which, int axis, int value) override {}
  void OnGamepadBtDown(int which, int button) override {}
  void OnGamepadBtUp(int which, int button) override {}
  void OnGamepadHat(int which, int hat, int value) override {}
  void OnGamepadBall(int which, int ball, int xrel, int yrel) override {}
};

/// Interface for SDLWindowPtr listener
class IWindowObserver {
 public:
  /// Callback called on any windows event
  /// @param event SDL_Event structure
  virtual void OnEvent(const SDL_Event &event) = 0;
  /// Callback called on Quit
  virtual void OnQuit() = 0;
  /// Callback called when Window is not in focus
  virtual void OnPause() = 0;
  /// Callback called when Window back into focus
  virtual void OnResume() = 0;
};

/// Window events listener
class WindowObserver : public IWindowObserver, public NoCopy {
 public:
  /// Constructor
  WindowObserver();
  virtual ~WindowObserver();

  virtual void OnEvent(const SDL_Event &event) {}
  virtual void OnQuit() {}
  virtual void OnPause() {}
  virtual void OnResume() {}
};

}  // namespace Glue
