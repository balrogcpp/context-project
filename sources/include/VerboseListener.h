// This source file is part of "glue project"
// Created by Andrew Vasiliev

#pragma once
#include "Input.h"

namespace glue {

class VerboseListener final : public InputObserver {
 public:
  void SetVerbose(bool verbose);
  bool IsVerbose();

 private:
  void OnKeyDown(SDL_Keycode sym) override;
  void OnKeyUp(SDL_Keycode sym) override;
  void OnMouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) override;
  void OnMouseWheel(int x, int y) override;
  void OnMouseLbDown(int x, int y) override;
  void OnMouseLbUp(int x, int y) override;
  void OnMouseRbDown(int x, int y) override;
  void OnMouseRbUp(int x, int y) override;
  void OnMouseMbDown(int x, int y) override;
  void OnMouseMbUp(int x, int y) override;
  void OnJoystickAxis(int which, int axis, int value) override;
  void OnJoystickBtDown(int which, int button) override;
  void OnJoystickBtUp(int which, int button) override;
  void OnJoystickHat(int which, int hat, int value) override;
  void OnJoystickBall(int which, int ball, int xrel, int yrel) override;

  bool verbose_ = true;
};

}  // namespace glue
