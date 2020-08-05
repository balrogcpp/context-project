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

#include "Context.hpp"

#include "MenuAppState.h"
#include "DemoDotAppState.h"
#include "AppStateManager.hpp"
#include "OgreOggSound.h"

#include "CeguiOverlayManager.hpp"

using namespace Context;

namespace Demo {

MenuAppState::~MenuAppState() {
}

void MenuAppState::Reset() {
  CeguiOverlayManager::GetSingleton().Hide();
  sheet->removeChild(quitButton);
  sheet->removeChild(nextButton);
  //CEGUI::WindowManager::getSingleton().destroyAllWindows();
}

void MenuAppState::preRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) {
  //
}

void MenuAppState::postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) {
  //
}

bool MenuAppState::frameRenderingQueued(const Ogre::FrameEvent &evt) {

  return true;
}

void MenuAppState::move(int x, int y) {

}

void MenuAppState::move(int x, int y, int dx, int dy, bool left, bool right, bool middle) {

}

void MenuAppState::wheel(int x, int y) {

}

void MenuAppState::lb_down(int x, int y) {

}

void MenuAppState::lb_up(int x, int y) {

}

void MenuAppState::rb_down(int x, int y) {

}

void MenuAppState::rb_up(int x, int y) {

}

void MenuAppState::mb_down(int x, int y) {

}

void MenuAppState::mb_up(int x, int y) {

}

void MenuAppState::key_down(SDL_Keycode sym) {
#ifdef DEBUG
//  if (SDL_GetScancodeFromKey(sym) == SDL_SCANCODE_G) {
//    AppStateManager::GetSingleton().GoNextState();
//  }
#endif
}

void MenuAppState::quit() {
  AppStateManager::GetSingleton().ResetGlobals();
}

void MenuAppState::Next() {
  AppStateManager::GetSingleton().GoNextState();
}

void MenuAppState::Setup() {
  AppStateManager::GetSingleton().SetNextState(std::make_shared<DemoDotAppState>());

  ContextManager::GetSingleton().UpdateCursorStatus({false, true, true});

  quitButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Button", "CEGUIDemo/QuitButton");
  quitButton->setText("Quit");
  quitButton->setSize(CEGUI::USize(CEGUI::UDim(0.30, 0), CEGUI::UDim(0.05, 0)));
  quitButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.5f - 0.3f / 2.0f, 0.0),
                                          CEGUI::UDim(0.5f - 0.05f / 2.0f, 0.0)));

  sheet = CeguiOverlayManager::GetSingleton().GetCeguiWindow();
  sheet->addChild(quitButton);
  quitButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuAppState::quit, this));

  nextButton = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Button", "CEGUIDemo/StartButton");
  nextButton->setText("New Game");
  nextButton->setSize(CEGUI::USize(CEGUI::UDim(0.30, 0), CEGUI::UDim(0.05, 0)));
  nextButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.5f - 0.3f / 2.0f, 0.0),
                                          CEGUI::UDim(0.5f - 0.5f / 2.0f, 0.0)));

  sheet->addChild(nextButton);
  nextButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuAppState::Next, this));

  CeguiOverlayManager::GetSingleton().Show();
}

}
