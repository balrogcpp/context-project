/// created by Andrey Vasiliev

#pragma once

#include "AppStateManager.h"


namespace Glue {
class Menu final : public AppState {
 public:
  std::string GetName() override { return "Menu"; }
  void OnSetUp() override;
  void OnUpdate(float time) override;
  void BeforeRender(float time) override;
  void OnClean() override ;
  void OnKeyEvent(SDL_Scancode key, bool pressed) override;
};
}  // namespace Glue