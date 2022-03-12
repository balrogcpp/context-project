// This source file is part of Glue Engine. Created by Andrey Vasiliev

#include "PCHeader.h"
#include "DemoDotAppState.h"
#include "Engine.h"
#include "MenuAppState.h"
#include "Objects/CameraMan.h"
#include "ShaderHelpers.h"

using namespace std;
using namespace Glue;

namespace Demo {

DemoDotAppState::DemoDotAppState() {}

DemoDotAppState::~DemoDotAppState() {}

void DemoDotAppState::Pause() {}

void DemoDotAppState::Resume() {}

void DemoDotAppState::OnKeyDown(SDL_Keycode sym) {
  if (SDL_GetScancodeFromKey(sym) == SDL_SCANCODE_ESCAPE) {
    ShowContextMenu = true;
    GetEngine().InMenu();
    GetEngine().GrabMouse(false);
  }
}

void DemoDotAppState::Cleanup() {}

void DemoDotAppState::Update(float time) {
  Ogre::ImGuiOverlay::NewFrame();

  static ImGuiIO &io = ImGui::GetIO();
  ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Always);
  ImGui::SetNextWindowSize({0, 0}, ImGuiCond_Always);
  ImGui::SetNextWindowCollapsed(false, ImGuiCond_Always);
  ImGui::SetNextWindowBgAlpha(0.5);

  ImGui::Begin("FPS", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  ImGui::End();

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  if (!ShowContextMenu) {
    GetEngine().OffMenu();
    return;
  } else {
    GetEngine().InMenu();
  }
#endif

  ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
  ImGui::SetNextWindowSize({0, 0}, ImGuiCond_Always);
  ImGui::SetNextWindowBgAlpha(0.5);
  ImGui::SetNextWindowFocus();

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  ImGui::SetNextWindowCollapsed(true, ImGuiCond_Appearing);
  ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoResize);
#else
  ImGui::SetNextWindowCollapsed(false, ImGuiCond_Always);
  ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
#endif

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  ImGui::NewLine();

  if (ImGui::Button("         Resume          ")) {
    GetAudio().PlaySound("selection", true);
    GetEngine().GrabMouse(true);
    GetEngine().OffMenu();
    ShowContextMenu = false;
  }
#endif

  ImGui::NewLine();

  if (ImGui::Button("        Menu        ")) {
    GetAudio().PlaySound("selection", true);
    ChangeState(make_unique<MenuAppState>());
  }

  ImGui::NewLine();

  if (ImGui::Button("          Quit           ")) {
    GetAudio().PlaySound("selection", true);
    ChangeState();
  }

  ImGui::NewLine();

  ImGui::End();
}

void DemoDotAppState::SetUp() {
  GetEngine().GrabMouse(true);
  // GetScene().GetCameraMan().SetStyle(CameraMan::ControlStyle::FPS);
  LoadFromFile("1.scene");

  GetAudio().CreateSound("menu", "GameSong2.ogg", true);
  GetAudio().PlaySound("menu");

  GetAudio().CreateSound("selection", "Menu-Selection-Change-M.ogg", false);
  GetAudio().CreateSound("click", "VideoGameMenuSoundsMenu-Selection-Change-N.ogg", false);

  Ogre::ImGuiOverlay::NewFrame();
}

}  // namespace Demo
