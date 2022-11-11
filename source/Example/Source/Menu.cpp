/// created by Andrey Vasiliev

#include "pch.h"
#include "Menu.h"
#include "ImStyles.h"
#include "SystemLocator.h"

namespace {}  // namespace

namespace Glue {
void Menu::OnClean() { GetComponent<SystemLocator>().OnClean(); }

void Menu::OnSetUp() {
  // GetComponent<VideoManager>().GetWindow().SetMouseRelativeMode(true);
  // GetComponent<VideoManager>().GetWindow().SetFullscreen(true);
#ifdef ANDROID
  GetComponent<CompositorManager>().SetFixedViewportSize(1024, 768);
#endif
  GetComponent<SceneManager>().LoadFromFile("1.scene");
  GetComponent<SkyManager>().SetUpSky();
  // GetComponent<CompositorManager>().SetCompositorEnabled("Blur", true);
  // auto *font = GetComponent<VideoManager>().AddFont("NotoSans-Regular");
  GetComponent<VideoManager>().ShowOverlay(true);
  ImStyle::SetupImGuiStyle_RedCustom();
}

void Menu::OnUpdate(float time) {
  const static ImGuiIO &io = ImGui::GetIO();
  const static auto *viewport = ImGui::GetMainViewport();

  Ogre::ImGuiOverlay::NewFrame();

  float vx = viewport->Size.x, vy = viewport->Size.y;
  float scale = 0.95;
  float border = 0.5 - 0.5 * scale;

  ImGui::SetNextWindowPos({border * vx, border * vy});
  ImGui::SetNextWindowSize({scale * vx, scale * vy});
  ImGui::SetNextWindowBgAlpha(0.2);
  ImGui::Begin("Settings", 0, ImGuiWindowFlags_NoDecoration);

  static int tab;
  std::vector<std::string> types{" Window", " Errors", " Info", "AnotherTab"};

  DrawTabHorizontally("Styles", ImVec2(ImGetWidth(), 50), types, tab);

  const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
  ImGui::BeginChild("ScrollingRegion", ImVec2(0, -0), true, ImGuiWindowFlags_NoInputs);
  ImGui::EndChild();

  ImGui::End();

  // fps counter
  ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Always);
  ImGui::Begin("FPS", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs);
  ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0 / io.Framerate, io.Framerate);
  ImGui::End();
}
}  // namespace Glue
