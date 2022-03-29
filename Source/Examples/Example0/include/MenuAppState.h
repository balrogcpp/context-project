// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include "AppState.h"
#include "Components/metrics_gui.h"

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

  void DrawMainMenu();
  void DrawSettingsPage();
  void DrawInputPage();
  void DrawOverlay();

 private:
  MetricsGuiMetric frameTimeMetric{"Frame time", "s", MetricsGuiMetric::USE_SI_UNIT_PREFIX};
  MetricsGuiPlot frameTimePlot;
};

}  // namespace Demo
