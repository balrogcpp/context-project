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

#pragma once

#include "IoListeners.h"

#include <vector>

namespace io {
//----------------------------------------------------------------------------------------------------------------------
class InputSequencer {
 public:
  using KeyboardListenersList = std::vector<KeyboardListener *>;
  using MouseListenersList = std::vector<MouseListener *>;
  using JoyListenersList = std::vector<JoyListener *>;
  using OtherListenersList = std::vector<OtherEventListener *>;

  InputSequencer(const InputSequencer &) = delete;
  InputSequencer &operator=(const InputSequencer &) = delete;
  InputSequencer(InputSequencer &&) = delete;
  InputSequencer &operator=(InputSequencer &&) = delete;

  static InputSequencer &GetSingleton() {
    static InputSequencer InputSingleton;
    return InputSingleton;
  }

  InputSequencer() {
    kb_listeners_.reserve(reserve_);
    ms_listeners_.reserve(reserve_);
    joy_listeners_.reserve(reserve_);
    other_listeners_.reserve(reserve_);
  }

 private:
  KeyboardListenersList kb_listeners_;
  MouseListenersList ms_listeners_;
  JoyListenersList joy_listeners_;
  OtherListenersList other_listeners_;
  const size_t reserve_ = 16;

 public:
  void RegKbListener(KeyboardListener *p) {
    kb_listeners_.push_back(p);
  }
//----------------------------------------------------------------------------------------------------------------------
  void UnregKbListener(KeyboardListener *p) {
    auto it = find(kb_listeners_.begin(), kb_listeners_.end(), p);
    if (it != kb_listeners_.end()) {
      iter_swap(it, prev(kb_listeners_.end()));
      kb_listeners_.pop_back();
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  void RegMsListener(MouseListener *p) {
    ms_listeners_.push_back(p);
  }
//----------------------------------------------------------------------------------------------------------------------
  void UnregMsListener(MouseListener *p) {
    auto it = find(ms_listeners_.begin(), ms_listeners_.end(), p);
    if (it != ms_listeners_.end()) {
      iter_swap(it, prev(ms_listeners_.end()));
      ms_listeners_.pop_back();
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  void RegJoyListener(JoyListener *p) {
    joy_listeners_.push_back(p);
  }
//----------------------------------------------------------------------------------------------------------------------
  void UnregJoyListener(JoyListener *p) {
    auto it = find(joy_listeners_.begin(), joy_listeners_.end(), p);
    if (it != joy_listeners_.end()) {
      iter_swap(it, prev(joy_listeners_.end()));
      joy_listeners_.pop_back();
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  void RegEventListener(OtherEventListener *p) {
    other_listeners_.push_back(p);
  }
//----------------------------------------------------------------------------------------------------------------------
  void UnregEventListener(OtherEventListener *p) {
    auto it = find(other_listeners_.begin(), other_listeners_.end(), p);
    if (it != other_listeners_.end()) {
      iter_swap(it, prev(other_listeners_.end()));
      other_listeners_.pop_back();
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  void RegisterListener(InputListener *p) {
    RegKbListener(p);
    RegMsListener(p);
    RegJoyListener(p);
    RegEventListener(p);
  }
//----------------------------------------------------------------------------------------------------------------------
  void UnregisterListener(InputListener *p) {
    UnregKbListener(p);
    UnregMsListener(p);
    UnregJoyListener(p);
    UnregEventListener(p);
  }
//----------------------------------------------------------------------------------------------------------------------
  void Reset() {
    kb_listeners_.clear();
    ms_listeners_.clear();
    joy_listeners_.clear();
    other_listeners_.clear();
  }
//----------------------------------------------------------------------------------------------------------------------
  void Capture() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {

      switch (event.type) {
        case SDL_KEYUP: {
          for (auto it : kb_listeners_) {
            it->KeyUp(event.key.keysym.sym);
          }
          break;
        }

        case SDL_KEYDOWN: {
          for (auto it : kb_listeners_) {
            it->KeyDown(event.key.keysym.sym);
          }
          break;
        }

        case SDL_MOUSEMOTION: {
          for (auto it : ms_listeners_) {
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
              for (auto it : ms_listeners_) {
                it->LbDown(event.button.x, event.button.y);
              }
              break;
            }
            case SDL_BUTTON_RIGHT: {
              for (auto it : ms_listeners_) {
                it->RbDown(event.button.x, event.button.y);
              }

              break;
            }
            case SDL_BUTTON_MIDDLE: {
              for (auto it : ms_listeners_) {
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
              for (auto it : ms_listeners_) {
                it->LbUp(event.button.x, event.button.y);
              }
              break;
            }
            case SDL_BUTTON_RIGHT: {
              for (auto it : ms_listeners_) {
                it->RbUp(event.button.x, event.button.y);
              }
              break;
            }
            case SDL_BUTTON_MIDDLE: {
              for (auto it : ms_listeners_) {
                it->MbUp(event.button.x, event.button.y);
              }
              break;
            }
          }
          break;
        }

        case SDL_MOUSEWHEEL: {
          for (auto it : ms_listeners_) {
            it->Wheel(event.wheel.x, event.wheel.y);
          }

          break;
        }

        case SDL_JOYAXISMOTION: {
          for (auto it : joy_listeners_) {
            it->Axis(event.jaxis.which, event.jaxis.axis, event.jaxis.value);
          }

          break;
        }

        case SDL_JOYBALLMOTION: {
          for (auto it : joy_listeners_) {
            it->Ball(event.jball.which, event.jball.ball, event.jball.xrel, event.jball.yrel);
          }
          break;
        }

        case SDL_JOYHATMOTION: {
          for (auto it : joy_listeners_) {
            it->Hat(event.jhat.which, event.jhat.hat, event.jhat.value);
          }
          break;
        }
        case SDL_JOYBUTTONDOWN: {
          for (auto it : joy_listeners_) {
            it->BtDown(event.jbutton.which, event.jbutton.button);
          }
          break;
        }

        case SDL_JOYBUTTONUP: {
          for (auto it : joy_listeners_) {
            if (it) {
              it->BtUp(event.jbutton.which,
                       event.jbutton.button);
            }
          }
          break;
        }

        case SDL_QUIT: {
          for (auto it : other_listeners_) {
            it->Quit();
          }
          break;
        }

        default: {
          for (auto it : other_listeners_) {
            it->Event(event);
          }
        }
      }
    }
  }
}; //class InputSequencer
} //namespace io
