/*
MIT License

Copyright (c) 2020 Andrey Vasiliev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
/*
#include "pcheader.hpp"

#include "CeguiOverlayManager.hpp"

#include "ContextManager.hpp"

// Disable logging in CEGUI for Release build
#ifndef DEBUG
class CeguiNonLogger : public CEGUI::Logger {
  void logEvent(const CEGUI::String &, CEGUI::LoggingLevel) {}
  void setLogFilename(const CEGUI::String &, bool) {}
};
static CeguiNonLogger g_ceguiNonLogger;
#endif

namespace Context {

CeguiOverlayManager CeguiOverlayManager::CeguiOverlayManagerSingleton;
//----------------------------------------------------------------------------------------------------------------------
void CeguiOverlayManager::Setup() {
//Initialize CEGUI
  std::string caption = ContextManager::GetSingleton().GetWindowCaption();
  Ogre::RenderTarget *renderTarget = ContextManager::GetSingleton().GetOgreRootPtr()->getRenderTarget(caption);
  CEGUI::OgreRenderer &myRenderer = CEGUI::OgreRenderer::bootstrapSystem(*renderTarget);

//window manager
  auto &winMgr = CEGUI::WindowManager::getSingleton();

  CEGUI::ImageManager::setImagesetDefaultResourceGroup("Imagesets");
  CEGUI::Font::setDefaultResourceGroup("Fonts");
  CEGUI::Scheme::setDefaultResourceGroup("Schemes");
  CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
  CEGUI::WindowManager::setDefaultResourceGroup("Layouts");

  //Setup CEGUI resorus
  CEGUI::ImageManager::getSingleton().loadImageset("TaharezLook.imageset");
  CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");

  CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
  cegui_window_ = wmgr.createWindow("DefaultWindow", "CEGUIDemo/Sheet");

  CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(CeguiOverlayManager::GetSingleton().GetCeguiWindow());

  CeguiOverlayManager::Hide();
}
//----------------------------------------------------------------------------------------------------------------------
void CeguiOverlayManager::move(int x, int y) {
  if (active_) {
    CEGUI::System &sys = CEGUI::System::getSingleton();
    CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(x, y);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void CeguiOverlayManager::move(int x, int y, int dx, int dy, bool left, bool right, bool middle) {
  if (active_) {
    CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(dx, dy);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void CeguiOverlayManager::wheel(int x, int y) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void CeguiOverlayManager::lb_down(int x, int y) {
  if (active_) {
    CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(CEGUI::MouseButton::LeftButton);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void CeguiOverlayManager::lb_up(int x, int y) {
  if (active_) {
    CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(CEGUI::MouseButton::LeftButton);
  }
}
//----------------------------------------------------------------------------------------------------------------------
bool CeguiOverlayManager::frameRenderingQueued(const Ogre::FrameEvent &evt) {
  if (active_) {
    CEGUI::System::getSingleton().injectTimePulse(evt.timeSinceLastFrame);
  }

  return true;
}
//----------------------------------------------------------------------------------------------------------------------
void CeguiOverlayManager::Show() {
  cegui_window_->show();
  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(true);
  active_ = true;
}
//----------------------------------------------------------------------------------------------------------------------
void CeguiOverlayManager::Hide() {
  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(false);
  cegui_window_->hide();
  active_ = false;
}
//----------------------------------------------------------------------------------------------------------------------
void CeguiOverlayManager::Reset() {
//  cegui_window_->hide();
//  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(false);
}

} //namespace Context
*/