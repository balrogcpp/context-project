// This source file is part of Glue Engine. Created by Andrey Vasiliev

#include "PCHeader.h"
#include "Components/Overlay.h"
#ifdef OGRE_BUILD_COMPONENT_OVERLAY
#include <Overlay/OgreImGuiOverlay.h>
#include <Overlay/OgreOverlay.h>
#include <Overlay/OgreOverlayManager.h>
#include <Overlay/OgreOverlaySystem.h>
#endif

using namespace std;

namespace Glue {

Overlay::Overlay(Ogre::RenderWindow* OgreRenderWindowPtr) {
  OgreRenderWindow = OgreRenderWindowPtr;
  Ogre::SceneManager* OgreScene = Ogre::Root::getSingletonPtr()->getSceneManager("Default");
  OgreOverlayPtr = new Ogre::OverlaySystem();
  OgreScene->addRenderQueueListener(OgreOverlayPtr);

  ImGuiOverlayPtr = new Ogre::ImGuiOverlay();

  Ogre::OverlayManager::getSingleton().addOverlay(ImGuiOverlayPtr);
  OgreRenderWindowPtr->addListener(this);

  ImGuiListener = make_unique<ImGuiInputListener>();

#ifndef DEBUG
  // Suppress ini file creation
  ImGuiIO& io = ImGui::GetIO();
  io.IniFilename = nullptr;
  io.LogFilename = nullptr;
#endif
}

Overlay::~Overlay() {}

void Overlay::preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt) {}

void Overlay::OnUpdate(float time) {}

void Overlay::OnClean() {}

void Overlay::OnPause() {}

void Overlay::OnResume() {}

void Overlay::OrangeStyle() {
  ImGuiStyle &style = ImGui::GetStyle();

  style.Colors[ImGuiCol_TitleBg] = ImColor(255, 101, 53, 255);
  style.Colors[ImGuiCol_TitleBgActive] = ImColor(255, 101, 53, 255);
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImColor(0, 0, 0, 130);

  style.Colors[ImGuiCol_Button] = ImColor(31, 30, 31, 255);
  style.Colors[ImGuiCol_ButtonActive] = ImColor(255, 101, 53, 255);
  style.Colors[ImGuiCol_ButtonHovered] = ImColor(255, 101, 53, 255);

  style.Colors[ImGuiCol_Separator] = ImColor(70, 70, 70, 255);
  style.Colors[ImGuiCol_SeparatorActive] = ImColor(76, 76, 76, 255);
  style.Colors[ImGuiCol_SeparatorHovered] = ImColor(76, 76, 76, 255);

  style.Colors[ImGuiCol_FrameBg] = ImColor(37, 36, 37, 255);
  style.Colors[ImGuiCol_FrameBgActive] = ImColor(37, 36, 37, 255);
  style.Colors[ImGuiCol_FrameBgHovered] = ImColor(37, 36, 37, 255);

  style.Colors[ImGuiCol_Header] = ImColor(0, 0, 0, 0);
  style.Colors[ImGuiCol_HeaderActive] = ImColor(0, 0, 0, 0);
  style.Colors[ImGuiCol_HeaderHovered] = ImColor(46, 46, 46, 255);

  return;
  bool bStyleDark_ = false;
  float alpha_ = 0.1;

  if (bStyleDark_) {
    for (int i = 0; i <= ImGuiCol_COUNT; i++) {
      ImVec4& col = style.Colors[i];
      float H, S, V;
      ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, H, S, V);

      if (S < 0.1f) {
        V = 1.0f - V;
      }
      ImGui::ColorConvertHSVtoRGB(H, S, V, col.x, col.y, col.z);
      if (col.w < 1.00f) {
        col.w *= alpha_;
      }
    }
  } else {
    for (int i = 0; i <= ImGuiCol_COUNT; i++) {
      ImVec4& col = style.Colors[i];
      if (col.w < 1.00f) {
        col.x *= alpha_;
        col.y *= alpha_;
        col.z *= alpha_;
        col.w *= alpha_;
      }
    }
  }

}

void Overlay::SetupImGuiStyle(bool bStyleDark_, float alpha_) {
  ImGuiStyle& style = ImGui::GetStyle();

  // light style from Pacôme Danhiez (user itamago) https://github.com/ocornut/imgui/pull/511#issuecomment-175719267
  style.Alpha = 1.0f;
  style.FrameRounding = 3.0f;
  style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
  style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
  //style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  style.Colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
  style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
  style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
  style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
  style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
  style.Colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
  style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
  style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
  style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
  style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
  //style.Colors[ImGuiCol_ComboBg] = ImVec4(0.86f, 0.86f, 0.86f, 0.99f);
  style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
  style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
  style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
  style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  //style.Colors[ImGuiCol_Column] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
  //style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
  //style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
  style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
  //style.Colors[ImGuiCol_CloseButton] = ImVec4(0.59f, 0.59f, 0.59f, 0.50f);
  //style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
  //style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
  style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
  style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
  //style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

  if (bStyleDark_) {
    for (int i = 0; i <= ImGuiCol_COUNT; i++) {
      ImVec4& col = style.Colors[i];
      float H, S, V;
      ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, H, S, V);

      if (S < 0.1f) {
        V = 1.0f - V;
      }
      ImGui::ColorConvertHSVtoRGB(H, S, V, col.x, col.y, col.z);
      if (col.w < 1.00f) {
        col.w *= alpha_;
      }
    }
  } else {
    for (int i = 0; i <= ImGuiCol_COUNT; i++) {
      ImVec4& col = style.Colors[i];
      if (col.w < 1.00f) {
        col.x *= alpha_;
        col.y *= alpha_;
        col.z *= alpha_;
        col.w *= alpha_;
      }
    }
  }
}


  // Helper to wire demo markers located in code to a interactive browser
  typedef void (*ImGuiDemoMarkerCallback)(const char* file, int line, const char* section, void* user_data);
extern ImGuiDemoMarkerCallback GImGuiDemoMarkerCallback;
extern void* GImGuiDemoMarkerCallbackUserData;
ImGuiDemoMarkerCallback GImGuiDemoMarkerCallback = NULL;
void* GImGuiDemoMarkerCallbackUserData = NULL;
#define IMGUI_DEMO_MARKER(section)                                                                                                 \
  do {                                                                                                                             \
    if (GImGuiDemoMarkerCallback != NULL) GImGuiDemoMarkerCallback(__FILE__, __LINE__, section, GImGuiDemoMarkerCallbackUserData); \
  } while (0)

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
static void HelpMarker(const char* desc) {
  ImGui::TextDisabled("(?)");
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}

void Overlay::DrawSimpleOverlay(bool* p_open) {
  static int corner = 0;
  ImGuiIO& io = ImGui::GetIO();
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
                                  ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
  if (corner != -1) {
    const float PAD = 10.0f;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 work_pos = viewport->WorkPos;  // Use work area to avoid menu-bar/task-bar, if any!
    ImVec2 work_size = viewport->WorkSize;
    ImVec2 window_pos, window_pos_pivot;
    window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
    window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
    window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
    window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    window_flags |= ImGuiWindowFlags_NoMove;
  }
  ImGui::SetNextWindowBgAlpha(0.35f);  // Transparent background
  if (ImGui::Begin("Example: Simple overlay", p_open, window_flags)) {
    IMGUI_DEMO_MARKER("Examples/Simple Overlay");
    ImGui::Text(
        "Simple overlay\n"
        "in the corner of the screen.\n"
        "(right-click to change position)");
    ImGui::Separator();
    if (ImGui::IsMousePosValid())
      ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
    else
      ImGui::Text("Mouse Position: <invalid>");
    if (ImGui::BeginPopupContextWindow()) {
      if (ImGui::MenuItem("Custom", NULL, corner == -1)) corner = -1;
      if (ImGui::MenuItem("Top-left", NULL, corner == 0)) corner = 0;
      if (ImGui::MenuItem("Top-right", NULL, corner == 1)) corner = 1;
      if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) corner = 2;
      if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
      if (p_open && ImGui::MenuItem("Close")) *p_open = false;
      ImGui::EndPopup();
    }
  }
  ImGui::End();
}

void Overlay::DrawAppFullscreen(bool* p_open) {
  static bool use_work_area = true;
  static ImGuiWindowFlags flags =
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;

  // We demonstrate using the full viewport area or the work area (without menu-bars, task-bars etc.)
  // Based on your use case you may want one of the other.
  const ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(use_work_area ? viewport->WorkPos : viewport->Pos);
  ImGui::SetNextWindowSize(use_work_area ? viewport->WorkSize : viewport->Size);

  if (ImGui::Begin("Example: Fullscreen window", p_open, flags)) {
    ImGui::Checkbox("Use work area instead of main area", &use_work_area);
    ImGui::SameLine();
    HelpMarker(
        "Main Area = entire viewport,\nWork Area = entire viewport minus sections used by the main menu bars, task bars etc.\n\nEnable the main-menu "
        "bar in Examples menu to see the difference.");

    ImGui::CheckboxFlags("ImGuiWindowFlags_NoBackground", &flags, ImGuiWindowFlags_NoBackground);
    ImGui::CheckboxFlags("ImGuiWindowFlags_NoDecoration", &flags, ImGuiWindowFlags_NoDecoration);
    ImGui::Indent();
    ImGui::CheckboxFlags("ImGuiWindowFlags_NoTitleBar", &flags, ImGuiWindowFlags_NoTitleBar);
    ImGui::CheckboxFlags("ImGuiWindowFlags_NoCollapse", &flags, ImGuiWindowFlags_NoCollapse);
    ImGui::CheckboxFlags("ImGuiWindowFlags_NoScrollbar", &flags, ImGuiWindowFlags_NoScrollbar);
    ImGui::Unindent();

    if (p_open && ImGui::Button("Close this window")) *p_open = false;
  }
  ImGui::End();
}

void Overlay::PrepareFontTexture(const std::string& FontName, const ImFontConfig* FontConfig, const ImWchar* GlyphRanges,
                                 const std::string& ResourceGroup) {
  ImGuiOverlayPtr->addFont(FontName, ResourceGroup, FontConfig, GlyphRanges);
  OrangeStyle();
  //SetupImGuiStyle(false, 0.5);
  ImGuiOverlayPtr->show();
}

}  // namespace Glue
