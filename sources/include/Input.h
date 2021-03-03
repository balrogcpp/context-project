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
#include "view_ptr.h"


namespace xio {
class InputSequencer;
class InputObserver;
class WindowObserver;

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
  using KeyboardListenersList = std::vector<view_ptr<InputObserver>>;
  using OtherListenersList = std::vector<view_ptr<WindowObserver>>;

  //Copy not allowed
  InputSequencer(const InputSequencer &) = delete;
  InputSequencer &operator=(const InputSequencer &) = delete;

  InputSequencer();
  virtual ~InputSequencer();

  static InputSequencer &GetInstance();

 private:
  KeyboardListenersList io_listeners;
  OtherListenersList win_listeners;
  const size_t RESERVE_SIZE = 16;
  inline static bool instanced_;

 public:
  void RegObserver(view_ptr<InputObserver> p);

  void UnregObserver(view_ptr<InputObserver> p);

  void RegWinObserver(view_ptr<WindowObserver> p);

  void UnregWinObserver(view_ptr<WindowObserver> p);

  void Clear();

  void Reserve(size_t size);

  void Capture();
};

//----------------------------------------------------------------------------------------------------------------------
class InputObserver {
 public:
  InputObserver();
  virtual ~InputObserver();

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

  //Joystick
  virtual void OnJoystickAxis(int which, int axis, int value) {}
  virtual void OnJoystickBtDown(int which, int button) {}
  virtual void OnJoystickBtUp(int which, int button) {}
  virtual void OnJoystickHat(int which, int hat, int value) {}
  virtual void OnJoystickBall(int which, int ball, int xrel, int yrel) {}
};
//----------------------------------------------------------------------------------------------------------------------
class WindowObserver {
 public:
  WindowObserver();
  virtual ~WindowObserver();

  virtual void Event(const SDL_Event &evt) {}
  virtual void Quit() {}
  virtual void Other(uint8_t type, int code, void *data1, void *data2) {}
};
} //namespace
