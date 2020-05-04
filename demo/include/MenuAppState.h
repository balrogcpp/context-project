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

#pragma once

#include "AppState.hpp"

namespace CEGUI {
class Window;
}

namespace Demo {

class MenuAppState : public Context::AppState {
 public:
    MenuAppState() : AppState() {};

    ~MenuAppState();

 private:
    CEGUI::Window *quitButton = nullptr;
    CEGUI::Window *nextButton = nullptr;
    CEGUI::Window *sheet = nullptr;

 public:
  void preRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) final;
  void postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) final;
  bool frameRenderingQueued(const Ogre::FrameEvent &evt) override final;
  void Setup() final;
  void Reset() final;
  void Quit() final;
  void Next() final;

 private:
  virtual void MouseMove(int x, int y) override final;
  virtual void MouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) override final;
  virtual void MouseWheel(int x, int y) override final;
  virtual void LeftButtonDown(int x, int y) override final;
  virtual void LeftButtonUp(int x, int y) override final;
  virtual void RightButtonDown(int x, int y) override final;
  virtual void RightButtonUp(int x, int y) override final;
  virtual void MiddleButtonDown(int x, int y) override final;
  virtual void MiddleButtonUp(int x, int y) override final;
  virtual void KeyDown(SDL_Keycode sym) override final;
};

}
