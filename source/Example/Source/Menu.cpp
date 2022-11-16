/// created by Andrey Vasiliev

#include "pch.h"
#include "Menu.h"
#include "ImStyles.h"
#include "SystemLocator.h"

namespace {
bool showMenu = false;
}  // namespace

namespace Glue {
void Menu::OnClean() { GetComponent<SystemLocator>().OnClean(); }

void Menu::OnKeyEvent(SDL_Scancode key, bool pressed) {
  if (key == SDL_SCANCODE_ESCAPE && pressed) {
    showMenu = !showMenu;
  }
}

void Menu::OnSetUp() {
#ifdef ANDROID
  GetComponent<CompositorManager>().SetFixedViewportSize(1024, 768);
#endif
  GetComponent<SceneManager>().LoadFromFile("1.scene");
  GetComponent<SkyManager>().SetUpSky();
  // auto *font = GetComponent<VideoManager>().AddFont("NotoSans-Regular");
  GetComponent<VideoManager>().ShowOverlay(true);
  ImGuiB::SetupImGuiStyle_DiscordDark();
}

void Menu::OnUpdate(float time) {}

void Menu::BeforeRender(float time) {
  const static ImGuiIO &io = ImGui::GetIO();
  const static auto *viewport = ImGui::GetMainViewport();

  Ogre::ImGuiOverlay::NewFrame();

  // fps counter
  ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Always);
  ImGui::Begin("FPS", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);
  ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0 / io.Framerate, io.Framerate);
  ImGui::End();

  ImGui::SetNextWindowPos(ImVec2(ImGetWidth() * 0.1, ImGetHeight() * 0.1), ImGuiCond_Always);
  ImGui::Begin("OpenMenu", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
  if (ImGuiB::TabButton("OpenMenu", !showMenu)) {
    showMenu = true;
  }
  ImGui::End();

  if (!showMenu) {
    GetComponent<VideoManager>().GetWindow().SetMouseRelativeMode(true);
    return;
  } else {
    GetComponent<VideoManager>().GetWindow().SetMouseRelativeMode(false);
  }

  ImGuiB::SetupImGuiStyle_NeverBlue();

  float vx = viewport->Size.x, vy = viewport->Size.y;
  float scale = 0.95;
  float border = 0.5 - 0.5 * scale;

  ImGui::SetNextWindowPos({border * vx, border * vy});
  ImGui::SetNextWindowSize({scale * vx, scale * vy});
  ImGui::Begin("Settings", 0, ImGuiWindowFlags_NoDecoration);

  static int activeTab = 0;
  std::vector<std::string> tabs{"Window", "Errors", "Info", "AnotherTab"};

  const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

  ImGuiB::DrawTabHorizontally("Settings", ImVec2(ImGetWidth(), ImGetHeight() * 0.065), tabs, activeTab);
  ImGui::BeginChild("ScrollingRegion", ImVec2(ImGetWidth() * 0.5, ImGetHeight()), false);

  static bool windowFlags[8] = {0, 0, 0, 1, 1, 0, 0, 0};
  static int Combos[2];
  static int Slider = 0;

  if (ImGuiB::Checkbox("Fullscreen", &windowFlags[0])) {
    GetComponent<VideoManager>().GetWindow().SetFullscreen(windowFlags[0]);
  }

  if (ImGuiB::Checkbox("Resizable", &windowFlags[1])) {
    GetComponent<VideoManager>().GetWindow().SetResizable(windowFlags[1]);
  }

  if (ImGuiB::Checkbox("Maximize", &windowFlags[2])) {
    windowFlags[2] ? GetComponent<VideoManager>().GetWindow().SetMaximized() : GetComponent<VideoManager>().GetWindow().SetRestored();
  }

  if (ImGuiB::Checkbox("Always on top", &windowFlags[3])) {
    GetComponent<VideoManager>().GetWindow().SetAlwaysOnTop(windowFlags[3]);
  }

  if (ImGuiB::Checkbox("Show border", &windowFlags[4])) {
    GetComponent<VideoManager>().GetWindow().SetBordered(windowFlags[4]);
  }

  if (ImGuiB::Checkbox("Grab mouse", &windowFlags[5])) {
    GetComponent<VideoManager>().GetWindow().SetGrabMouse(windowFlags[5]);
  }

  if (ImGuiB::Checkbox("Hide mouse", &windowFlags[6])) {
    GetComponent<VideoManager>().GetWindow().SetMouseRelativeMode(windowFlags[6]);
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

  // used to generate resolution string
  // std::string str = " ";
  // for (const auto &it : resList) {
  //  str += it.item;
  //  str += "\\0 ";
  //}
  // printf("%s\n", str.c_str());

  // used to generate display list
  SDL_DisplayMode displayMode;
  std::string displays;
  static int index = 0;
  for (unsigned int i = 0; i < SDL_GetNumVideoDisplays(); i++) {
    if (!SDL_GetCurrentDisplayMode(i, &displayMode)) {
      std::string buff = "Display #" + std::to_string(i) + ": " + SDL_GetDisplayName(i) + " " + std::to_string(displayMode.w) + "x" +
                         std::to_string(displayMode.h) + " @" + std::to_string(displayMode.refresh_rate);

      displays += buff;
      displays += '\0';
    }
  }

  if (ImGuiB::Combo("DisplayList", &index, displays.c_str())) {
    GetComponent<VideoManager>().GetWindow().SetDisplay(index);
  }

  constexpr const char *resStr =
      " 360x800\0 800x600\0 810x1080\0 768x1024\0 834x1112\0 1024x768\0 1024x1366\0 1280x720\0 1280x800\0 1280x1024\0 1360x768\0 1366x768\0 "
      "1440x900\0 1536x864\0 1600x900\0 1680x1050\0 1920x1080\0 1920x1200\0 2048x1152\0 2560x1440\0";

  static int j = 0;
  if (ImGuiB::Combo("Resolution", &j, resStr)) {
    if (windowFlags[0]) {
      GetComponent<CompositorManager>().SetFixedViewportSize(resList[j].x, resList[j].y);
    } else {
      GetComponent<VideoManager>().GetWindow().SetSize(resList[j].x, resList[j].y);
    }
  }

  static bool compositorFlags[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  if (ImGuiB::Checkbox("Bloom", &compositorFlags[0])) {
    GetComponent<CompositorManager>().SetCompositorEnabled("Bloom", compositorFlags[0]);
  }

  if (ImGuiB::Checkbox("Blur", &compositorFlags[1])) {
    GetComponent<CompositorManager>().SetCompositorEnabled("Blur", compositorFlags[1]);
  }

  if (ImGuiB::Checkbox("SSAO", &compositorFlags[2])) {
    GetComponent<CompositorManager>().SetCompositorEnabled("SSAO", compositorFlags[2]);
  }

  if (ImGuiB::Checkbox("FXAA", &compositorFlags[3])) {
    GetComponent<CompositorManager>().SetCompositorEnabled("FXAA", compositorFlags[3]);
  }

  ImGui::EndChild();

  ImGui::BeginChild("ScrollingRegion2", ImVec2(ImGetWidth() * 0.5, ImGetHeight()), false);
  ImGui::EndChild();

  ImGui::End();
}
}  // namespace Glue
