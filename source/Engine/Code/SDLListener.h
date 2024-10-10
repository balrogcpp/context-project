/// created by Andrey Vasiliev

#pragma once
#include "Singleton.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <algorithm>
#include <vector>

namespace gge {

class DeviceListener {
 public:
  /// Callback on keyboard key down
  virtual void OnKeyEvent(SDL_Scancode key, bool pressed) {}
  /// Callback on keyboard typing
  /// @param text typed text
  virtual void OnTextInput(const char *text) {}
  /// Callback on mouse move #1
  /// @param dx position change in pixels
  /// @param dy position change in pixels
  virtual void OnMouseMotion(int dx, int dy) {}
  /// Callback on Wheel Button Down
  /// @param x absolute position in pixels
  /// @param y absolute position in pixels
  virtual void OnMouseWheel(int x, int y) {}
  /// Callback on Left Button Down
  /// @param x absolute position in pixels
  /// @param y absolute position in pixels
  virtual void OnMouseButton(int button, bool pressed) {}
  /// Callback on joystick axis event
  /// @param which joystick number
  /// @param axis which axis
  /// @param value
  virtual void OnGamepadAxis(int which, int axis, int value) {}
  /// Callback on joystick button Down
  /// @param which joystick number
  /// @param button button number
  virtual void OnGamepadButton(int which, int button, bool pressed) {}
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
};

/// This Singleton class is main part of Observer implementation
/// Keeps listeners list, capture inputs and send messages every frame
class InputSequencer final : public LazySingleton<InputSequencer> {
 public:
  InputSequencer() {}
  virtual ~InputSequencer() {}

  ///
  inline void UnregAll() noexcept {
    UnregAllDeviceListeners();
    UnregAlWindowListeners();
  }

  ///
  inline void UnregAlWindowListeners() noexcept {
    winListeners.clear();
    winListeners.shrink_to_fit();
  }

  ///
  inline void UnregAllDeviceListeners() noexcept {
    devListeners.clear();
    devListeners.shrink_to_fit();
  }

  ///
  inline void RegDeviceListener(DeviceListener *p) noexcept {
    if (std::find(devListeners.begin(), devListeners.end(), p) == devListeners.end()) {
      devListeners.push_back(p);
    }
  }

  inline void UnregDeviceListener(DeviceListener *p) noexcept {
    auto it = std::find(devListeners.begin(), devListeners.end(), p), end = devListeners.end();
    if (it != end) {
      std::swap(it, --end);
      devListeners.pop_back();
    }
  }

  /// Register SDLWindowPtr input listener
  inline void RegWindowListener(WindowListener *p) noexcept {
#if defined(__ANDROID__)
    if (!SDL_GetEventFilter(nullptr, nullptr)) {
      auto callback = [](void *userdata, SDL_Event *event) { return InputSequencer::GetInstance().HandleAppEvents(userdata, event); };
      SDL_SetEventFilter(callback, nullptr);
    }
#endif

    if (std::find(winListeners.begin(), winListeners.end(), p) == winListeners.end()) {
      winListeners.push_back(p);
    }
  }

  inline void UnregWindowListener(WindowListener *p) noexcept {
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
      for (auto &it : winListeners) it->OnEvent(event);

      switch (event.type) {
        case SDL_KEYUP:
        case SDL_KEYDOWN: {
          for (auto &it : devListeners) it->OnKeyEvent(event.key.keysym.scancode, event.type == SDL_KEYDOWN);
          break;
        }

        case SDL_TEXTINPUT: {
          for (auto &it : devListeners) it->OnTextInput(event.text.text);
          break;
        }

        case SDL_MOUSEMOTION: {
          for (auto it : devListeners) it->OnMouseMotion(event.motion.xrel, event.motion.yrel);
          break;
        }

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP: {
          int mouse_button = -1;
          switch (event.button.button) {
            case SDL_BUTTON_LEFT: {
              mouse_button = 0;
              break;
            }
            case SDL_BUTTON_RIGHT: {
              mouse_button = 1;
              break;
            }
            case SDL_BUTTON_MIDDLE: {
              mouse_button = 2;
              break;
            }
            default: {
              break;
            }
          }
          for (auto it : devListeners) it->OnMouseButton(mouse_button, event.type == SDL_MOUSEBUTTONDOWN);
          break;
        }

        case SDL_MOUSEWHEEL: {
          int wheel_x = (event.wheel.x > 0) ? 1.0f : (event.wheel.x < 0) ? -1.0f : 0.0f;
          int wheel_y = (event.wheel.y > 0) ? 1.0f : (event.wheel.y < 0) ? -1.0f : 0.0f;
          for (auto &it : devListeners) it->OnMouseWheel(wheel_x, wheel_y);
          break;
        }

        case SDL_CONTROLLERAXISMOTION: {
          for (auto &it : devListeners) it->OnGamepadAxis(event.caxis.which, event.caxis.axis, event.caxis.value);
          break;
        }

        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERBUTTONUP: {
          for (auto &it : devListeners) it->OnGamepadButton(event.cbutton.which, event.cbutton.button, event.type == SDL_CONTROLLERBUTTONDOWN);
          break;
        }

        case SDL_QUIT: {
          for (auto &it : winListeners) it->OnQuit();
          break;
        }
#ifndef __ANDROID__
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
          }

          break;
        }
#endif

        default: {
          break;
        }
      }
    }
  }

 protected:
  /// Device listener list
  std::vector<DeviceListener *> devListeners;

  /// Window event listeners
  std::vector<WindowListener *> winListeners;

  /// Required for Android/IOS
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
};

}  // namespace gge
