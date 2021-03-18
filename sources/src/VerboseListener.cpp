//MIT License
//
//Copyright (c) 2021 Andrew Vasiliev
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

#include <iostream>
#include "VerboseListener.h"

using namespace std;

namespace xio {

void VerboseListener::OnKeyDown(SDL_Keycode sym) {
  if (verbose_) {
	cout << __func__;
	cout << ' ';
	cout << "sym ";
	cout << sym;
	cout << '\n';
  }
}

//----------------------------------------------------------------------------------------------------------------------
void VerboseListener::OnKeyUp(SDL_Keycode sym) {
  if (verbose_) {
	cout << __func__;
	cout << ' ';
	cout <<
			  sym;
	cout << '\n';
  }
}

//----------------------------------------------------------------------------------------------------------------------
void VerboseListener::OnMouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) {
  if (verbose_) {
	cout << __func__;
	cout << ' ';
	cout << " x ";
	cout << x << ' ';
	cout << "y" << ' ';
	cout << y << ' ';
	cout << "dx" << ' ';
	cout << dx << ' ';
	cout << "dy" << ' ';
	cout << dy << ' ';
	cout << "left" << ' ';
	cout << left << ' ';
	cout << "right" << ' ';
	cout << right << ' ';
	cout << "middle" << ' ';
	cout << middle << ' ';
	cout << '\n';
  }
}

//----------------------------------------------------------------------------------------------------------------------
void VerboseListener::OnMouseWheel(int x, int y) {
  if (verbose_) {
	cout << __func__;
	cout << "Up" << ' ';
	cout << x << ' ';
	cout << "Down" << ' ';
	cout << y << '\n';
  }
}

//----------------------------------------------------------------------------------------------------------------------
void VerboseListener::OnMouseLbDown(int x, int y) {
  if (verbose_) {
	cout << __func__;
	cout << ' ';
	cout << "x" << ' ';
	cout << x << ' ';
	cout << "y" << ' ';
	cout << y << ' ';
	cout << '\n';
  }
}

//----------------------------------------------------------------------------------------------------------------------
void VerboseListener::OnMouseLbUp(int x, int y) {
  if (verbose_) {
	cout << __func__;
	cout << ' ';
	cout << "x" << ' ';
	cout << x << ' ';
	cout << "y" << ' ';
	cout << y << ' ';
	cout << '\n';
  }
}

//----------------------------------------------------------------------------------------------------------------------
void VerboseListener::OnMouseRbDown(int x, int y) {
  if (verbose_) {
	cout << __func__;
	cout << ' ';
	cout << "x" << ' ';
	cout << x << ' ';
	cout << "y" << ' ';
	cout << y << ' ';
	cout << '\n';
  }
}

//----------------------------------------------------------------------------------------------------------------------
void VerboseListener::OnMouseRbUp(int x, int y) {
  if (verbose_) {
	cout << __func__;
	cout << ' ';
	cout << "x" << ' ';
	cout << x << ' ';
	cout << "y" << ' ';
	cout << y << ' ';
	cout << '\n';
  }
}

//----------------------------------------------------------------------------------------------------------------------
void VerboseListener::OnMouseMbDown(int x, int y) {
  if (verbose_) {
	cout << __func__;
	cout << ' ';
	cout << "x" << ' ';
	cout << x << ' ';
	cout << "y" << ' ';
	cout << y << ' ';
	cout << '\n';
  }
}

//----------------------------------------------------------------------------------------------------------------------
void VerboseListener::OnMouseMbUp(int x, int y) {
  if (verbose_) {
	cout << __func__;
	cout << ' ';
	cout << "x" << ' ';
	cout << x << ' ';
	cout << "y" << ' ';
	cout << y << ' ';
	cout << '\n';
  }
}

//----------------------------------------------------------------------------------------------------------------------
void VerboseListener::OnJoystickAxis(int which, int axis, int value) {
  if (verbose_) {
	cout << __func__;
	cout << ' ';
	cout << "which" << ' ';
	cout << which << ' ';
	cout << "axis" << ' ';
	cout << axis << ' ';
	cout << "value" << ' ';
	cout << value << ' ';
	cout << '\n';
  }
}

//----------------------------------------------------------------------------------------------------------------------
void VerboseListener::OnJoystickBtDown(int which, int button) {
  if (verbose_) {
	cout << __func__;
	cout << ' ';
	cout << "which" << ' ';
	cout << which << ' ';
	cout << "button" << ' ';
	cout << button << ' ';
	cout << '\n';
  }
}

//----------------------------------------------------------------------------------------------------------------------
void VerboseListener::OnJoystickBtUp(int which, int button) {
  if (verbose_) {
	cout << __func__;
	cout << ' ';
	cout << "which" << ' ';
	cout << which << ' ';
	cout << "button" << ' ';
	cout << button << ' ';
	cout << '\n';
  }
}

//----------------------------------------------------------------------------------------------------------------------
void VerboseListener::OnJoystickHat(int which, int hat, int value) {
  if (verbose_) {
	cout << __func__;
	cout << ' ';
	cout << "which" << ' ';
	cout << which << ' ';
	cout << "hat" << ' ';
	cout << hat << ' ';
	cout << "value" << ' ';
	cout << value << ' ';
	cout << '\n';
  }
}

//----------------------------------------------------------------------------------------------------------------------
void VerboseListener::OnJoystickBall(int which, int ball, int xrel, int yrel) {
  if (verbose_) {
	cout << __func__;
	cout << ' ';
	cout << "which" << ' ';
	cout << which << ' ';
	cout << "ball" << ' ';
	cout << ball << ' ';
	cout << "yrel" << ' ';
	cout << yrel << ' ';
	cout << '\n';
  }
}

} //namespace
