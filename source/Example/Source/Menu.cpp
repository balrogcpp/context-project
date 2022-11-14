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
  ImGui::SetNextWindowBgAlpha(0.1);
  ImGui::Begin("Settings", 0, ImGuiWindowFlags_NoDecoration);

  static int ActiveTab = 0;
  std::vector<std::string> types{"Window", "Errors", "Info", "AnotherTab"};

  const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

  ImGui::SetNextWindowBgAlpha(0.0);
  //ImGui::BeginChild("ScrollingRegion", ImVec2(0, -0), true);
  ImGui::BeginChild("ScrollingRegion", ImVec2(0, -0), false);

  static bool Checkboxes[8] = {0, 0, 0, 1, 1, 0, 0, 0};
  static int Combos[2];
  static int Slider = 0;

  ImGuiB::SetupImGuiStyle_NeverBlue();

  if (ImGuiB::Checkbox("Fullscreen", &Checkboxes[0])) {
    GetComponent<VideoManager>().GetWindow().SetFullscreen(Checkboxes[0]);
  }

  if (ImGuiB::Checkbox("Resizable", &Checkboxes[1])) {
    GetComponent<VideoManager>().GetWindow().SetResizable(Checkboxes[1]);
  }

  if (ImGuiB::Checkbox("Always on top", &Checkboxes[2])) {
    GetComponent<VideoManager>().GetWindow().SetAlwaysOnTop(Checkboxes[2]);
  }

  if (ImGuiB::Checkbox("Show border", &Checkboxes[3])) {
    GetComponent<VideoManager>().GetWindow().SetBordered(Checkboxes[3]);
  }

  if (ImGuiB::Checkbox("Grab mouse", &Checkboxes[4])) {
    GetComponent<VideoManager>().GetWindow().SetGrabMouse(Checkboxes[4]);
  }

  struct ResolutionItem {
    const char *item;
    int x;
    int y;
  };

  constexpr ResolutionItem resList[] = {
      {"360x800", 360, 800},     {"800x600", 800, 600},     {"810x1080", 810, 1080},   {"768x1024", 768, 1024},   {"834x1112", 834, 1112},
      {"1024x768", 1024, 768},   {"1024x1366", 1024, 1366}, {"1280x720", 1280, 720},   {"1280x800", 1280, 800},   {"1280x1024", 1280, 1024},
      {"1360x768", 1360, 768},   {"1366x768", 1366, 768},   {"1440x900", 1440, 900},   {"1536x864", 1536, 864},   {"1600x900", 1600, 900},
      {"1680x1050", 1680, 1050}, {"1920x1080", 1920, 1080}, {"1920x1200", 1920, 1200}, {"2048x1152", 2048, 1152}, {"2560x1440", 2560, 1440},
  };

  // used to generate string
  //std::string str = " ";
  //for (const auto &it : resList) {
  //  str += it.item;
  //  str += "\\0 ";
  //}
  //printf("%s\n", str.c_str());

  constexpr const char *resStr =
      " 360x800\0 800x600\0 810x1080\0 768x1024\0 834x1112\0 1024x768\0 1024x1366\0 1280x720\0 1280x800\0 1280x1024\0 1360x768\0 1366x768\0 " \
      "1440x900\0 1536x864\0 1600x900\0 1680x1050\0 1920x1080\0 1920x1200\0 2048x1152\0 2560x1440\0";

  static int i = 0;
  if (ImGuiB::Combo("Resolution", &i, resStr)) {
    if (Checkboxes[0]) {
      GetComponent<CompositorManager>().SetFixedViewportSize(resList[i].x, resList[i].y);
    } else {
      GetComponent<VideoManager>().GetWindow().SetSize(resList[i].x, resList[i].y);
    }
  }

  ImGui::EndChild();

  ImGui::End();

  // fps counter
  ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Always);
  ImGui::Begin("FPS", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs);
  ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0 / io.Framerate, io.Framerate);
  ImGui::End();
}
}  // namespace Glue
