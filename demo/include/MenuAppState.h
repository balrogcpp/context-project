// This source file is part of context-project
// Created by Andrew Vasiliev

#pragma once
#include "AppState.h"

namespace Demo {

class MenuAppState : public xio::AppState {
 public:
  MenuAppState() : AppState() {}
  virtual ~MenuAppState() {}

  void Init() override;
  void Cleanup() override;
  void Pause() override {}
  void Resume() override {}
  void Update(float time) override;

  void OnKeyDown(SDL_Keycode sym) override;
};

}
