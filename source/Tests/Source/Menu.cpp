/// created by Andrey Vasiliev

#include "pch.h"
#include "Menu.h"
#include "ImStyles.h"
#include "SystemLocator.h"

using namespace std;

namespace {
bool showMenu = false;
ImFont *font = nullptr;
}  // namespace

namespace gge {
void Menu::OnClean() { GetComponent<SystemLocator>().OnClean(); }

void Menu::OnKeyEvent(SDL_Scancode key, bool pressed) {
  if (pressed && key == SDL_SCANCODE_ESCAPE) {
    showMenu = !showMenu;
  }
}

void Menu::OnSetUp() {
#ifdef MOBILE
  int x = GetComponent<VideoManager>().GetWindow().GetDisplaySizeX();
  int y = GetComponent<VideoManager>().GetWindow().GetDisplaySizeY();
  GetComponent<CompositorManager>().SetFixedViewportSize(x / 1.5, y / 1.5);
#else
  int x = GetComponent<VideoManager>().GetWindow().GetSizeX();
  int y = GetComponent<VideoManager>().GetWindow().GetSizeY();
#endif
  float dpi = GetComponent<VideoManager>().GetWindow().GetDisplayDPI();

  ImGuiIO &io = ImGui::GetIO();
  static ImGuiStyle &style = ImGui::GetStyle();
  // ImGuiB::SetupImGuiStyle_Unreal();
  ImGuiB::SetupImGuiStyle_SpectrumDark();

//  GetComponent<SceneManager>().LoadFromFile("bath.scene");
  //GetComponent<SceneManager>().LoadFromFile("metallic_showroom_gallery.scene");
//  GetComponent<SceneManager>().LoadFromFile("throne_room.scene");
  GetComponent<SceneManager>().LoadFromFile("1.scene");
  GetComponent<SkyManager>().SetUpSky();

  float diag0 = 100000.0;
  float diag = min(x, y);
  float scale = diag / diag0;
  scale *= dpi;
#ifdef MOBILE
  //  scale *= (7.0 / 8.0);
  scale *= 0.5;
#endif

  style.ScaleAllSizes(scale);
  io.Fonts->AddFontDefault();
  font = GetComponent<VideoManager>().AddOverlayFont("Muse500", floor(34 * scale));

  GetComponent<VideoManager>().ShowOverlay(true);
}

void Menu::OnUpdate(float time) {}

void Menu::OnSizeChanged(int x, int y, uint32_t id) {
  if (x <= 0 || y <= 0) {
    return;
  }

  ImGuiIO &io = ImGui::GetIO();
  static ImGuiStyle &style = ImGui::GetStyle();
  float diag0 = 100000.0;
  float diag = min(x, y);
  float dpi = GetComponent<VideoManager>().GetWindow().GetDisplayDPI();
  float scale = diag / diag0;
  scale *= dpi;
#ifdef MOBILE
  scale *= 0.35;
#endif

  //  Setup Dear ImGui style
  ImGuiStyle styleold = style;  // Backup colors
  style = ImGuiStyle();         // IMPORTANT: ScaleAllSizes will change the original size, so we should reset all style config
  style.WindowBorderSize = 1.0;
  style.ChildBorderSize = 1.0;
  style.PopupBorderSize = 1.0;
  style.FrameBorderSize = 1.0;
  style.TabBorderSize = 1.0;
  style.WindowRounding = 0.0;
  style.ChildRounding = 0.0;
  style.PopupRounding = 0.0;
  style.FrameRounding = 0.0;
  style.ScrollbarRounding = 0.0;
  style.GrabRounding = 0.0;
  style.TabRounding = 0.0;
  style.ScaleAllSizes(scale);
  memcpy(style.Colors, styleold.Colors, sizeof(style.Colors));  // Restore colors

  io.Fonts->Clear();
  io.Fonts->AddFontDefault();
  font = GetComponent<VideoManager>().AddOverlayFont("Muse500", floor(34 * scale));
  GetComponent<VideoManager>().RebuildOverlayFontAtlas();
}

void Menu::BeforeRender(float time) {
  ImGuiIO &io = ImGui::GetIO();
  static ImGuiStyle &style = ImGui::GetStyle();
  static auto *viewport = ImGui::GetMainViewport();
  static VideoManager &manager = GetComponent<VideoManager>();
  static SystemLocator &system = GetComponent<SystemLocator>();
  static Window &window = manager.GetWindow();

  Ogre::ImGuiOverlay::NewFrame();

  float vx = viewport->Size.x, vy = viewport->Size.y;
  float scale = 7.0 / 8.0;
  float border = 0.5 - 0.5 * scale;

  // fps counter
  ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Always);
  ImGui::Begin("FPS", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);
  ImGui::PushFont(nullptr);
  ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
  ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0 / io.Framerate, io.Framerate);
  ImGui::PopStyleColor();
  ImGui::PopFont();
  ImGui::End();

#ifdef MOBILE
  ImGui::SetNextWindowPos({0.0f, 0.1f * vy}, ImGuiCond_Always);
  ImGui::SetNextWindowSize({vx * 0.15f, vy * 0.15f});
  ImGui::Begin("WindowOpenMenu", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
  if (ImGui::Button("MENU", {vx * 0.2f, vy * 0.2f})) showMenu = !showMenu;
  ImGui::End();
#endif

  if (!showMenu) {
    window.SetMouseRelativeMode(true);
    GetComponent<SystemLocator>().SetSleep(false);
    return;
  } else {
    window.SetMouseRelativeMode(false);
    GetComponent<SystemLocator>().SetSleep(true);
  }

  ImGui::SetNextWindowPos({border * vx, border * vy});
  ImGui::SetNextWindowSize({scale * vx, scale * vy});
  ImGui::SetNextWindowBgAlpha(0.8);
  ImGui::Begin("Settings", 0, ImGuiWindowFlags_NoDecoration);

  ImGui::PushFont(font);

  // ImGuiB::SetupImGuiStyle_NeverBlue();
  // static int activeTab = 0;
  // std::vector<std::string> tabs{"Window", "Errors", "Info", "AnotherTab"};
  // const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
  // ImGuiB::DrawTabHorizontally("Settings", ImVec2(ImGetWidth(), ImGui::GetFontSize() * 4.0), tabs, activeTab);

  ImGui::BeginChild("ScrollingRegion1", ImVec2(ImGetWidth(), ImGetHeight()), false);

  static bool flags[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  static int combos[10] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0};

#ifdef ANDROID
  flags[0] = 1;
#endif

#ifndef ANDROID
  if (ImGui::Checkbox("Fullscreen", &flags[0])) {
    window.SetFullscreen(flags[0]);
  }

  if (ImGui::Checkbox("Resizable", &flags[1])) {
    window.SetResizable(flags[1]);
  }

  if (ImGui::Checkbox("Maximize", &flags[2])) {
    window.SetResizable(true);
    std::this_thread::sleep_for(100ms);

    if (flags[2]) {
      window.SetMaximized();
      window.SetRefresh();
    } else {
      window.SetRestored();
      window.SetPrevSize();
      window.SetPositionCentered();
    }

    window.SetResizable(flags[1]);
  }

  if (ImGui::Checkbox("Always on top", &flags[3])) {
    window.SetAlwaysOnTop(flags[3]);
  }

  if (ImGui::Checkbox("Hide border", &flags[4])) {
    window.SetBordered(!flags[4]);
  }

  if (ImGui::Checkbox("Grab mouse", &flags[5])) {
    window.SetGrabMouse(flags[5]);
  }

  if (ImGui::Checkbox("Hide mouse", &flags[6])) {
    window.SetMouseRelativeMode(flags[6]);
  }

  if (ImGui::Checkbox("Enable shadows", &flags[7])) {
    manager.EnableShadows(flags[7]);
  }

  if (ImGui::Checkbox("Enable profiler", &flags[8])) {
    Ogre::Profiler::getSingleton().setEnabled(flags[8]);
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

  const unsigned short texSizeList[] = {128, 256, 512, 1024, 2048, 4096, 8192};
  const char *texSizeListStr = " 128\0 256\0 512\0 1024\0 2048\0 4096\0 8192\0";
  if (ImGui::Combo("Shadow tex size", &combos[2], texSizeListStr)) {
    manager.SetShadowTexSize(texSizeList[combos[2]]);
  }

  const unsigned short anisotropyLevel[] = {1, 2, 4, 8, 16};
  const char *anisotropyLevelStr = " 1\0 2\0 4\0 8\0 16\0";
  if (ImGui::Combo("Anisotropy lvl", &combos[3], anisotropyLevelStr)) {
    manager.SetTexFiltering(anisotropyLevel[combos[4]], combos[3]);
  }

  const unsigned short texFiltering[] = {0, 1, 2, 3};
  const char *texFilteringStr = " None\0 Bilinear\0 Trilinear\0 Anisotropic\0";
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
  std::string resStr = std::string(" -") + '\0';
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

      if (flags[0]) {
        GetComponent<CompositorManager>().SetFixedViewportSize(x, y);
      } else {
        window.SetSize(x, y);
        window.SetPositionCentered();
      }
    }
  }

  static bool compositorFlags[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  if (ImGui::Checkbox("Glow", &compositorFlags[0])) {
    GetComponent<CompositorManager>().EnableCompositor("Glow", compositorFlags[0]);
  }

  if (ImGui::Checkbox("Blur", &compositorFlags[1])) {
    GetComponent<CompositorManager>().EnableCompositor("Blur", compositorFlags[1]);
  }

  if (ImGui::Checkbox("SSAO", &compositorFlags[2])) {
    GetComponent<CompositorManager>().EnableCompositor("SSAO", compositorFlags[2]);
  }

  if (ImGui::Checkbox("FXAA", &compositorFlags[3])) {
    GetComponent<CompositorManager>().EnableCompositor("FXAA", compositorFlags[3]);
  }

  if (ImGui::Checkbox("SMAA", &compositorFlags[4])) {
    GetComponent<CompositorManager>().EnableCompositor("SMAA", compositorFlags[4]);
  }

  if (ImGui::Checkbox("SSR", &compositorFlags[5])) {
    GetComponent<CompositorManager>().EnableCompositor("SSR", compositorFlags[5]);
  }

  ImGui::EndChild();
  ImGui::PopFont();

  ImGui::End();
}
}  // namespace gge
