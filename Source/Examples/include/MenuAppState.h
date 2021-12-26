// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "AppState.h"

namespace Demo {

class MenuAppState : public Glue::AppState {
 public:
  MenuAppState() : AppState() {}
  virtual ~MenuAppState() {}

  void SetUp() override;
  void Cleanup() override;
  void Pause() override {}
  void Resume() override {}
  void Update(float time) override;

  void OnKeyDown(SDL_Keycode sym) override;
};

}  // namespace Demo
