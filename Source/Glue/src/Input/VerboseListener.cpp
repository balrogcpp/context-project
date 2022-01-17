// This source file is part of "glue project". Created by Andrey Vasiliev

#include "PCHeader.h"
#include "Input/VerboseListener.h"
#include "Input/InputSequencer.h"
#include <cstdio>

using namespace std;

namespace Glue {

VerboseListener::VerboseListener() { InputSequencer::GetInstance().RegObserver(this); }

VerboseListener::~VerboseListener() { InputSequencer::GetInstance().UnRegObserver(this); }

void VerboseListener::SetVerbose(bool verbose) { verbose = verbose; }

bool VerboseListener::IsVerbose() { return verbose; }

void VerboseListener::OnKeyDown(SDL_Keycode sym) {
  if (verbose) {
    printf("%s sym %d\n", __func__, sym);
  }
}

void VerboseListener::OnKeyUp(SDL_Keycode sym) {
  if (verbose) {
    printf("%s sym %d\n", __func__, sym);
  }
}

void VerboseListener::OnMouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) {
  if (verbose) {
    printf("%s x %d y %d dx %d dy %d left %d right %d middle %d\n", __func__, x, y, dx, dy, left, right, middle);
  }
}

void VerboseListener::OnMouseWheel(int x, int y) {
  if (verbose) {
    printf("%s x %d y %d\n", __func__, x, y);
  }
}

void VerboseListener::OnMouseLbDown(int x, int y) {
  if (verbose) {
    printf("%s x %d y %d\n", __func__, x, y);
  }
}

void VerboseListener::OnMouseLbUp(int x, int y) {
  if (verbose) {
    printf("%s x %d y %d\n", __func__, x, y);
  }
}

void VerboseListener::OnMouseRbDown(int x, int y) {
  if (verbose) {
    printf("%s x %d y %d\n", __func__, x, y);
  }
}

void VerboseListener::OnMouseRbUp(int x, int y) {
  if (verbose) {
    printf("%s x %d y %d\n", __func__, x, y);
  }
}

void VerboseListener::OnMouseMbDown(int x, int y) {
  if (verbose) {
    printf("%s x %d y %d\n", __func__, x, y);
  }
}

void VerboseListener::OnMouseMbUp(int x, int y) {
  if (verbose) {
    printf("%s x %d y %d\n", __func__, x, y);
  }
}

void VerboseListener::OnGamepadAxis(int which, int axis, int value) {
  if (verbose) {
    printf("%s which %d axis %d value %d\n", __func__, which, axis, value);
  }
}

void VerboseListener::OnGamepadBtDown(int which, int button) {
  if (verbose) {
    printf("%s which %d button %d\n", __func__, which, button);
  }
}

void VerboseListener::OnGamepadBtUp(int which, int button) {
  if (verbose) {
    printf("%s which %d button %d\n", __func__, which, button);
  }
}

void VerboseListener::OnGamepadHat(int which, int hat, int value) {
  if (verbose) {
    printf("%s which %d hat %d value %d\n", __func__, which, hat, value);
  }
}

void VerboseListener::OnGamepadBall(int which, int ball, int xrel, int yrel) {
  if (verbose) {
    printf("%s which %d ball %d xrel %d yrel %d\n", __func__, which, ball, xrel, yrel);
  }
}

}  // namespace Glue
