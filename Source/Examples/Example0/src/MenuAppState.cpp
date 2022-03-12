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

void MenuAppState::Update(float time) {
  DrawOverlay();
}

void MenuAppState::OnKeyDown(SDL_Keycode sym) {}

void MenuAppState::DrawOverlay() {
  Ogre::ImGuiOverlay::NewFrame();

  static ImGuiIO& io = ImGui::GetIO();

  static bool ShowSimpleOverlay = true;
  if (ShowSimpleOverlay) GetOverlay().DrawSimpleOverlay(&ShowSimpleOverlay);

  DrawMenu();

  ImGui::End();
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
}

void MenuAppState::SetUp() {
  GetEngine().GrabMouse(false);

  GetAudio().CreateSound("menu", "GameMenu.ogg", true);
  GetAudio().PlaySound("menu");

  GetAudio().CreateSound("selection", "Menu-Selection-Change-M.ogg", false);
  GetAudio().CreateSound("click", "VideoGameMenuSoundsMenu-Selection-Change-N.ogg", false);
}

}  // namespace Demo
