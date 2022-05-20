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

static void OrangeStyle() {
  ImGuiStyle& style = ImGui::GetStyle();

  style.Colors[ImGuiCol_TitleBg] = ImColor(255, 101, 53, 255);
  style.Colors[ImGuiCol_TitleBgActive] = ImColor(255, 101, 53, 255);
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImColor(0, 0, 0, 130);

  style.Colors[ImGuiCol_Tab] = ImColor(31, 30, 31, 255);
  style.Colors[ImGuiCol_TabActive] = ImColor(255, 101, 53, 255);
  style.Colors[ImGuiCol_TabHovered] = ImColor(255, 101, 53, 255);

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
}

Overlay::Overlay(Ogre::RenderWindow* OgreRenderWindowPtr) {
  OgreRenderWindow = OgreRenderWindowPtr;
  Ogre::SceneManager* OgreScene = Ogre::Root::getSingletonPtr()->getSceneManager("Default");
  OgreOverlayPtr = new Ogre::OverlaySystem();
  OgreScene->addRenderQueueListener(OgreOverlayPtr);
  ImGuiOverlayPtr = new Ogre::ImGuiOverlay();
  Ogre::OverlayManager::getSingleton().addOverlay(ImGuiOverlayPtr);
  OgreRenderWindowPtr->addListener(this);
  ImGuiListener = make_unique<ImGuiInputListener>();
  IoPtr = &ImGui::GetIO();
  ImGuiIO& io = ImGui::GetIO();
  OrangeStyle();
  ImGuiOverlayPtr->hide();
  io.IniFilename = nullptr;
  io.LogFilename = nullptr;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
#ifdef MOBILE
  io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;
#endif
}

Overlay::~Overlay() {}

void Overlay::preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt) {}

struct ResolutionItem {
  const char* Capture = "";
  int x = 0;
  int y = 0;
};

constexpr ResolutionItem ResolutionList[] = {
    {"360x800", 360, 800},
    {"800x600", 800, 600},
    {"810x1080", 810, 1080},
    {"768x1024", 768, 1024},
    {"834x1112", 834, 1112},
    {"1024x768", 1024, 768},
    {"1024x1366", 1024, 1366},
    {"1280x720", 1280, 720},
    {"1280x800", 1280, 800},
    {"1280x1024", 1280, 1024},
    {"1360x768", 1360, 768},
    {"1366x768", 1366, 768},
    {"1440x900", 1440, 900},
    {"1536x864", 1536, 864},
    {"1600x900", 1600, 900},
    {"1680x1050", 1680, 1050},
    {"1920x1080", 1920, 1080},
    {"1920x1200", 1920, 1200},
    {"2048x1152", 2048, 1152},
    {"2560x1440", 2560, 1440},
};

void Overlay::OnUpdate(float time) {
  static ImGuiIO& io = ImGui::GetIO();
  //  io.AddKeyEvent(kc2sc(SDLK_KP_ENTER), true);
  // if (io.KeysDown[SDL_SCANCODE_RETURN]) io.NavInputs[ImGuiNavInput_Activate] = 1.0f;
  //  ImGui::ShowDemoWindow();
}

void Overlay::OnSetUp() {
  ImGuiIO& io = ImGui::GetIO();
  AddFont("NotoSans-Regular", nullptr, io.Fonts->GetGlyphRangesCyrillic());
  ImFontConfig config;
  config.MergeMode = true;
  static const ImWchar icon_ranges[] = {ICON_MIN_MD, ICON_MAX_MD, 0};
  AddFont("KenneyIcon-Regular", &config, icon_ranges);
  ImGuiOverlayPtr->setZOrder(300);
  Show();
}

void Overlay::OnClean() {}

void Overlay::OnPause() {}

void Overlay::OnResume() {}

void Overlay::AddFont(const std::string& FontName, const ImFontConfig* FontConfig, const ImWchar* GlyphRanges, const std::string& ResourceGroup) {
  ImGuiOverlayPtr->addFont(FontName, ResourceGroup, FontConfig, GlyphRanges);
}

void Overlay::Show() { ImGuiOverlayPtr->show(); }

void Overlay::Hide() { ImGuiOverlayPtr->hide(); }

void Overlay::DrawMouseCursor(bool Draw) {}

void Overlay::NewFrame() { Ogre::ImGuiOverlay::NewFrame(); }

}  // namespace Glue
