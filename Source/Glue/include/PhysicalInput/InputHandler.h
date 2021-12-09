// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Components/Component.h"
#include "Input.h"
#include "LazySingleton.h"
#include <vector>

namespace Glue {

class MutedInputObserver : public IInputObserver {
 public:
  MutedInputObserver();
  virtual ~MutedInputObserver();

  // Keyboard
  void OnKeyDown(SDL_Keycode sym) override {}
  void OnKeyUp(SDL_Keycode sym) override {}

  // Mouse
  void OnMouseMove(int dx, int dy) override {}
  void OnMouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) override {}
  void OnMouseWheel(int x, int y) override {}
  void OnMouseLbDown(int x, int y) override {}
  void OnMouseLbUp(int x, int y) override {}
  void OnMouseRbDown(int x, int y) override {}
  void OnMouseRbUp(int x, int y) override {}
  void OnMouseMbDown(int x, int y) override {}
  void OnMouseMbUp(int x, int y) override {}
  void OnTextInput(const char* text) override {}

  // Gamepad
  void OnGamepadAxis(int which, int axis, int value) override {}
  void OnGamepadBtDown(int which, int button) override {}
  void OnGamepadBtUp(int which, int button) override {}
  void OnGamepadHat(int which, int hat, int value) override {}
  void OnGamepadBall(int which, int ball, int xrel, int yrel) override {}
};

class InputHandler final : public InputObserver, public DynamicSingleton<InputHandler> {
 public:
  InputHandler();
  virtual ~InputHandler();

  void RegObserver(MutedInputObserver* p);
  void UnregObserver(MutedInputObserver* p);

  void Pause();
  void Resume();
  void Cleanup();
  void Update(float time);

  // These methods are used to turn off inputs in for some listeners by request

  // Keyboard
  void OnKeyDown(SDL_Keycode sym) override;
  void OnKeyUp(SDL_Keycode sym) override;
  void OnTextInput(const char* text) override;

  // Mouse
  void OnMouseMove(int dx, int dy) override;
  void OnMouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) override;
  void OnMouseWheel(int x, int y) override;
  void OnMouseLbDown(int x, int y) override;
  void OnMouseLbUp(int x, int y) override;
  void OnMouseRbDown(int x, int y) override;
  void OnMouseRbUp(int x, int y) override;
  void OnMouseMbDown(int x, int y) override;
  void OnMouseMbUp(int x, int y) override;

 protected:
  std::vector<MutedInputObserver*> io_listeners;
  bool paused_ = false;
};

}  // namespace Glue
