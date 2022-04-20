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
  ImGuiIO& io = ImGui::GetIO();
  OrangeStyle();
  ImGuiOverlayPtr->hide();
  io.IniFilename = nullptr;
  io.LogFilename = nullptr;
}

Overlay::~Overlay() {}

void Overlay::preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt) {}

void Overlay::OnUpdate(float time) {}

void Overlay::OnSetUp() {
  ImGuiIO& io = ImGui::GetIO();
  AddFont("NotoSans-Regular", nullptr, io.Fonts->GetGlyphRangesCyrillic());
  ImFontConfig config;
  config.MergeMode = true;
  static const ImWchar icon_ranges[] = {ICON_MIN_MD, ICON_MAX_MD, 0};
  AddFont("KenneyIcon-Regular", &config, icon_ranges);

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

void Overlay::NewFrame() { Ogre::ImGuiOverlay::NewFrame(); }

}  // namespace Glue
