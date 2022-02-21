// This source file is part of "glue project". Created by Andrey Vasiliev

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
  this->OgreRenderWindow = OgreRenderWindowPtr;
  Ogre::SceneManager* OgreScene = Ogre::Root::getSingletonPtr()->getSceneManager("Default");
  OgreOverlay = new Ogre::OverlaySystem();
  OgreScene->addRenderQueueListener(OgreOverlay);

  imgui_overlay = new Ogre::ImGuiOverlay();

  Ogre::OverlayManager::getSingleton().addOverlay(imgui_overlay);
  OgreRenderWindowPtr->addListener(this);

  ImGuiListener = make_unique<ImGuiInputListener>();

  // Suppress ini file creation
  ImGuiIO& io = ImGui::GetIO();
  io.IniFilename = nullptr;
  io.LogFilename = nullptr;
}

Overlay::~Overlay() {}

void Overlay::preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt) {
  //  if (!evt.source->getOverlaysEnabled())  {
  //    return;
  //  }
}

void Overlay::OnUpdate(float time) {}

void Overlay::OnClean() {}

void Overlay::OnPause() {}

void Overlay::OnResume() {}

void Overlay::PrepareFontTexture(const std::string& FontName, const std::string& ResourceGroup) {
  imgui_overlay->addFont(FontName, ResourceGroup);
  imgui_overlay->show();
}

}  // namespace Glue
