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

#include <vector>
#include <string>
#include <exception>

namespace xio {
class InputSequencer;
//----------------------------------------------------------------------------------------------------------------------
class InputObserver {
 public:
  //Keyboard
  virtual void OnKeyDown(SDL_Keycode sym) {}
  virtual void OnKeyUp(SDL_Keycode sym) {}

  //Mouse
  virtual void OnMouseMove(int dx, int dy) {}
  virtual void OnMouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) {}
  virtual void OnMouseWheel(int x, int y) {}
  virtual void OnMouseLbDown(int x, int y) {}
  virtual void OnMouseLbUp(int x, int y) {}
  virtual void OnMouseRbDown(int x, int y) {}
  virtual void OnMouseRbUp(int x, int y) {}
  virtual void OnMouseMbDown(int x, int y) {}
  virtual void OnMouseMbUp(int x, int y) {}

  //Joystic
  virtual void OnJoysticAxis(int which, int axis, int value) {}
  virtual void OnJoysticBtDown(int which, int button) {}
  virtual void OnJoysticBtUp(int which, int button) {}
  virtual void OnJoysticHat(int which, int hat, int value) {}
  virtual void OnJoysticBall(int which, int ball, int xrel, int yrel) {}
};
//----------------------------------------------------------------------------------------------------------------------
class WindowObserver {
 public:
  virtual void Event(const SDL_Event &evt) {}
  virtual void Quit() {}
  virtual void Other(uint8_t type, int code, void *data1, void *data2) {}
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

  InputSequencer();
  virtual ~InputSequencer();

 private:
  KeyboardListenersList io_listeners;
  OtherListenersList win_listeners;
  const size_t RESERVE_SIZE = 16;
  inline static bool instanced_ = false;

 public:
  void RegObserver(InputObserver *p);

  void UnregObserver(InputObserver *p);

  void RegWinObserver(WindowObserver *p);

  void UnregWinObserver(WindowObserver *p);

  void Clear();

  void Reserve(size_t size);

  void Capture();
};
}