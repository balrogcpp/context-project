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

  // Suppress ini file creation
  ImGuiIO& io = ImGui::GetIO();
  io.IniFilename = nullptr;
  io.LogFilename = nullptr;
}

Overlay::~Overlay() {}

void Overlay::preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt) {}

void Overlay::OnUpdate(float time) {}

void Overlay::OnClean() {}

void Overlay::OnPause() {}

void Overlay::OnResume() {}

void Overlay::PrepareFontTexture(const std::string& FontName, const ImFontConfig* FontConfig, const ImWchar* GlyphRanges, const std::string& ResourceGroup) {
  ImGuiIO& io = ImGui::GetIO();
  ImGuiOverlayPtr->addFont(FontName, ResourceGroup, FontConfig, GlyphRanges);
  ImGuiOverlayPtr->show();
}

}  // namespace Glue
