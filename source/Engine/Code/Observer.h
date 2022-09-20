/// created by Andrey Vasiliev

#pragma once
#include "Singleton.h"
extern "C" {
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
}
#include <algorithm>
#include <set>

namespace Glue {

/// Interface for Physical input listener (Mouse, Keyboard and Gamepad)
class InputObserver {
 public:
  /// Callback on keyboard key down
  virtual void OnKeyDown(SDL_Keycode sym) {}
  /// Callback on keyboard key up
  virtual void OnKeyUp(SDL_Keycode sym) {}

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

  /// Callback on keyboard typing
  /// @param text typed text
  virtual void OnTextInput(const char *text) {}

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
class WindowObserver {
 public:
  /// Callback called on any windows event
  /// @param event SDL_Event structure
  virtual void OnEvent(const SDL_Event &event) {}
  /// Callback called on Quit
  virtual void OnQuit() {}
  /// Callback called when Window is not in focus
  virtual void OnPause() {}
  /// Callback called when Window back into focus
  virtual void OnResume() {}
};

/// This Singleton class is main part of Observer implementation
/// Keeps listeners list, capture inputs and send messages every frame
class InputSequencer final : public LazySingleton<InputSequencer> {
 public:
  /// Register physical input listener
  void RegObserver(InputObserver *p) { io_listeners.insert(p); }

  /// Un-Register physical input listener
  void UnRegObserver(InputObserver *p) { io_listeners.erase(p); }

  /// Register SDLWindowPtr input listener
  void RegWinObserver(WindowObserver *p) {
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
    auto callback = [](void *userdata, SDL_Event *event) { return InputSequencer::GetInstance().HandleAppEvents(userdata, event); };

    SDL_SetEventFilter(callback, nullptr);
#endif

    win_listeners.insert(p);
  }

  /// Un-Register SDLWindowPtr input listener
  void UnregWinObserver(WindowObserver *p) { win_listeners.erase(p); }

  /// Called once per frame, sent callback message to listeners
  void Capture() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_KEYUP: {
          for (auto &it : io_listeners) it->OnKeyUp(event.key.keysym.sym);
          break;
        }

        case SDL_KEYDOWN: {
          for (auto &it : io_listeners) it->OnKeyDown(event.key.keysym.sym);
          break;
        }

        case SDL_MOUSEMOTION: {
          for (auto it : io_listeners) {
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
              for (auto &it : io_listeners) it->OnMouseLbDown(event.button.x, event.button.y);
              break;
            }
            case SDL_BUTTON_RIGHT: {
              for (auto &it : io_listeners) it->OnMouseRbDown(event.button.x, event.button.y);
              break;
            }
            case SDL_BUTTON_MIDDLE: {
              for (auto &it : io_listeners) it->OnMouseMbDown(event.button.x, event.button.y);
              break;
            }
          }
          break;
        }

        case SDL_MOUSEBUTTONUP: {
          switch (event.button.button) {
            case SDL_BUTTON_LEFT: {
              for (auto &it : io_listeners) it->OnMouseLbUp(event.button.x, event.button.y);
              break;
            }
            case SDL_BUTTON_RIGHT: {
              for (auto &it : io_listeners) it->OnMouseRbUp(event.button.x, event.button.y);
              break;
            }
            case SDL_BUTTON_MIDDLE: {
              for (auto &it : io_listeners) it->OnMouseMbUp(event.button.x, event.button.y);
              break;
            }
          }
          break;
        }

        case SDL_MOUSEWHEEL: {
          for (auto &it : io_listeners) it->OnMouseWheel(event.wheel.x, event.wheel.y);
          break;
        }

        case SDL_JOYAXISMOTION: {
          for (auto &it : io_listeners) it->OnGamepadAxis(event.jaxis.which, event.jaxis.axis, event.jaxis.value);
          break;
        }

        case SDL_JOYBALLMOTION: {
          for (auto &it : io_listeners) it->OnGamepadBall(event.jball.which, event.jball.ball, event.jball.xrel, event.jball.yrel);
          break;
        }

        case SDL_JOYHATMOTION: {
          for (auto &it : io_listeners) it->OnGamepadHat(event.jhat.which, event.jhat.hat, event.jhat.value);
          break;
        }

        case SDL_JOYBUTTONDOWN: {
          for (auto &it : io_listeners) it->OnGamepadBtDown(event.jbutton.which, event.jbutton.button);
          break;
        }

        case SDL_JOYBUTTONUP: {
          for (auto &it : io_listeners) it->OnGamepadBtUp(event.jbutton.which, event.jbutton.button);
          break;
        }

        case SDL_QUIT: {
          for (auto &it : win_listeners) it->OnQuit();
          break;
        }
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
        case SDL_TEXTINPUT: {
          for (auto &it : io_listeners) it->OnTextInput(event.text.text);
          break;
        }

        case SDL_WINDOWEVENT: {
          switch (event.window.event) {
            case SDL_WINDOWEVENT_MINIMIZED:
            case SDL_WINDOWEVENT_FOCUS_LOST: {
              for (auto &it : win_listeners) it->OnPause();
              break;
            }

            case SDL_WINDOWEVENT_FOCUS_GAINED: {
              for (auto &it : win_listeners) it->OnResume();
              break;
            }
          }

          break;
        }
#endif

        default: {
          for (auto &it : win_listeners) it->OnEvent(event);
        }
      }
    }
  }

 protected:
  /// Listeners list (physical input)
  std::set<InputObserver *> io_listeners;

  /// Listeners list (SDLWindowPtr input)
  std::set<WindowObserver *> win_listeners;

  /// Required for Android/IOS
  int HandleAppEvents(void *userdata, SDL_Event *event) {
    switch (event->type) {
      case SDL_APP_WILLENTERBACKGROUND:
      case SDL_APP_DIDENTERBACKGROUND: {
        for (auto &it : win_listeners) it->OnPause();
        return 0;
      }

      case SDL_APP_WILLENTERFOREGROUND:
      case SDL_APP_DIDENTERFOREGROUND: {
        for (auto &it : win_listeners) it->OnResume();
        return 0;
      }

      default:
        return 1;
    }
  }
};

}  // namespace Glue
