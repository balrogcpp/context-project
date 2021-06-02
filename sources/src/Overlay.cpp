// MIT License
//
// Copyright (c) 2021 Andrew Vasiliev
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "pcheader.h"

#ifdef OGRE_BUILD_COMPONENT_OVERLAY
#include <Overlay/OgreImGuiOverlay.h>
#include <Overlay/OgreOverlay.h>
#include <Overlay/OgreOverlayManager.h>
#include <Overlay/OgreOverlaySystem.h>
#endif

#include "Overlay.h"

using namespace std;

namespace xio {
Overlay::Overlay(view_ptr<Ogre::RenderWindow> render_window)
    : render_window_(render_window) {
  Ogre::SceneManager* sm =
      Ogre::Root::getSingletonPtr()->getSceneManager("Default");
  overlay_ = new Ogre::OverlaySystem();
  sm->addRenderQueueListener(overlay_.get());

  imgui_ = make_unique<Ogre::ImGuiOverlay>();
  imgui_->setZOrder(300);

  Ogre::OverlayManager::getSingleton().addOverlay(imgui_.get());
  render_window_->addListener(this);

  imgui_listener_ = make_unique<ImGuiInputListener>();

  // Suppress ini file creation
  ImGuiIO& io = ImGui::GetIO();
  io.IniFilename = "";
  io.LogFilename = "";
}

//----------------------------------------------------------------------------------------------------------------------
Overlay::~Overlay() {}

//----------------------------------------------------------------------------------------------------------------------
void Overlay::preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt) {
  if (!evt.source->getOverlaysEnabled()) return;
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
void Overlay::PrepareTexture(const std::string& name_,
                             const std::string group_) {
  imgui_->addFont(name_, group_);
  imgui_->show();
}

}  // namespace xio
