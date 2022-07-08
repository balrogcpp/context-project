// This source file is part of Glue Engine. Created by Andrey Vasiliev

#include "PCHeader.h"
#include "Input/InputSequencer.h"
#include "Input/InputObserver.h"
#include "Input/WindowObserver.h"
#include <algorithm>

using namespace std;

namespace Glue {

int InputSequencer::HandleAppEvents(void *userdata, SDL_Event *event) {
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

InputSequencer::InputSequencer() {}

InputSequencer::~InputSequencer() {}

void InputSequencer::RegObserver(InputObserver *p) { io_listeners.insert(p); }

void InputSequencer::UnRegObserver(InputObserver *p) { io_listeners.erase(p); }

void InputSequencer::RegWinObserver(WindowObserver *p) {
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  auto callback = [](void *userdata, SDL_Event *event) { return InputSequencer::GetInstance().HandleAppEvents(userdata, event); };

  SDL_SetEventFilter(callback, nullptr);
#endif

  win_listeners.insert(p);
}

void InputSequencer::UnregWinObserver(WindowObserver *p) { win_listeners.erase(p); }

void InputSequencer::Capture() {
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

}  // namespace Glue
