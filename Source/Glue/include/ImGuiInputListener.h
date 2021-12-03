// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Input.h"
extern "C" {
#include <SDL2/SDL_keycode.h>
}

namespace Glue {

class ImGuiInputListener final : public InputObserver {
 public:
  ImGuiInputListener();
  virtual ~ImGuiInputListener();

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
};

}  // namespace glue
