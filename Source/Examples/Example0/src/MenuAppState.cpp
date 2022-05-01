// This source file is part of Glue Engine. Created by Andrey Vasiliev

#include "PCHeader.h"
#include "MenuAppState.h"
#include "DemoDotAppState.h"
#include "Engine.h"

using namespace std;
using namespace Glue;

namespace Demo {

void MenuAppState::SetUp() {
  GetEngine().GrabMouse(false);

  GetAudio().AddSound("menu", "GameMenu.ogg", nullptr, true);
  GetAudio().PlaySound("menu");

  GetAudio().AddSound("selection", "Menu-Selection-Change-M.ogg");
  GetAudio().AddSound("click", "VideoGameMenuSoundsMenu-Selection-Change-N.ogg");
}

void MenuAppState::Cleanup() {}

void MenuAppState::Update(float time) { DrawOverlay(); }

void MenuAppState::OnKeyDown(SDL_Keycode sym) {}

void MenuAppState::DrawSettingsPage() {

}

void MenuAppState::DrawInputPage() {

}

void MenuAppState::DrawMainMenu() {
  static ImGuiIO& io = ImGui::GetIO();

  ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5, io.DisplaySize.y * 0.5), ImGuiCond_Always, ImVec2(0.5, 0.5));

  ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);

  ImGui::NewLine();

  if (ImGui::Button(ICON_KI_GAMEPAD u8"    Начать")) {
    GetAudio().PlaySound("selection", true);
    ChangeState(make_unique<DemoDotAppState>());
  }

  ImGui::NewLine();

  if (ImGui::Button(ICON_KI_WRENCH u8"    Настройки")) {
    GetAudio().PlaySound("selection", true);
  }

  ImGui::NewLine();

  if (ImGui::Button(ICON_KI_EXIT u8"    Выйти")) {
    GetAudio().PlaySound("selection", true);
    ChangeState();
  }

  ImGui::NewLine();

  ImGui::End();
}

void MenuAppState::DrawOverlay() {
  Ogre::ImGuiOverlay::NewFrame();

  DrawMainMenu();
}

}  // namespace Demo
