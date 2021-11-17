// This source file is part of "glue project"
// Created by Andrew Vasiliev

#pragma once
#include "Input.h"
#include "Singleton.h"
#include "System.h"
#include <vector>

namespace glue {

class CommandObserver {
 public:
};

class MutedInputObserver : public InputObserverI {
 public:
  MutedInputObserver();
  virtual ~MutedInputObserver();
};

class InputHandler final : public InputObserver, public Singleton<InputHandler> {
 public:
  InputHandler();
  virtual ~InputHandler();

  void RegObserver(view_ptr<MutedInputObserver> p);
  void UnregObserver(view_ptr<MutedInputObserver> p);

  void Pause();
  void Resume();
  void Cleanup();
  void Update(float time);

  // These methods are used to turn off inputs in for some listeners by request

  // Keyboard
  void OnKeyDown(SDL_Keycode sym) override;
  void OnKeyUp(SDL_Keycode sym) override;
  void OnTextInput(const char *text) override;

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
  std::vector<view_ptr<MutedInputObserver>> io_listeners;
  // std::vector<view_ptr<CommandObserver>> cmd_listeners;
  bool paused_ = false;
};

}  // namespace glue
