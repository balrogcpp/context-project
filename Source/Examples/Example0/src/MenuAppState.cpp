// This source file is part of "glue project". Created by Andrey Vasiliev

#include "MenuAppState.h"
#include "DemoDotAppState.h"
#include "Engine.h"
#include "PCHeader.h"

using namespace std;
using namespace Glue;

namespace Demo {

void MenuAppState::Cleanup() {}

void MenuAppState::Update(float time) {
  Ogre::ImGuiOverlay::NewFrame();

  {
    static ImGuiIO &io = ImGui::GetIO();

    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize({0, 0}, ImGuiCond_Always);
    ImGui::SetNextWindowCollapsed(false, ImGuiCond_Always);
    ImGui::SetNextWindowFocus();

    ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

    const float hdx = 1920;
    const float hdy = 1080;
    const float hddiag = sqrt(hdx * hdx + hdy * hdy);
    float x = GetEngine().GetWindowSize().first;
    float y = GetEngine().GetWindowSize().second;
    static float diag = sqrt(x * x + y * y);
    float scale = 0.5f * diag / hddiag;
    ImGui::SetWindowFontScale(scale);

    ImGui::NewLine();

    if (ImGui::Button("        DEMO        ")) {
      ChangeState(make_unique<DemoDotAppState>());
    }

    ImGui::NewLine();

    if (ImGui::Button("        EXIT           ")) {
      ChangeState();
    }

    ImGui::NewLine();

    ImGui::End();
  }
}

void MenuAppState::OnKeyDown(SDL_Keycode sym) {}

void MenuAppState::SetUp() { GetEngine().GrabMouse(false); }

}  // namespace Demo
