/// created by Andrey Vasiliev

#pragma once

#include "AppStateManager.h"

namespace gge {

class Game final : public AppState {
 public:
  std::string GetName() override { return "Game"; }
  void OnSetUp() override;
  void OnUpdate(float time) override;
  void OnClean() override ;
};

}  // namespace gge
