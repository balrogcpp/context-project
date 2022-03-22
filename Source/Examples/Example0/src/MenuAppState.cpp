// This source file is part of Glue Engine. Created by Andrey Vasiliev

#include "PCHeader.h"
#include "MenuAppState.h"
#include "DemoDotAppState.h"
#include "Engine.h"
#define SDL_h_
#include "ImHotKey.h"

using namespace std;
using namespace Glue;

namespace Demo {

void MenuAppState::Cleanup() {}

void MenuAppState::Update(float time) { DrawOverlay(); }

void MenuAppState::OnKeyDown(SDL_Keycode sym) {}

void MenuAppState::DrawOverlay() {
  Ogre::ImGuiOverlay::NewFrame();

  static ImGuiIO& io = ImGui::GetIO();

  static bool ShowSimpleOverlay = true;
  if (ShowSimpleOverlay) GetOverlay().DrawSimpleOverlay(&ShowSimpleOverlay);

//  ImGui::Begin("MetricsGuiMetric");
//  frameTimeMetric.AddNewValue(1.f / io.Framerate);
//  frameTimePlot.UpdateAxes();
//  frameTimePlot.DrawHistory();
//  ImGui::End();

  DrawMenu();
}

void MenuAppState::DrawMenu() {
  static ImGuiIO& io = ImGui::GetIO();

  ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5, io.DisplaySize.y * 0.5), ImGuiCond_Always, ImVec2(0.5, 0.5));
  ImGui::SetNextWindowSize({0, 0}, ImGuiCond_Always);
  ImGui::SetNextWindowCollapsed(false, ImGuiCond_Always);
  ImGui::SetNextWindowFocus();

  ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

  ImGui::NewLine();

  if (ImGui::Button(u8"Start")) {
    GetAudio().PlaySound("selection", true);
    ChangeState(make_unique<DemoDotAppState>());
  }

  ImGui::NewLine();

  if (ImGui::Button(u8"Quit")) {
    GetAudio().PlaySound("selection", true);
    ChangeState();
  }

  ImGui::NewLine();

  ImGui::End();
}

void MenuAppState::SetUp() {
  GetEngine().GrabMouse(false);

  GetAudio().CreateSound("menu", "GameMenu.ogg", true);
  GetAudio().PlaySound("menu");

  GetAudio().CreateSound("selection", "Menu-Selection-Change-M.ogg", false);
  GetAudio().CreateSound("click", "VideoGameMenuSoundsMenu-Selection-Change-N.ogg", false);

  frameTimeMetric.mSelected = true;

  frameTimePlot.mBarRounding = 0.f;           // amount of rounding on bars
  frameTimePlot.mRangeDampening = 0.95f;      // weight of historic range on axis range [0,1]
  frameTimePlot.mInlinePlotRowCount = 2;      // height of DrawList() inline plots, in text rows
  frameTimePlot.mPlotRowCount = 5;            // height of DrawHistory() plots, in text rows
  frameTimePlot.mVBarMinWidth = 6;            // min width of bar graph bar in pixels
  frameTimePlot.mVBarGapWidth = 1;            // width of bar graph inter-bar gap in pixels
  frameTimePlot.mShowAverage = false;         // draw horizontal line at series average
  frameTimePlot.mShowInlineGraphs = false;    // show history plot in DrawList()
  frameTimePlot.mShowOnlyIfSelected = false;  // draw show selected metrics
  frameTimePlot.mShowLegendDesc = true;       // show series description in legend
  frameTimePlot.mShowLegendColor = true;      // use series color in legend
  frameTimePlot.mShowLegendUnits = true;      // show units in legend values
  frameTimePlot.mShowLegendAverage = false;   // show series average in legend
  frameTimePlot.mShowLegendMin = true;        // show plot y-axis minimum in legend
  frameTimePlot.mShowLegendMax = true;        // show plot y-axis maximum in legend
  frameTimePlot.mBarGraph = false;            // use bars to draw history
  frameTimePlot.mStacked = false;             // stack series when drawing history
  frameTimePlot.mSharedAxis = false;          // use first series' axis range
  frameTimePlot.mFilterHistory = true;        // allow single plot point to represent more than on history value

  frameTimePlot.AddMetric(&frameTimeMetric);
}

}  // namespace Demo
