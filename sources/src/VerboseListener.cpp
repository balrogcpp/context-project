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

#include "VerboseListener.h"

#include <iostream>

using namespace std;

namespace xio {

//----------------------------------------------------------------------------------------------------------------------
void VerboseListener::SetVerbose(bool verbose) { verbose_ = verbose; }

//----------------------------------------------------------------------------------------------------------------------
bool VerboseListener::IsVerbose() { return verbose_; }

//----------------------------------------------------------------------------------------------------------------------
void VerboseListener::OnKeyDown(SDL_Keycode sym) {
  if (verbose_) {
    printf("%s sym %d\n", __func__, sym);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void VerboseListener::OnKeyUp(SDL_Keycode sym) {
  if (verbose_) {
    printf("%s sym %d\n", __func__, sym);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void VerboseListener::OnMouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) {
  if (verbose_) {
    printf("%s x %d y %d dx %d dy %d left %d right %d middle %d\n", __func__, x, y, dx, dy, left, right, middle);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void VerboseListener::OnMouseWheel(int x, int y) {
  if (verbose_) {
    printf("%s x %d y %d\n", __func__, x, y);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void VerboseListener::OnMouseLbDown(int x, int y) {
  if (verbose_) {
    printf("%s x %d y %d\n", __func__, x, y);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void VerboseListener::OnMouseLbUp(int x, int y) {
  if (verbose_) {
    printf("%s x %d y %d\n", __func__, x, y);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void VerboseListener::OnMouseRbDown(int x, int y) {
  if (verbose_) {
    printf("%s x %d y %d\n", __func__, x, y);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void VerboseListener::OnMouseRbUp(int x, int y) {
  if (verbose_) {
    printf("%s x %d y %d\n", __func__, x, y);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void VerboseListener::OnMouseMbDown(int x, int y) {
  if (verbose_) {
    printf("%s x %d y %d\n", __func__, x, y);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void VerboseListener::OnMouseMbUp(int x, int y) {
  if (verbose_) {
    printf("%s x %d y %d\n", __func__, x, y);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void VerboseListener::OnJoystickAxis(int which, int axis, int value) {
  if (verbose_) {
    printf("%s which %d axis %d value %d\n", __func__, which, axis, value);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void VerboseListener::OnJoystickBtDown(int which, int button) {
  if (verbose_) {
    printf("%s which %d button %d\n", __func__, which, button);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void VerboseListener::OnJoystickBtUp(int which, int button) {
  if (verbose_) {
    printf("%s which %d button %d\n", __func__, which, button);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void VerboseListener::OnJoystickHat(int which, int hat, int value) {
  if (verbose_) {
    printf("%s which %d hat %d value %d\n", __func__, which, hat, value);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void VerboseListener::OnJoystickBall(int which, int ball, int xrel, int yrel) {
  if (verbose_) {
    printf("%s which %d ball %d xrel %d yrel %d\n", __func__, which, ball, xrel, yrel);
  }
}

}  // namespace xio
