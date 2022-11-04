/// created by Andrey Vasiliev

#pragma once
#include "Singleton.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <algorithm>
#include <vector>

namespace Glue {
class KeyboardListener {
 public:
  /// Callback on keyboard key down
  virtual void OnKeyDown(SDL_Keycode sym) {}
  /// Callback on keyboard key up
  virtual void OnKeyUp(SDL_Keycode sym) {}
  /// Callback on keyboard typing
  /// @param text typed text
  virtual void OnTextInput(const char *text) {}
};

/// Interface for Physical input listener (Mouse, Keyboard and Gamepad)
class MouseListener {
 public:
  /// Callback on mouse move #1
  /// @param dx position change in pixels
  /// @param dy position change in pixels
  virtual void OnMouseMove(int dx, int dy) {}
  /// Callback on mouse move #2, called with #1 but with extra params
  /// @param x absolute position in pixels
  /// @param y absolute position in pixels
  /// @param dx position change in pixels
  /// @param dy position change in pixels
  /// @param left true is left button down
  /// @param right true is right button down
  /// @param middle true is middle button down
  virtual void OnMouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) {}
  /// Callback on Wheel Button Down
  /// @param x absolute position in pixels
  /// @param y absolute position in pixels
  virtual void OnMouseWheel(int x, int y) {}
  /// Callback on Left Button Down
  /// @param x absolute position in pixels
  /// @param y absolute position in pixels
  virtual void OnMouseLbDown(int x, int y) {}
  /// Callback on Left Button Up
  /// @param x absolute position in pixels
  /// @param y absolute position in pixels
  virtual void OnMouseLbUp(int x, int y) {}
  /// Callback on Right Button Down
  /// @param x absolute position in pixels
  /// @param y absolute position in pixels
  virtual void OnMouseRbDown(int x, int y) {}
  /// Callback on Right Button Up
  /// @param x absolute position in pixels
  /// @param y absolute position in pixels
  virtual void OnMouseRbUp(int x, int y) {}
  /// Callback on Middle Button Down
  /// @param x absolute position in pixels
  /// @param y absolute position in pixels
  virtual void OnMouseMbDown(int x, int y) {}
  /// Callback on Middle Button Up
  /// @param x absolute position in pixels
  /// @param y absolute position in pixels
  virtual void OnMouseMbUp(int x, int y) {}
};

/// Interface for Physical input listener (Mouse, Keyboard and Gamepad)
class ControllerListener {
 public:
  /// Callback on joystick axis event
  /// @param which joystick number
  /// @param axis which axis
  /// @param value
  virtual void OnGamepadAxis(int which, int axis, int value) {}
  /// Callback on joystick button Down
  /// @param which joystick number
  /// @param button button number
  virtual void OnGamepadBtDown(int which, int button) {}
  /// Callback on joystick button Up
  /// @param which joystick number
  /// @param button button number
  virtual void OnGamepadBtUp(int which, int button) {}
  /// Callback on joystick Hat
  /// @param which joystick number
  /// @param ball hat number
  /// @param value hat value
  virtual void OnGamepadHat(int which, int hat, int value) {}
  /// Callback on joystick Ball
  /// @param which joystick number
  /// @param ball ball number
  /// @param xrel ball x value
  /// @param yrel ball y value
  virtual void OnGamepadBall(int which, int ball, int xrel, int yrel) {}
};

/// Interface for Window events listener listener
class WindowListener {
 public:
  /// Callback called on any windows event
  /// @param event SDL_Event structure
  virtual void OnEvent(const SDL_Event &event) {}
  /// Callback called on Quit
  virtual void OnQuit() {}
  /// Callback called when Window is not in focus
  virtual void OnFocusLost() {}
  /// Callback called when Window back into focus
  virtual void OnFocusGained() {}
  ///
  virtual void OnSizeChanged(int x, int y, uint32_t id) {}
  ///
  virtual void OnExposed() {}
};

/// This Singleton class is main part of Observer implementation
/// Keeps listeners list, capture inputs and send messages every frame
class InputSequencer final : public LazySingleton<InputSequencer> {
 public:
  InputSequencer() {
    winListeners.reserve(50);
    keyboardListeners.reserve(50);
    mouseListeners.reserve(50);
    contListeners.reserve(50);
  }
  virtual ~InputSequencer() {}

  ///
  void UnregAll() noexcept {
    UnregAllControllerListeners();
    UnregAllMouseListeners();
    UnregAllKeyboardListeners();
    UnregAlWindowListeners();
  }

  ///
  void UnregAlWindowListeners() noexcept {
    winListeners.clear();
    winListeners.reserve(50);
  }

  ///
  void UnregAllKeyboardListeners() noexcept {
    keyboardListeners.clear();
    keyboardListeners.reserve(50);
  }

  ///
  void UnregAllMouseListeners() noexcept {
    mouseListeners.clear();
    mouseListeners.reserve(50);
  }

  ///
  void UnregAllControllerListeners() noexcept {
    contListeners.clear();
    contListeners.reserve(50);
  }

  ///
  void RegKeyboardListener(KeyboardListener *p) noexcept {
    if (std::find(keyboardListeners.begin(), keyboardListeners.end(), p) == keyboardListeners.end()) {
      keyboardListeners.push_back(p);
    }
  }

  void UnregKeyboardListener(KeyboardListener *p) noexcept {
    auto it = std::find(keyboardListeners.begin(), keyboardListeners.end(), p), end = keyboardListeners.end();
    if (it != end) {
      std::swap(it, --end);
      keyboardListeners.pop_back();
    }
  }

  /// Register mouse listener
  void RegMouseListener(MouseListener *p) noexcept {
    if (std::find(mouseListeners.begin(), mouseListeners.end(), p) == mouseListeners.end()) {
      mouseListeners.push_back(p);
    }
  }

  void UnregMouseListener(MouseListener *p) noexcept {
    auto it = std::find(mouseListeners.begin(), mouseListeners.end(), p), end = mouseListeners.end();
    if (it != end) {
      std::swap(it, --end);
      mouseListeners.pop_back();
    }
  }

  /// Register controller listener
  void RegContListener(ControllerListener *p) noexcept {
    if (std::find(contListeners.begin(), contListeners.end(), p) == contListeners.end()) {
      contListeners.push_back(p);
    }
  }

  void UnregContListener(ControllerListener *p) noexcept {
    auto it = std::find(contListeners.begin(), contListeners.end(), p), end = contListeners.end();
    if (it != end) {
      std::swap(it, --end);
      contListeners.pop_back();
    }
  }

  /// Register SDLWindowPtr input listener
  void RegWindowListener(WindowListener *p) noexcept {
#ifdef __ANDROID__
    if (!SDL_GetEventFilter(nullptr, nullptr)) {
      auto callback = [](void *userdata, SDL_Event *event) { return InputSequencer::GetInstance().HandleAppEvents(userdata, event); };
      SDL_SetEventFilter(callback, nullptr);
    }
#endif

    if (std::find(winListeners.begin(), winListeners.end(), p) == winListeners.end()) {
      winListeners.push_back(p);
    }
  }

  void UnregWindowListener(WindowListener *p) noexcept {
    auto it = std::find(winListeners.begin(), winListeners.end(), p), end = winListeners.end();
    if (it != end) {
      std::swap(it, --end);
      winListeners.pop_back();
    }
  }

  /// Called once per frame, sent callback message to listeners
  inline void Capture() noexcept {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_KEYUP: {
          for (auto &it : keyboardListeners) it->OnKeyUp(event.key.keysym.sym);
          break;
        }

        case SDL_KEYDOWN: {
          for (auto &it : keyboardListeners) it->OnKeyDown(event.key.keysym.sym);
          break;
        }

        case SDL_MOUSEMOTION: {
          for (auto it : mouseListeners) {
            it->OnMouseMove(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel,
                            (event.motion.state & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0, (event.motion.state & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0,
                            (event.motion.state & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0);
            it->OnMouseMove(event.motion.xrel, event.motion.yrel);
          }
          break;
        }

        case SDL_MOUSEBUTTONDOWN: {
          switch (event.button.button) {
            case SDL_BUTTON_LEFT: {
              for (auto &it : mouseListeners) it->OnMouseLbDown(event.button.x, event.button.y);
              break;
            }
            case SDL_BUTTON_RIGHT: {
              for (auto &it : mouseListeners) it->OnMouseRbDown(event.button.x, event.button.y);
              break;
            }
            case SDL_BUTTON_MIDDLE: {
              for (auto &it : mouseListeners) it->OnMouseMbDown(event.button.x, event.button.y);
              break;
            }
          }
          break;
        }

        case SDL_MOUSEBUTTONUP: {
          switch (event.button.button) {
            case SDL_BUTTON_LEFT: {
              for (auto &it : mouseListeners) it->OnMouseLbUp(event.button.x, event.button.y);
              break;
            }
            case SDL_BUTTON_RIGHT: {
              for (auto &it : mouseListeners) it->OnMouseRbUp(event.button.x, event.button.y);
              break;
            }
            case SDL_BUTTON_MIDDLE: {
              for (auto &it : mouseListeners) it->OnMouseMbUp(event.button.x, event.button.y);
              break;
            }
          }
          break;
        }

        case SDL_MOUSEWHEEL: {
          for (auto &it : mouseListeners) it->OnMouseWheel(event.wheel.x, event.wheel.y);
          break;
        }

        case SDL_JOYAXISMOTION: {
          for (auto &it : contListeners) it->OnGamepadAxis(event.jaxis.which, event.jaxis.axis, event.jaxis.value);
          break;
        }

        case SDL_JOYBALLMOTION: {
          for (auto &it : contListeners) it->OnGamepadBall(event.jball.which, event.jball.ball, event.jball.xrel, event.jball.yrel);
          break;
        }

        case SDL_JOYHATMOTION: {
          for (auto &it : contListeners) it->OnGamepadHat(event.jhat.which, event.jhat.hat, event.jhat.value);
          break;
        }

        case SDL_JOYBUTTONDOWN: {
          for (auto &it : contListeners) it->OnGamepadBtDown(event.jbutton.which, event.jbutton.button);
          break;
        }

        case SDL_JOYBUTTONUP: {
          for (auto &it : contListeners) it->OnGamepadBtUp(event.jbutton.which, event.jbutton.button);
          break;
        }

        case SDL_QUIT: {
          for (auto &it : winListeners) it->OnQuit();
          break;
        }
#ifndef __ANDROID__
        case SDL_TEXTINPUT: {
          for (auto &it : keyboardListeners) it->OnTextInput(event.text.text);
          break;
        }

        case SDL_WINDOWEVENT: {
          switch (event.window.event) {
            case SDL_WINDOWEVENT_FOCUS_LOST: {
              for (auto &it : winListeners) it->OnFocusLost();
              break;
            }

            case SDL_WINDOWEVENT_FOCUS_GAINED: {
              for (auto &it : winListeners) it->OnFocusGained();
              break;
            }

            case SDL_WINDOWEVENT_SIZE_CHANGED: {
              for (auto &it : winListeners) it->OnSizeChanged(event.window.data1, event.window.data2, event.window.windowID);
              break;
            }

            case SDL_WINDOWEVENT_EXPOSED: {
              for (auto &it : winListeners) it->OnExposed();
              break;
            }
          }

          break;
        }
#endif

        default: {
          for (auto &it : winListeners) it->OnEvent(event);
        }
      }
    }
  }

 protected:
  /// Keyboard listener list
  std::vector<KeyboardListener *> keyboardListeners;

  /// Listeners list (physical input)
  std::vector<MouseListener *> mouseListeners;

  /// Listeners list (physical input)
  std::vector<ControllerListener *> contListeners;

  /// Listeners list (SDLWindowPtr input)
  std::vector<WindowListener *> winListeners;

  /// Required for Android/IOS
#ifdef __ANDROID__
  int HandleAppEvents(void *userdata, SDL_Event *event) {
    switch (event->type) {
      case SDL_APP_WILLENTERBACKGROUND:
      case SDL_APP_DIDENTERBACKGROUND: {
        for (auto &it : winListeners) it->OnFocusLost();
        return 0;
      }

      case SDL_APP_WILLENTERFOREGROUND:
      case SDL_APP_DIDENTERFOREGROUND: {
        for (auto &it : winListeners) it->OnFocusGained();
        return 0;
      }

      default:
        return 1;
    }
  }
#endif
};
}  // namespace Glue
