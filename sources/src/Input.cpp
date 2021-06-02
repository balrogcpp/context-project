// MIT License
//
// Copyright (c) 2021 Andrew Vasiliev
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "Input.h"

#include <algorithm>

using namespace std;

namespace xio {

//----------------------------------------------------------------------------------------------------------------------
int InputSequencer::HandleAppEvents(void *userdata, SDL_Event *event) {
  switch (event->type) {
    case SDL_APP_WILLENTERBACKGROUND:
    case SDL_APP_DIDENTERBACKGROUND: {
      for_each(win_listeners.begin(), win_listeners.end(),
               [](auto it) { it->Pause(); });
      return 0;
    }

    case SDL_APP_WILLENTERFOREGROUND:
    case SDL_APP_DIDENTERFOREGROUND: {
      for_each(win_listeners.begin(), win_listeners.end(),
               [](auto it) { it->Resume(); });
      return 0;
    }

    default:
      return 1;
  }
}

//----------------------------------------------------------------------------------------------------------------------
InputSequencer::InputSequencer() {}

//----------------------------------------------------------------------------------------------------------------------
InputSequencer::~InputSequencer() {}

//----------------------------------------------------------------------------------------------------------------------
void InputSequencer::RegObserver(view_ptr<InputObserver> p) {
  io_listeners.insert(p);
}

//----------------------------------------------------------------------------------------------------------------------
void InputSequencer::UnregObserver(view_ptr<InputObserver> p) {
  io_listeners.erase(p);
}

//----------------------------------------------------------------------------------------------------------------------
void InputSequencer::RegWinObserver(view_ptr<WindowObserver> p) {
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  auto callback = [](void *userdata, SDL_Event *event) {
    return InputSequencer::GetInstance().HandleAppEvents(userdata, event);
  };

  SDL_SetEventFilter(callback, nullptr);
#endif

  win_listeners.insert(p);
}

//----------------------------------------------------------------------------------------------------------------------
void InputSequencer::UnregWinObserver(view_ptr<WindowObserver> p) {
  win_listeners.erase(p);
}

//----------------------------------------------------------------------------------------------------------------------
void InputSequencer::Capture() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_KEYUP: {
        for_each(io_listeners.begin(), io_listeners.end(),
                 [&event](auto it) { it->OnKeyUp(event.key.keysym.sym); });
        break;
      }

      case SDL_KEYDOWN: {
        for_each(io_listeners.begin(), io_listeners.end(),
                 [&event](auto it) { it->OnKeyDown(event.key.keysym.sym); });
        break;
      }

      case SDL_MOUSEMOTION: {
        for (auto it : io_listeners) {
          it->OnMouseMove(
              event.motion.x, event.motion.y, event.motion.xrel,
              event.motion.yrel,
              (event.motion.state & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0,
              (event.motion.state & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0,
              (event.motion.state & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0);
          it->OnMouseMove(event.motion.xrel, event.motion.yrel);
        }
        break;
      }

      case SDL_MOUSEBUTTONDOWN: {
        switch (event.button.button) {
          case SDL_BUTTON_LEFT: {
            for_each(io_listeners.begin(), io_listeners.end(),
                     [&event](auto it) {
                       it->OnMouseLbDown(event.button.x, event.button.y);
                     });
            break;
          }
          case SDL_BUTTON_RIGHT: {
            for_each(io_listeners.begin(), io_listeners.end(),
                     [&event](auto it) {
                       it->OnMouseRbDown(event.button.x, event.button.y);
                     });
            break;
          }
          case SDL_BUTTON_MIDDLE: {
            for_each(io_listeners.begin(), io_listeners.end(),
                     [&event](auto it) {
                       it->OnMouseMbDown(event.button.x, event.button.y);
                     });
            break;
          }
        }
        break;
      }

      case SDL_MOUSEBUTTONUP: {
        switch (event.button.button) {
          case SDL_BUTTON_LEFT: {
            for_each(io_listeners.begin(), io_listeners.end(),
                     [&event](auto it) {
                       it->OnMouseLbUp(event.button.x, event.button.y);
                     });
            break;
          }
          case SDL_BUTTON_RIGHT: {
            for_each(io_listeners.begin(), io_listeners.end(),
                     [&event](auto it) {
                       it->OnMouseRbUp(event.button.x, event.button.y);
                     });
            break;
          }
          case SDL_BUTTON_MIDDLE: {
            for_each(io_listeners.begin(), io_listeners.end(),
                     [&event](auto it) {
                       it->OnMouseMbUp(event.button.x, event.button.y);
                     });
            break;
          }
        }
        break;
      }

      case SDL_MOUSEWHEEL: {
        for_each(io_listeners.begin(), io_listeners.end(), [&event](auto it) {
          it->OnMouseWheel(event.wheel.x, event.wheel.y);
        });
        break;
      }

      case SDL_JOYAXISMOTION: {
        for_each(io_listeners.begin(), io_listeners.end(), [&event](auto it) {
          it->OnJoystickAxis(event.jaxis.which, event.jaxis.axis,
                             event.jaxis.value);
        });
        break;
      }

      case SDL_JOYBALLMOTION: {
        for_each(io_listeners.begin(), io_listeners.end(), [&event](auto it) {
          it->OnJoystickBall(event.jball.which, event.jball.ball,
                             event.jball.xrel, event.jball.yrel);
        });
        break;
      }

      case SDL_JOYHATMOTION: {
        for_each(io_listeners.begin(), io_listeners.end(), [&event](auto it) {
          it->OnJoystickHat(event.jhat.which, event.jhat.hat, event.jhat.value);
        });
        break;
      }

      case SDL_JOYBUTTONDOWN: {
        for_each(io_listeners.begin(), io_listeners.end(), [&event](auto it) {
          it->OnJoystickBtDown(event.jbutton.which, event.jbutton.button);
        });
        break;
      }

      case SDL_JOYBUTTONUP: {
        for_each(io_listeners.begin(), io_listeners.end(), [&event](auto it) {
          it->OnJoystickBtUp(event.jbutton.which, event.jbutton.button);
        });
        break;
      }

      case SDL_QUIT: {
        for_each(win_listeners.begin(), win_listeners.end(),
                 [](auto it) { it->Quit(); });
        break;
      }

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
      case SDL_TEXTINPUT: {
        for_each(io_listeners.begin(), io_listeners.end(),
                 [&event](auto it) { it->OnTextInput(event.text.text); });
        break;
      }

      case SDL_WINDOWEVENT: {
        switch (event.window.event) {
          case SDL_WINDOWEVENT_MINIMIZED:
          case SDL_WINDOWEVENT_FOCUS_LOST: {
            for_each(win_listeners.begin(), win_listeners.end(),
                     [](auto it) { it->Pause(); });
            break;
          }

          case SDL_WINDOWEVENT_FOCUS_GAINED: {
            for_each(win_listeners.begin(), win_listeners.end(),
                     [](auto it) { it->Resume(); });
            break;
          }
        }

        break;
      }
#endif

      default: {
        for_each(win_listeners.begin(), win_listeners.end(),
                 [&event](auto it) { it->Event(event); });
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
InputObserver::InputObserver() {
  static auto &ref = InputSequencer::GetInstance();
  ref.RegObserver(this);
}

//----------------------------------------------------------------------------------------------------------------------
InputObserver::~InputObserver() {
  static auto &ref = InputSequencer::GetInstance();
  ref.UnregObserver(this);
}

//----------------------------------------------------------------------------------------------------------------------
WindowObserver::WindowObserver() {
  static auto &ref = InputSequencer::GetInstance();
  ref.RegWinObserver(this);
}

//----------------------------------------------------------------------------------------------------------------------
WindowObserver::~WindowObserver() {
  static auto &ref = InputSequencer::GetInstance();
  ref.UnregWinObserver(this);
}

}  // namespace xio
