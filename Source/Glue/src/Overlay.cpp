// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Overlay.h"
#ifdef OGRE_BUILD_COMPONENT_OVERLAY
#include <Overlay/OgreImGuiOverlay.h>
#include <Overlay/OgreOverlay.h>
#include <Overlay/OgreOverlayManager.h>
#include <Overlay/OgreOverlaySystem.h>
#endif

using namespace std;

namespace glue {
Overlay::Overlay(ViewPtr<Ogre::RenderWindow> render_window) {
  this->render_window = render_window;
  Ogre::SceneManager* sm = Ogre::Root::getSingletonPtr()->getSceneManager("Default");
  overlay_system = new Ogre::OverlaySystem();
  sm->addRenderQueueListener(overlay_system.get());

  imgui_overlay = new Ogre::ImGuiOverlay();
  imgui_overlay->setZOrder(300);

  Ogre::OverlayManager::getSingleton().addOverlay(imgui_overlay.get());
  render_window->addListener(this);

  imgui_listener = make_unique<ImGuiInputListener>();

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

void Overlay::Update(float time) {}

void Overlay::Cleanup() {}

void Overlay::Pause() {}

void Overlay::Resume() {}

void Overlay::PrepareTexture(const std::string& name_, const std::string& group_) {
  imgui_overlay->addFont(name_, group_);
  imgui_overlay->show();
}

}  // namespace glue
