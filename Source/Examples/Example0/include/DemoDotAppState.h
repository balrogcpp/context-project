// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include "AppState.h"

namespace Demo {

class DemoDotAppState final : public Glue::AppState {
 public:
  DemoDotAppState() {}
  virtual ~DemoDotAppState() {}

  void SetUp() override;
  void Cleanup() override;
  void Pause() override;
  void Resume() override;
  void Update(float time) override;
  void OnKeyDown(SDL_Keycode sym) override;

  void DrawMenu();

 private:
  bool ShowContextMenu = false;
};

}  // namespace Demo
