//Cpp file for dummy context2_deps target
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

#include "pcheader.hpp"

#include "Io.hpp"

#include <string>
#include <iostream>

using namespace std;

namespace Context {

//----------------------------------------------------------------------------------------------------------------------
void InputSequencer::RegKbListener(KeyboardListener *l) {
  if (l && find(kb_listeners.begin(), kb_listeners.end(), l) == kb_listeners.end()) {
    kb_listeners.push_back(l);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void InputSequencer::UnregKbListener(KeyboardListener *l) {
  if (!kb_listeners.empty()) {
    KeyboardListenersList::iterator it = find(kb_listeners.begin(), kb_listeners.end(), l);
    if (l && it != kb_listeners.end()) {
      std::iter_swap(it, std::prev(kb_listeners.end()));
      kb_listeners.pop_back();
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void InputSequencer::RegMsListener(MouseListener *l) {
  if (l && find(ms_listeners.begin(), ms_listeners.end(), l) == ms_listeners.end()) {
    ms_listeners.push_back(l);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void InputSequencer::UnregMsListener(MouseListener *l) {
  if (!ms_listeners.empty()) {
    MouseListenersList::iterator it = find(ms_listeners.begin(), ms_listeners.end(), l);
    if (l && it != ms_listeners.end()) {
      std::iter_swap(it, std::prev(ms_listeners.end()));
      ms_listeners.pop_back();
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void InputSequencer::RegJoyListener(JoyListener *l) {
  if (l && find(joy_listeners.begin(), joy_listeners.end(), l) == joy_listeners.end()) {
    joy_listeners.push_back(l);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void InputSequencer::UnregJoyListener(JoyListener *l) {
  if (!joy_listeners.empty()) {
    JoyListenersList ::iterator it = find(joy_listeners.begin(), joy_listeners.end(), l);
    if (l && it != joy_listeners.end()) {
      std::iter_swap(it, std::prev(joy_listeners.end()));
      joy_listeners.pop_back();
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void InputSequencer::RegEventListener(OtherEventListener *l) {
  if (l && find(other_listeners.begin(), other_listeners.end(), l) == other_listeners.end()) {
    other_listeners.push_back(l);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void InputSequencer::UnregEventListener(OtherEventListener *l) {
  if (!other_listeners.empty()) {
    OtherListenersList::iterator it = find(other_listeners.begin(), other_listeners.end(), l);
    if (l && it != other_listeners.end()) {
      std::iter_swap(it, std::prev(other_listeners.end()));
      other_listeners.pop_back();
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void InputSequencer::Capture() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {

    switch (event.type) {
      case SDL_KEYUP: {
        for (auto it : kb_listeners) {
          it->KeyUp(event.key.keysym.sym);
        }
        break;
      }

      case SDL_KEYDOWN: {
        for (auto it : kb_listeners) {
          it->KeyDown(event.key.keysym.sym);
        }
        break;
      }

      case SDL_MOUSEMOTION: {
        for (auto it : ms_listeners) {
          it->Move(event.motion.x, event.motion.y,
                   event.motion.xrel, event.motion.yrel,
                   (event.motion.state & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0,
                   (event.motion.state & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0,
                   (event.motion.state & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0);
        }
        break;
      }

      case SDL_MOUSEBUTTONDOWN: {
        switch (event.button.button) {
          case SDL_BUTTON_LEFT: {
            for (auto it : ms_listeners) {
              it->LbDown(event.button.x, event.button.y);
            }
            break;
          }
          case SDL_BUTTON_RIGHT: {
            for (auto it : ms_listeners) {
              it->RbDown(event.button.x, event.button.y);
            }

            break;
          }
          case SDL_BUTTON_MIDDLE: {
            for (auto it : ms_listeners) {
              it->MbDown(event.button.x, event.button.y);
            }
            break;
          }
        }
        break;
      }

      case SDL_MOUSEBUTTONUP: {
        switch (event.button.button) {
          case SDL_BUTTON_LEFT: {
            for (auto it : ms_listeners) {
              it->LbUp(event.button.x, event.button.y);
            }
            break;
          }
          case SDL_BUTTON_RIGHT: {
            for (auto it : ms_listeners) {
              it->RbUp(event.button.x, event.button.y);
            }
            break;
          }
          case SDL_BUTTON_MIDDLE: {
            for (auto it : ms_listeners) {
              it->MbUp(event.button.x, event.button.y);
            }
            break;
          }
        }
        break;
      }

      case SDL_MOUSEWHEEL: {
        for (auto it : ms_listeners) {
          it->Wheel(event.wheel.x, event.wheel.y);
        }

        break;
      }

      case SDL_JOYAXISMOTION: {
        for (auto it : joy_listeners) {
          it->Axis(event.jaxis.which, event.jaxis.axis, event.jaxis.value);
        }

        break;
      }

      case SDL_JOYBALLMOTION: {
        for (auto it : joy_listeners) {
          it->Ball(event.jball.which, event.jball.ball, event.jball.xrel, event.jball.yrel);
        }
        break;
      }

      case SDL_JOYHATMOTION: {
        for (auto it : joy_listeners) {
          it->Hat(event.jhat.which, event.jhat.hat, event.jhat.value);
        }
        break;
      }
      case SDL_JOYBUTTONDOWN: {
        for (auto it : joy_listeners) {
          it->BtDown(event.jbutton.which, event.jbutton.button);
        }
        break;
      }

      case SDL_JOYBUTTONUP: {
        for (auto it : joy_listeners) {
          if (it) {
            it->BtUp(event.jbutton.which,
                     event.jbutton.button);
          }
        }
        break;
      }

      case SDL_QUIT: {
        for (auto it : other_listeners) {
          it->Quit();
        }
        break;
      }

      default: {
        for (auto it : other_listeners) {
          it->Event(event);
        }
      }
    }
  }
}

} //namespace Context
