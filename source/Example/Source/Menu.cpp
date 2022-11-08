/// created by Andrey Vasiliev

#include "pch.h"
#include "Menu.h"
#include "SystemLocator.h"
#include "imgui_user/ImStyles.h"

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

  auto *font = GetComponent<VideoManager>().AddFont("NotoSans-Regular");
  GetComponent<VideoManager>().ShowOverlay(true);
  // ImStyle::SetupImGuiStyle_DiscordDark();
  ImStyle::SetupImGuiStyle_GreenFont();
}

void Menu::OnUpdate(float time) {
  static ImGuiIO &io = ImGui::GetIO();
  Ogre::ImGuiOverlay::NewFrame();

  ImGui::ShowDemoWindow();
  ImGui::ShowMetricsWindow();

  // fps counter
  // ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Always);
  // ImGui::SetNextWindowSize({0, 0}, ImGuiCond_Always);
  // ImGui::SetNextWindowCollapsed(false, ImGuiCond_Always);
  // ImGui::SetNextWindowBgAlpha(0.5);
  // ImGui::Begin("FPS", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
  // ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0 / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  // ImGui::End();

  static bool use_work_area = true;
  // const ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
  const auto *viewport = ImGui::GetMainViewport();
  //  ImGui::SetNextWindowPos(use_work_area ? viewport->WorkPos : viewport->Pos);
  //  ImGui::SetNextWindowSize(use_work_area ? viewport->WorkSize : viewport->Size);
  //  ImGui::SetNextWindowBgAlpha(0.5);
  //  ImGui::Begin("Example: Fullscreen window", nullptr);
  //  ImGui::End();

  // ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5, io.DisplaySize.y * 0.5), ImGuiCond_Always, ImVec2(0.5, 0.5));
  // ImGui::SetNextWindowSize({0, 0}, ImGuiCond_Always);
  // ImGui::SetNextWindowBgAlpha(0.5);
  //  ImGui::SetNextWindowCollapsed(false, ImGuiCond_Always);
  //  ImGui::Begin("Menu");
  //  ImGui::BeginChild("Menu", nullptr);

  //  ImGui::NewLine();
  //  if (ImGui::Button(u8"Back to game")) {
  //  }

  //  ImGui::End();
  //  ImGui::End();

  //    ImGui::NewLine();
  //    if (ImGui::Button(u8"    Main Menu")) {
  //      //ChangeState(make_unique<MenuAppState>());
  //    }
  //    ImGui::NewLine();
  //    if (ImGui::Button(u8"    Quit game")) {
  //      //ChangeState();
  //    }
}
}  // namespace Glue
