// This source file is part of context-project
// Created by Andrew Vasiliev

#include "pcheader.h"

#ifdef OGRE_BUILD_COMPONENT_OVERLAY
#include <Overlay/OgreImGuiOverlay.h>
#include <Overlay/OgreOverlay.h>
#include <Overlay/OgreOverlayManager.h>
#include <Overlay/OgreOverlaySystem.h>
#endif

#include "Overlay.h"

using namespace std;

namespace glue {
Overlay::Overlay(view_ptr<Ogre::RenderWindow> render_window) : render_window_(render_window) {
  Ogre::SceneManager* sm = Ogre::Root::getSingletonPtr()->getSceneManager("Default");
  overlay_ = new Ogre::OverlaySystem();
  sm->addRenderQueueListener(overlay_.get());

  imgui_ = new Ogre::ImGuiOverlay();
  imgui_->setZOrder(300);

  Ogre::OverlayManager::getSingleton().addOverlay(imgui_.get());
  render_window_->addListener(this);

  imgui_listener_ = make_unique<ImGuiInputListener>();

  // Suppress ini file creation
  ImGuiIO& io = ImGui::GetIO();
  io.IniFilename = nullptr;
  io.LogFilename = nullptr;
}

//----------------------------------------------------------------------------------------------------------------------
Overlay::~Overlay() {}

//----------------------------------------------------------------------------------------------------------------------
void Overlay::preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt) {
//  if (!evt.source->getOverlaysEnabled())  {
//    return;
//  }
}

//----------------------------------------------------------------------------------------------------------------------
void Overlay::Update(float time) {}

//----------------------------------------------------------------------------------------------------------------------
void Overlay::Cleanup() {}

//----------------------------------------------------------------------------------------------------------------------
void Overlay::Pause() {}

//----------------------------------------------------------------------------------------------------------------------
void Overlay::Resume() {}

//----------------------------------------------------------------------------------------------------------------------
void Overlay::PrepareTexture(const std::string& name_, const std::string& group_) {
  imgui_->addFont(name_, group_);
  imgui_->show();
}

}  // namespace glue
