// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Input/InputObserver.h"

namespace Glue {

class InputObserverImpl : public InputObserver {
 public:
  virtual void OnKeyDown(SDL_Keycode sym) override;
  virtual void OnKeyUp(SDL_Keycode sym) override;
  virtual void OnMouseMove(int dx, int dy) override;
  virtual void OnMouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) override;
  virtual void OnMouseWheel(int x, int y) override;
  virtual void OnMouseLbDown(int x, int y) override;
  virtual void OnMouseLbUp(int x, int y) override;
  virtual void OnMouseRbDown(int x, int y) override;
  virtual void OnMouseRbUp(int x, int y) override;
  virtual void OnMouseMbDown(int x, int y) override;
  virtual void OnMouseMbUp(int x, int y) override;
  virtual void OnTextInput(const char *text) override;
  virtual void OnGamepadAxis(int which, int axis, int value) override;
  virtual void OnGamepadBtDown(int which, int button) override;
  virtual void OnGamepadBtUp(int which, int button) override;
  virtual void OnGamepadHat(int which, int hat, int value) override;
  virtual void OnGamepadBall(int which, int ball, int xrel, int yrel) override;
};

}  // namespace Glue
