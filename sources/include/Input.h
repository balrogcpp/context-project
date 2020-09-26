//MIT License
//
//Copyright (c) 2020 Andrey Vasiliev
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#pragma once

extern "C" {
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_events.h>
}
#include <cstdint>
#include <vector>
#include <exception>

namespace xio {
class InputSequencer;
//----------------------------------------------------------------------------------------------------------------------
class InputObserver {
 public:
  //Keyboard
  virtual void KeyDown(SDL_Keycode sym) {}
  virtual void KeyUp(SDL_Keycode sym) {}

  //Mouse
  virtual void Move(int32_t x, int32_t y) {}
  virtual void Move(int32_t x, int32_t y, int32_t dx, int32_t dy, bool left, bool right, bool middle) {}
  virtual void Wheel(int32_t x, int32_t y) {}
  virtual void LbDown(int32_t x, int32_t y) {}
  virtual void LbUp(int32_t x, int32_t y) {}
  virtual void RbDown(int32_t x, int32_t y) {}
  virtual void RbUp(int32_t x, int32_t y) {}
  virtual void MbDown(int32_t x, int32_t y) {}
  virtual void MbUp(int32_t x, int32_t y) {}

  //Joystic
  virtual void Axis(int32_t which, int32_t axis, int32_t value) {}
  virtual void BtDown(int32_t which, int32_t button) {}
  virtual void BtUp(int32_t which, int32_t button) {}
  virtual void Hat(int32_t which, int32_t hat, int32_t value) {}
  virtual void Ball(int32_t which, int32_t ball, int32_t xrel, int32_t yrel) {}
};
//----------------------------------------------------------------------------------------------------------------------
class WindowObserver {
 public:
  virtual void Event(const SDL_Event &evt) {}
  virtual void Quit() {}
  virtual void Other(uint8_t type, int32_t code, void *data1, void *data2) {}
};
//----------------------------------------------------------------------------------------------------------------------
class InputException : public std::exception {
 public:
  InputException() = default;

  explicit InputException(std::string description)
      : description(std::move(description)) {}

  virtual ~InputException() {}

 public:
  std::string getDescription() const noexcept {
    return description;
  }

  const char *what() const noexcept override {
    return description.c_str();
  }

 protected:
  std::string description = std::string("Description not specified");
  size_t code = 0;
};
//----------------------------------------------------------------------------------------------------------------------
class InputSequencer {
 public:
  using KeyboardListenersList = std::vector<InputObserver *>;
  using OtherListenersList = std::vector<WindowObserver *>;

  //Copy not allowed
  InputSequencer(const InputSequencer &) = delete;
  InputSequencer &operator=(const InputSequencer &) = delete;

  InputSequencer() {
    if (instanced_)
      throw InputException("Only one instance of InputSequencer is allowed!\n");
    Reserve(RESERVE_SIZE);
    instanced_ = true;
  }

 private:
  KeyboardListenersList io_listeners;
  OtherListenersList win_listeners;
  const size_t RESERVE_SIZE = 16;
  inline static bool instanced_ = false;

 public:
  void RegObserver(InputObserver *p) {
    io_listeners.push_back(p);
  }
//----------------------------------------------------------------------------------------------------------------------
  void UnregObserver(InputObserver *p) {
    auto it = find(io_listeners.begin(), io_listeners.end(), p);
    if (it != io_listeners.end()) {
      iter_swap(it, prev(io_listeners.end()));
      io_listeners.pop_back();
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  void RegWinObserver(WindowObserver *p) {
    win_listeners.push_back(p);
  }
//----------------------------------------------------------------------------------------------------------------------
  void UnregWinObserver(WindowObserver *p) {
    auto it = find(win_listeners.begin(), win_listeners.end(), p);
    if (it != win_listeners.end()) {
      iter_swap(it, prev(win_listeners.end()));
      win_listeners.pop_back();
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  void Clear() {
    io_listeners.clear();
    io_listeners.shrink_to_fit();
    win_listeners.clear();
    win_listeners.shrink_to_fit();
  }
//----------------------------------------------------------------------------------------------------------------------
  void Reserve(size_t size) {
    io_listeners.reserve(size);
    win_listeners.reserve(size);
  }
//----------------------------------------------------------------------------------------------------------------------
  void Capture() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {

      switch (event.type) {
        case SDL_KEYUP: {
          for (auto it : io_listeners)
            it->KeyUp(event.key.keysym.sym);
          break;
        }
        case SDL_KEYDOWN: {
          for (auto it : io_listeners)
            it->KeyDown(event.key.keysym.sym);
          break;
        }
        case SDL_MOUSEMOTION: {
          for (auto it : io_listeners)
            it->Move(event.motion.x, event.motion.y,
                     event.motion.xrel, event.motion.yrel,
                     (event.motion.state & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0,
                     (event.motion.state & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0,
                     (event.motion.state & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0);
          break;
        }
        case SDL_MOUSEBUTTONDOWN: {
          switch (event.button.button) {
            case SDL_BUTTON_LEFT: {
              for (auto it : io_listeners)
                it->LbDown(event.button.x, event.button.y);
              break;
            }
            case SDL_BUTTON_RIGHT: {
              for (auto it : io_listeners)
                it->RbDown(event.button.x, event.button.y);
              break;
            }
            case SDL_BUTTON_MIDDLE: {
              for (auto it : io_listeners)
                it->MbDown(event.button.x, event.button.y);
              break;
            }
          }
          break;
        }
        case SDL_MOUSEBUTTONUP: {
          switch (event.button.button) {
            case SDL_BUTTON_LEFT: {
              for (auto it : io_listeners)
                it->LbUp(event.button.x, event.button.y);
              break;
            }
            case SDL_BUTTON_RIGHT: {
              for (auto it : io_listeners)
                it->RbUp(event.button.x, event.button.y);
              break;
            }
            case SDL_BUTTON_MIDDLE: {
              for (auto it : io_listeners)
                it->MbUp(event.button.x, event.button.y);
              break;
            }
          }
          break;
        }

        case SDL_MOUSEWHEEL: {
          for (auto it : io_listeners)
            it->Wheel(event.wheel.x, event.wheel.y);
          break;
        }

        case SDL_JOYAXISMOTION: {
          for (auto it : io_listeners)
            it->Axis(event.jaxis.which, event.jaxis.axis, event.jaxis.value);
          break;
        }
        case SDL_JOYBALLMOTION: {
          for (auto it : io_listeners)
            it->Ball(event.jball.which, event.jball.ball, event.jball.xrel, event.jball.yrel);
          break;
        }
        case SDL_JOYHATMOTION: {
          for (auto it : io_listeners)
            it->Hat(event.jhat.which, event.jhat.hat, event.jhat.value);
          break;
        }
        case SDL_JOYBUTTONDOWN: {
          for (auto it : io_listeners)
            it->BtDown(event.jbutton.which, event.jbutton.button);
          break;
        }
        case SDL_JOYBUTTONUP: {
          for (auto it : io_listeners) {
            if (it)
              it->BtUp(event.jbutton.which, event.jbutton.button);
          }
          break;
        }
        case SDL_QUIT: {
          for (auto it : win_listeners)
            it->Quit();
          break;
        }
        default: {
          for (auto it : win_listeners)
            it->Event(event);
        }
      }
    }
  }
};
}