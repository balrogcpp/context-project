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
#include "DummyListener.hpp"

extern "C" {
#include <SDL2/SDL.h>
}

#include <iostream>

namespace Context {

//----------------------------------------------------------------------------------------------------------------------
void DummyListener::KeyDown(SDL_Keycode sym) {
  if (print_) {
    std::cout << __func__;
    std::cout << " ";
    std::cout << "sym ";
    std::cout << sym;
    std::cout << '\n';
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DummyListener::KeyUp(SDL_Keycode sym) {
  if (print_) {
    std::cout << __func__;
    std::cout << " ";
    std::cout << sym;
    std::cout << '\n';
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DummyListener::Move(int x, int y, int dx, int dy, bool left, bool right, bool middle) {
  if (print_) {
    std::cout << __func__;
    std::cout << " ";
    std::cout << " x ";
    std::cout << x << " ";
    std::cout << "y" << " ";
    std::cout << y << " ";
    std::cout << "dx" << " ";
    std::cout << dx << " ";
    std::cout << "dy" << " ";
    std::cout << dy << " ";
    std::cout << "left" << " ";
    std::cout << left << " ";
    std::cout << "right" << " ";
    std::cout << right << " ";
    std::cout << "middle" << " ";
    std::cout << middle << " ";
    std::cout << '\n';
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DummyListener::Wheel(int x, int y) {
  if (print_) {
    std::cout << __func__;
    std::cout << "Up" << " ";
    std::cout << x << " ";
    std::cout << "Down" << " ";
    std::cout << y << '\n';
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DummyListener::LbDown(int x, int y) {
  if (print_) {
    std::cout << __func__;
    std::cout << " ";
    std::cout << "x" << " ";
    std::cout << x << " ";
    std::cout << "y" << " ";
    std::cout << y << " ";
    std::cout << '\n';
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DummyListener::LbUp(int x, int y) {
  if (print_) {
    std::cout << __func__;
    std::cout << " ";
    std::cout << "x" << " ";
    std::cout << x << " ";
    std::cout << "y" << " ";
    std::cout << y << " ";
    std::cout << '\n';
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DummyListener::RbDown(int x, int y) {
  if (print_) {
    std::cout << __func__;
    std::cout << " ";
    std::cout << "x" << " ";
    std::cout << x << " ";
    std::cout << "y" << " ";
    std::cout << y << " ";
    std::cout << '\n';
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DummyListener::RbUp(int x, int y) {
  if (print_) {
    std::cout << __func__;
    std::cout << " ";
    std::cout << "x" << " ";
    std::cout << x << " ";
    std::cout << "y" << " ";
    std::cout << y << " ";
    std::cout << '\n';
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DummyListener::MbDown(int x, int y) {
  if (print_) {
    std::cout << __func__;
    std::cout << " ";
    std::cout << "x" << " ";
    std::cout << x << " ";
    std::cout << "y" << " ";
    std::cout << y << " ";
    std::cout << '\n';
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DummyListener::MbUp(int x, int y) {
  if (print_) {
    std::cout << __func__;
    std::cout << " ";
    std::cout << "x" << " ";
    std::cout << x << " ";
    std::cout << "y" << " ";
    std::cout << y << " ";
    std::cout << '\n';
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DummyListener::Axis(int which, int axis, int value) {
  if (print_) {
    std::cout << __func__;
    std::cout << " ";
    std::cout << "which" << " ";
    std::cout << which << " ";
    std::cout << "axis" << " ";
    std::cout << axis << " ";
    std::cout << "value" << " ";
    std::cout << value << " ";
    std::cout << '\n';
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DummyListener::BtDown(int which, int button) {
  if (print_) {
    std::cout << __func__;
    std::cout << " ";
    std::cout << "which" << " ";
    std::cout << which << " ";
    std::cout << "button" << " ";
    std::cout << button << " ";
    std::cout << '\n';
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DummyListener::BtUp(int which, int button) {
  if (print_) {
    std::cout << __func__;
    std::cout << " ";
    std::cout << "which" << " ";
    std::cout << which << " ";
    std::cout << "button" << " ";
    std::cout << button << " ";
    std::cout << '\n';
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DummyListener::Hat(int which, int hat, int value) {
  if (print_) {
    std::cout << __func__;
    std::cout << " ";
    std::cout << "which" << " ";
    std::cout << which << " ";
    std::cout << "hat" << " ";
    std::cout << hat << " ";
    std::cout << "value" << " ";
    std::cout << value << " ";
    std::cout << '\n';
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DummyListener::Ball(int which, int ball, int xrel, int yrel) {
  if (print_) {
    std::cout << __func__;
    std::cout << " ";
    std::cout << "which" << " ";
    std::cout << which << " ";
    std::cout << "ball" << " ";
    std::cout << ball << " ";
    std::cout << "yrel" << "";
    std::cout << yrel << "";
    std::cout << '\n';
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DummyListener::Event(const SDL_Event &evt) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void DummyListener::Quit() {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void DummyListener::Other(Uint8 type, int code, void *data1, void *data2) {
  //
}

} //namespace Context
