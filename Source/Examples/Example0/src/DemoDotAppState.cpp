// This source file is part of Glue Engine. Created by Andrey Vasiliev

#include "PCHeader.h"
#include "DemoDotAppState.h"
#include "CameraMan.h"
#include "Engine.h"
#include "MenuAppState.h"
#include "ShaderHelpers.h"

using namespace std;
using namespace Glue;

namespace Demo {

void DemoDotAppState::SetUp() {
  ShowMouseCursor(false);
  // GetScene().GetCameraMan().SetStyle(CameraMan::ControlStyle::FPS);
  LoadFromFile("1.scene");
  GetAudio().AddSound("ambient_music", "GameSong2.ogg", nullptr, true);
  GetAudio().SetSoundVolume("ambient_music", 0.5);
  //  GetAudio().PlaySound("ambient_music");
  //  GetAudio().AddSound("wind", "Wind-Mark_DiAngelo-1940285615.ogg", nullptr, true);
  //  GetAudio().PlaySound("wind");
  GetAudio().AddSound("selection", "Menu-Selection-Change-M.ogg");
  GetAudio().AddSound("click", "VideoGameMenuSoundsMenu-Selection-Change-N.ogg");
}

void DemoDotAppState::DrawMenu() {
  static ImGuiIO& io = ImGui::GetIO();
  Overlay::NewFrame();
  if (ShowContextMenu) {
    static bool use_work_area = true;
    static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(use_work_area ? viewport->WorkPos : viewport->Pos);
    ImGui::SetNextWindowSize(use_work_area ? viewport->WorkSize : viewport->Size);
    ImGui::SetNextWindowBgAlpha(0.8);
    ImGui::Begin("Example: Fullscreen window", nullptr, flags);
    ImGui::End();
  }
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  if (!ShowContextMenu) {
    GetEngine().OnMenuOff();
    return;
  } else {
    GetEngine().OnMenuOn();
  }
#endif
  ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5, io.DisplaySize.y * 0.5), ImGuiCond_Always, ImVec2(0.5, 0.5));
  ImGui::SetNextWindowSize({0, 0}, ImGuiCond_Always);
  ImGui::SetNextWindowBgAlpha(0.5);
  ImGui::SetNextWindowFocus();
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  ImGui::SetNextWindowCollapsed(true, ImGuiCond_Appearing);
  ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoResize);
#else
  ImGui::SetNextWindowCollapsed(false, ImGuiCond_Always);
  ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
#endif
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  ImGui::NewLine();
  if (ImGui::Button(ICON_KI_STAR u8"    Вернуться в игру")) {
    GetAudio().PlaySound("selection", true);
    GetEngine().GrabCursor(true);
    GetEngine().OnMenuOff();
    ShowContextMenu = false;
  }
#endif
  ImGui::NewLine();
  if (ImGui::Button(ICON_KI_STAR u8"    Главное меню")) {
    GetAudio().PlaySound("selection", true);
    ChangeState(make_unique<MenuAppState>());
  }
  ImGui::NewLine();
  if (ImGui::Button(ICON_KI_STAR u8"    Выйти из игры")) {
    GetAudio().PlaySound("selection", true);
    ChangeState();
  }
  ImGui::NewLine();
  ImGui::End();
  ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Always);
  ImGui::SetNextWindowSize({0, 0}, ImGuiCond_Always);
  ImGui::SetNextWindowCollapsed(false, ImGuiCond_Always);
  ImGui::SetNextWindowBgAlpha(0.5);
  ImGui::Begin("FPS", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0 / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  ImGui::End();
}

void DemoDotAppState::Update(float time) { DrawMenu(); }

void DemoDotAppState::Pause() {}

void DemoDotAppState::Resume() {}

void DemoDotAppState::OnKeyDown(SDL_Keycode sym) {
  if (SDL_GetScancodeFromKey(sym) == SDL_SCANCODE_ESCAPE) {
    ShowContextMenu = !ShowContextMenu;
    if (ShowContextMenu) {
      GetEngine().OnMenuOn();
    } else {
      GetEngine().OnMenuOff();
    }
  }
}

void DemoDotAppState::Cleanup() {}

}  // namespace Demo
