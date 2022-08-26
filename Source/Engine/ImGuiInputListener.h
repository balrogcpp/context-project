// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include "InputObserver.h"
#include "Singleton.h"
#include <imgui.h>
extern "C" {
#include <SDL2/SDL_keycode.h>
}

namespace Glue {
/// \class to control ImGui interface overlay
class ImGuiInputListener final : public InputObserver, public Singleton<ImGuiInputListener> {
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

}  // namespace Glue
