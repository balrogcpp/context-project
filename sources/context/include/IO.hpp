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

extern "C" {
#include <SDL2/SDL.h>
}

#include <vector>
#include <queue>
#include <mutex>

namespace io {

class KeyboardListener {
 public:
  virtual void KeyDown(SDL_Keycode sym) {}
  virtual void KeyUp(SDL_Keycode sym) {}
};
//----------------------------------------------------------------------------------------------------------------------
class MouseListener {
 public:
  virtual void Move(int x, int y) {}
  virtual void Move(int x, int y, int dx, int dy, bool left, bool right, bool middle) {}
  virtual void Wheel(int x, int y) {}
  virtual void LbDown(int x, int y) {}
  virtual void LbUp(int x, int y) {}
  virtual void RbDown(int x, int y) {}
  virtual void RbUp(int x, int y) {}
  virtual void MbDown(int x, int y) {}
  virtual void MbUp(int x, int y) {}
};
//----------------------------------------------------------------------------------------------------------------------
class JoyListener {
 public:
  virtual void Axis(int which, int axis, int value) {}
  virtual void BtDown(int which, int button) {}
  virtual void BtUp(int which, int button) {}
  virtual void Hat(int which, int hat, int value) {}
  virtual void Ball(int which, int ball, int xrel, int yrel) {}
};
//----------------------------------------------------------------------------------------------------------------------
class OtherEventListener {
 public:
  virtual void Event(const SDL_Event &evt) {}
  virtual void Quit() {}
  virtual void Other(Uint8 type, int code, void *data1, void *data2) {}
};
//----------------------------------------------------------------------------------------------------------------------
class InputListener :
    public JoyListener,
    public KeyboardListener,
    public MouseListener,
    public OtherEventListener {
};
//----------------------------------------------------------------------------------------------------------------------
class InputSequencer {
 public:
  InputSequencer() {
    kb_listeners.reserve(127);
    ms_listeners.reserve(127);
    joy_listeners.reserve(127);
    other_listeners.reserve(127);
  }

  InputSequencer(const InputSequencer &) = delete;
  InputSequencer &operator=(const InputSequencer &) = delete;
  InputSequencer(InputSequencer &&) = delete;
  InputSequencer &operator=(InputSequencer &&) = delete;

  static InputSequencer &GetSingleton() {
    static InputSequencer InputSingleton;
    return InputSingleton;
  }

 protected:
  using KeyboardListenersList = std::vector<KeyboardListener *>;
  using MouseListenersList = std::vector<MouseListener *>;
  using JoyListenersList = std::vector<JoyListener *>;
  using OtherListenersList = std::vector<OtherEventListener *>;

 protected:
  KeyboardListenersList kb_listeners;
  MouseListenersList ms_listeners;
  JoyListenersList joy_listeners;
  OtherListenersList other_listeners;

 public:
  virtual void RegKbListener(KeyboardListener *l) {
    if (l && find(kb_listeners.begin(), kb_listeners.end(), l) == kb_listeners.end()) {
      kb_listeners.push_back(l);
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  virtual void UnregKbListener(KeyboardListener *l) {
    if (!kb_listeners.empty()) {
      auto it = find(kb_listeners.begin(), kb_listeners.end(), l);
      if (l && it != kb_listeners.end()) {
        iter_swap(it, prev(kb_listeners.end()));
        kb_listeners.pop_back();
      }
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  virtual void RegMsListener(MouseListener *l) {
    if (l && find(ms_listeners.begin(), ms_listeners.end(), l) == ms_listeners.end()) {
      ms_listeners.push_back(l);
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  virtual void UnregMsListener(MouseListener *l) {
    if (!ms_listeners.empty()) {
      auto it = find(ms_listeners.begin(), ms_listeners.end(), l);
      if (l && it != ms_listeners.end()) {
        iter_swap(it, prev(ms_listeners.end()));
        ms_listeners.pop_back();
      }
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  virtual void RegJoyListener(JoyListener *l) {
    if (l && find(joy_listeners.begin(), joy_listeners.end(), l) == joy_listeners.end()) {
      joy_listeners.push_back(l);
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  virtual void UnregJoyListener(JoyListener *l) {
    if (!joy_listeners.empty()) {
      auto it = find(joy_listeners.begin(), joy_listeners.end(), l);
      if (l && it != joy_listeners.end()) {
        iter_swap(it, prev(joy_listeners.end()));
        joy_listeners.pop_back();
      }
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  virtual void RegEventListener(OtherEventListener *l) {
    if (l && find(other_listeners.begin(), other_listeners.end(), l) == other_listeners.end()) {
      other_listeners.push_back(l);
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  virtual void UnregEventListener(OtherEventListener *l) {
    if (!other_listeners.empty()) {
      auto it = find(other_listeners.begin(), other_listeners.end(), l);
      if (l && it != other_listeners.end()) {
        iter_swap(it, prev(other_listeners.end()));
        other_listeners.pop_back();
      }
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  virtual void RegisterListener(InputListener *l) {
    RegKbListener(l);
    RegMsListener(l);
    RegJoyListener(l);
    RegEventListener(l);
  }
//----------------------------------------------------------------------------------------------------------------------
  virtual void UnregisterListener(InputListener *l) {
    UnregKbListener(l);
    UnregMsListener(l);
    UnregJoyListener(l);
    UnregEventListener(l);
  }
//----------------------------------------------------------------------------------------------------------------------
  virtual void Reset() {
    kb_listeners.clear();
    ms_listeners.clear();
    joy_listeners.clear();
    other_listeners.clear();
  }
//----------------------------------------------------------------------------------------------------------------------
  virtual void Capture() {
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
}; //class InputSequencer
} //namespace io
