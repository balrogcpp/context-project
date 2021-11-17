// This source file is part of "glue project"
// Created by Andrew Vasiliev

#include "pcheader.h"
#include "MenuAppState.h"
#include "ComponentLocator.h"
#include "DemoDotAppState.h"

using namespace std;
using namespace glue;

namespace Demo {

//----------------------------------------------------------------------------------------------------------------------
void MenuAppState::Cleanup() {}

//----------------------------------------------------------------------------------------------------------------------
void MenuAppState::Update(float time) {
  Ogre::ImGuiOverlay::NewFrame();

  {
    static ImGuiIO &io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always,
                            ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize({0, 0}, ImGuiCond_Always);
    ImGui::SetNextWindowCollapsed(false, ImGuiCond_Always);
    ImGui::SetNextWindowFocus();

    ImGui::Begin("", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

    const float hdx = 1920;
    const float hdy = 1080;
    const float hddiag = sqrt(hdx * hdx + hdy * hdy);
    float x = GetWindow().GetSize().first;
    float y = GetWindow().GetSize().second;
    static float diag = sqrt(x * x + y * y);
    float scale = 0.5f * diag / hddiag;
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
//	scale *= 2.0f;
#endif
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

//----------------------------------------------------------------------------------------------------------------------
void MenuAppState::OnKeyDown(SDL_Keycode sym) {}

//----------------------------------------------------------------------------------------------------------------------
void MenuAppState::SetUp() { GetWindow().Grab(false); }

}  // namespace Demo
