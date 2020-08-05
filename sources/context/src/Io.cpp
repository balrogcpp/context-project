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
void KeyboardListener::key_down(SDL_Keycode sym) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void KeyboardListener::key_up(SDL_Keycode sym) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void MouseListener::move(int x, int y) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void MouseListener::move(int x, int y, int dx, int dy, bool left, bool right, bool middle) {

}
//----------------------------------------------------------------------------------------------------------------------
void MouseListener::wheel(int x, int y) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void MouseListener::lb_down(int x, int y) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void MouseListener::lb_up(int x, int y) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void MouseListener::rb_down(int x, int y) {
  //
}

void MouseListener::rb_up(int x, int y) {

}
//----------------------------------------------------------------------------------------------------------------------
void MouseListener::mb_down(int x, int y) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void MouseListener::mb_up(int x, int y) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void JoyListener::axis(int which, int axis, int value) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void JoyListener::bt_down(int which, int button) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void JoyListener::bt_up(int which, int button) {

}
//----------------------------------------------------------------------------------------------------------------------
void JoyListener::hat(int which, int hat, int value) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void JoyListener::ball(int which, int ball, int xrel, int yrel) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void ControllerListener::axis(int which, int axis, int value) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void ControllerListener::bt_down(int which, int button) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void ControllerListener::bt_up(int which, int button) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void ControllerListener::hat(int which, int hat, int value) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void ControllerListener::ball(int which, int ball, int xrel, int yrel) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void OtherEventListener::event(const SDL_Event &evt) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void OtherEventListener::quit() {
  //
}

void OtherEventListener::user(Uint8 type, int code, void *data1, void *data2) {

}
//----------------------------------------------------------------------------------------------------------------------
void InputSequencer::reg_kb_listener(KeyboardListener *l) {
  if (l && find(kb_listeners.begin(), kb_listeners.end(), l) == kb_listeners.end()) {
    kb_listeners.insert(l);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void InputSequencer::unreg_kb_listener(KeyboardListener *l) {
  if (!kb_listeners.empty()) {
    if (l && find(kb_listeners.begin(), kb_listeners.end(), l) != kb_listeners.end()) {
      kb_listeners.erase(l);
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void InputSequencer::reg_ms_listener(MouseListener *l) {
  if (l && find(ms_listeners.begin(), ms_listeners.end(), l) == ms_listeners.end()) {
    ms_listeners.insert(l);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void InputSequencer::unreg_ms_listener(MouseListener *l) {
  if (!ms_listeners.empty()) {
    if (l && find(ms_listeners.begin(), ms_listeners.end(), l) != ms_listeners.end()) {
      ms_listeners.erase(l);
    }
  }
}

void InputSequencer::reg_joy_listener(JoyListener *l) {
  if (l && find(joy_listeners.begin(), joy_listeners.end(), l) == joy_listeners.end()) {
    joy_listeners.insert(l);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void InputSequencer::unreg_joy_listener(JoyListener *l) {
  if (!joy_listeners.empty()) {
    if (l && find(joy_listeners.begin(), joy_listeners.end(), l) != joy_listeners.end()) {
      joy_listeners.erase(l);
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void InputSequencer::reg_con_listener(ControllerListener *l) {
  if (l && find(cont_listeners.begin(), cont_listeners.end(), l) == cont_listeners.end()) {
    cont_listeners.insert(l);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void InputSequencer::unreg_con_listener(ControllerListener *l) {
  if (!cont_listeners.empty()) {
    if (l && find(cont_listeners.begin(), cont_listeners.end(), l) != cont_listeners.end()) {
      cont_listeners.erase(l);
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void InputSequencer::reg_event_listener(OtherEventListener *l) {
  if (l && find(other_listeners.begin(), other_listeners.end(), l) == other_listeners.end()) {
    other_listeners.insert(l);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void InputSequencer::unreg_event_listener(OtherEventListener *l) {
  if (!other_listeners.empty()) {
    if (l && find(other_listeners.begin(), other_listeners.end(), l) != other_listeners.end()) {
      other_listeners.erase(l);
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void InputSequencer::capture() {
  SDL_Event Event;
  while (SDL_PollEvent(&Event)) {

    switch (Event.type) {
      case SDL_KEYUP: {
        for (auto it : kb_listeners) {
          it->key_up(Event.key.keysym.sym);
        }
        break;
      }

      case SDL_KEYDOWN: {
        for (auto it : kb_listeners) {
          it->key_down(Event.key.keysym.sym);
        }
        break;
      }

      case SDL_MOUSEMOTION: {
        for (auto it : ms_listeners) {
          it->move(Event.motion.x, Event.motion.y,
                   Event.motion.xrel, Event.motion.yrel,
                   (Event.motion.state & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0,
                   (Event.motion.state & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0,
                   (Event.motion.state & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0);
        }
        break;
      }

      case SDL_MOUSEBUTTONDOWN: {
        switch (Event.button.button) {
          case SDL_BUTTON_LEFT: {
            for (auto it : ms_listeners) {
              it->lb_down(Event.button.x, Event.button.y);
            }
            break;
          }
          case SDL_BUTTON_RIGHT: {
            for (auto it : ms_listeners) {
              it->rb_down(Event.button.x, Event.button.y);
            }

            break;
          }
          case SDL_BUTTON_MIDDLE: {
            for (auto it : ms_listeners) {
              it->mb_down(Event.button.x, Event.button.y);
            }
            break;
          }
        }
        break;
      }

      case SDL_MOUSEBUTTONUP: {
        switch (Event.button.button) {
          case SDL_BUTTON_LEFT: {
            for (auto it : ms_listeners) {
              it->lb_up(Event.button.x, Event.button.y);
            }
            break;
          }
          case SDL_BUTTON_RIGHT: {
            for (auto it : ms_listeners) {
              it->rb_up(Event.button.x, Event.button.y);
            }
            break;
          }
          case SDL_BUTTON_MIDDLE: {
            for (auto it : ms_listeners) {
              it->mb_up(Event.button.x, Event.button.y);
            }
            break;
          }
        }
        break;
      }

      case SDL_MOUSEWHEEL: {
        for (auto it : ms_listeners) {
          it->wheel(Event.wheel.x, Event.wheel.y);
        }

        break;
      }

      case SDL_JOYAXISMOTION: {
        for (auto it : joy_listeners) {
          it->axis(Event.jaxis.which, Event.jaxis.axis, Event.jaxis.value);
        }

        break;
      }

      case SDL_JOYBALLMOTION: {
        for (auto it : joy_listeners) {
          it->ball(Event.jball.which, Event.jball.ball, Event.jball.xrel, Event.jball.yrel);
        }
        break;
      }

      case SDL_JOYHATMOTION: {
        for (auto it : joy_listeners) {
          it->hat(Event.jhat.which, Event.jhat.hat, Event.jhat.value);
        }
        break;
      }
      case SDL_JOYBUTTONDOWN: {
        for (auto it : joy_listeners) {
          it->bt_down(Event.jbutton.which, Event.jbutton.button);
        }
        break;
      }

      case SDL_JOYBUTTONUP: {
        for (auto it : joy_listeners) {
          if (it) {
            it->bt_up(Event.jbutton.which,
                      Event.jbutton.button);
          }
        }
        break;
      }

      case SDL_QUIT: {
        for (auto it : other_listeners) {
          it->quit();
        }
        break;
      }

      default: {
        for (auto it : other_listeners) {
          it->event(Event);
        }
      }
    }
  }
}

} //namespace Context
