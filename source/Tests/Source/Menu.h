/// created by Andrey Vasiliev

#pragma once

#include "AppStateManager.h"


namespace gge {
class Menu final : public AppState {
 public:
  std::string GetName() override { return "Menu"; }
  void OnSetUp() override;
  void OnUpdate(float time) override;
  void OnClean() override ;
  void OnKeyEvent(SDL_Scancode key, bool pressed) override;
  void OnSizeChanged(int x, int y, uint32_t id) override;
};
}  // namespace gge
