/// created by Andrey Vasiliev

#include "pch.h"
#include "Menu.h"
#include "ImStyles.h"
#include "SystemLocator.h"

using namespace std;

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
#ifdef MOBILE
  int x = GetComponent<VideoManager>().GetWindow().GetDisplaySizeX();
  int y = GetComponent<VideoManager>().GetWindow().GetDisplaySizeY();
  GetComponent<CompositorManager>().SetFixedViewportSize(x / 1.5, y / 1.5);
  GetComponent<CompositorManager>().SetCompositorEnabled("Bloom", true);
  GetComponent<CompositorManager>().SetCompositorEnabled("Blur", true);
  GetComponent<CompositorManager>().SetCompositorEnabled("SSAO", true);
#endif
  GetComponent<SceneManager>().LoadFromFile("1.scene");
  GetComponent<SkyManager>().SetUpSky();
  //auto *font = GetComponent<VideoManager>().AddFont("Muse700");
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

#ifdef MOBILE
  ImGui::SetNextWindowPos(ImVec2(ImGetWidth() * 0.1, ImGetHeight() * 0.1), ImGuiCond_Always);
  ImGui::Begin("WindowOpenMenu", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
  if (ImGui::Button("OpenMenu", ImVec2(180, 30))) {
    showMenu = true;
  }
  ImGui::End();
#endif

  static VideoManager &manager = GetComponent<VideoManager>();
  static SystemLocator &system = GetComponent<SystemLocator>();
  static Window &window = manager.GetWindow();

  if (!showMenu) {
    window.SetMouseRelativeMode(true);
    return;
  } else {
    window.SetMouseRelativeMode(false);
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

  static bool windowFlags[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  static int combos[10] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0};

#ifndef ANDROID
  if (ImGui::Checkbox("Fullscreen", &windowFlags[0])) {
    window.SetFullscreen(windowFlags[0]);
  }

  if (ImGui::Checkbox("Resizable", &windowFlags[1])) {
    window.SetResizable(windowFlags[1]);
  }

  if (ImGui::Checkbox("Maximize", &windowFlags[2])) {
    window.SetResizable(true);

    std::this_thread::sleep_for(100ms);

    if (windowFlags[2]) {
      window.SetMaximized();
      window.SetRefresh();
    } else {
      window.SetRestored();
      window.SetPrevSize();
      window.SetPositionCentered();
    }

    window.SetResizable(windowFlags[1]);
  }

  if (ImGui::Checkbox("Always on top", &windowFlags[3])) {
    window.SetAlwaysOnTop(windowFlags[3]);
  }

  if (ImGui::Checkbox("Hide border", &windowFlags[4])) {
    window.SetBordered(!windowFlags[4]);
  }

  if (ImGui::Checkbox("Grab mouse", &windowFlags[5])) {
    window.SetGrabMouse(windowFlags[5]);
  }

  if (ImGui::Checkbox("Hide mouse", &windowFlags[6])) {
    window.SetMouseRelativeMode(windowFlags[6]);
  }

  if (ImGui::Checkbox("Enable shadows", &windowFlags[7])) {
    manager.EnableShadows(windowFlags[7]);
  }

  const char *intervalList = " -\0 1\0 2\0 3\0 4\0";
  if (ImGui::Combo("Vsync interval", &combos[0], intervalList)) {
    if (combos[0] == 0) {
      window.EnableVsync(false);
      combos[1] = 0;
    } else {
      window.EnableVsync(true);
      window.SetVsyncInterval(combos[0]);
    }
  }

  const char *fpsList = " -\0 60\0 30\0 20\0 15\0";
  if (ImGui::Combo("Lock FPS", &combos[1], fpsList)) {
    switch (combos[1]) {
      case (0): {
        system.EnableFpsLock(false);
        break;
      }
      case (1): {
        system.EnableFpsLock(true);
        system.SetFpsFreq(60);
        break;
      }
      case (2): {
        system.EnableFpsLock(true);
        system.SetFpsFreq(30);
        break;
      }
      case (3): {
        system.EnableFpsLock(true);
        system.SetFpsFreq(20);
        break;
      }
      case (4): {
        system.EnableFpsLock(true);
        system.SetFpsFreq(15);
        break;
      }
    }
  }

  const unsigned short texSizeList[] = {128, 256, 512, 1024, 2048, 4096};
  const char *texSizeListStr = " 128\0 256\0 512\0 1024\0 2048\0 4096\0";
  if (ImGui::Combo("Shadow tex size", &combos[2], texSizeListStr)) {
    manager.SetShadowTexSize(texSizeList[combos[2]]);
  }

  const unsigned short anisotropyLevel[] = {1, 2, 4, 8, 16};
  const char *anisotropyLevelStr = " 1\0 2\0 4\0 8\0 16\0";
  if (ImGui::Combo("Anisotropy lvl", &combos[3], anisotropyLevelStr)) {
    manager.SetTexFiltering(anisotropyLevel[combos[4]], combos[3]);
  }

  const unsigned short texFiltering[] = {0, 1, 2, 3};
  const char *texFilteringStr = " NONE\0 BILINEAR\0 TRILINEAR\0 ANISOTROPIC\0";
  if (ImGui::Combo("Texture filtering", &combos[4], texFilteringStr)) {
    manager.SetTexFiltering(texFiltering[combos[4]], combos[3]);
  }


#endif

  // used to generate display list
  SDL_DisplayMode displayMode;
  std::string displays;
  static int index = window.GetDisplay();
  for (int i = 0; i < SDL_GetNumVideoDisplays(); i++) {
    if (!SDL_GetCurrentDisplayMode(i, &displayMode)) {
      std::string buff = " #" + std::to_string(i) + ": " + SDL_GetDisplayName(i) + " " + std::to_string(displayMode.w) + "x" +
                         std::to_string(displayMode.h) + " @" + std::to_string(displayMode.refresh_rate);

      displays += buff;
      displays += '\0';
    }
  }

  if (ImGui::Combo("DisplayList", &index, displays.c_str())) {
    window.SetDisplay(index);
  }

  struct ResolutionItem {
    const char *item;
    int x;
    int y;
  };

  // resolution list
  constexpr ResolutionItem resList[] = {
      {"640x360", 640, 360},     {"800x600", 800, 600},     {"1024x768", 1024, 768},   {"1280x720", 1280, 720},   {"1280x800", 1280, 800},
      {"1280x1024", 1280, 1024}, {"1360x768", 1360, 768},   {"1366x768", 1366, 768},   {"1440x900", 1440, 900},   {"1536x864", 1536, 864},
      {"1600x900", 1600, 900},   {"1680x1050", 1680, 1050}, {"1920x1080", 1920, 1080}, {"1920x1200", 1920, 1200}, {"2048x1152", 2048, 1152},
      {"2048x1536", 2048, 1536}, {"2560x1080", 2560, 1080}, {"2560x1440", 2560, 1440}, {"2560x1600", 2560, 1600}, {"3440x1440", 3440, 1440},
      {"3840x2160", 3840, 2160},
  };

  // used to generate resolution string
  int sizeX = window.GetDisplaySizeX();
  int sizeY = window.GetDisplaySizeY();
  std::string resStr = std::string("  ") + '\0';
  resStr += std::to_string(sizeX) + "x" + std::to_string(sizeY) + " (native)" + '\0';
  for (const auto &it : resList) {
    if (sizeX >= it.x && sizeY >= it.y) {
      resStr += it.item;
      resStr += '\0';
    } else {
      break;
    }
  }

  static int j = 0;
  if (ImGui::Combo("Resolution", &j, resStr.c_str())) {
    int x, y;
    if (j > 0) {
      if (j == 1) {
        x = sizeX;
        y = sizeY;
      } else {
        x = resList[j - 2].x;
        y = resList[j - 2].y;
      }

      if (windowFlags[0]) {
        GetComponent<CompositorManager>().SetFixedViewportSize(x, y);
      } else {
        window.SetSize(x, y);
        window.SetPositionCentered();
      }
    }
  }

  static bool compositorFlags[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  if (ImGui::Checkbox("Bloom", &compositorFlags[0])) {
    GetComponent<CompositorManager>().SetCompositorEnabled("Bloom", compositorFlags[0]);
  }

  if (ImGui::Checkbox("Blur", &compositorFlags[1])) {
    GetComponent<CompositorManager>().SetCompositorEnabled("Blur", compositorFlags[1]);
  }

  if (ImGui::Checkbox("SSAO", &compositorFlags[2])) {
    GetComponent<CompositorManager>().SetCompositorEnabled("SSAO", compositorFlags[2]);
  }

  if (ImGui::Checkbox("FXAA", &compositorFlags[3])) {
    GetComponent<CompositorManager>().SetCompositorEnabled("FXAA", compositorFlags[3]);
  }

  ImGui::EndChild();

  //  ImGui::BeginChild("ScrollingRegion2", ImVec2(ImGetWidth() * 0.5, ImGetHeight()), false);
  //  ImGui::EndChild();

  ImGui::End();
}
}  // namespace Glue
