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
#pragma once

#include "ManagerCommon.hpp"

namespace CEGUI {
class Window;
}

namespace Context {

class CeguiOverlayManager : public ManagerCommon {
 private:
  CEGUI::Window *cegui_window_ = nullptr;
  bool active_ = false;

 public:
  [[nodiscard]] CEGUI::Window *GetCeguiWindow() const {
    return cegui_window_;
  }

 public:
  void Setup() override;
  void Show();
  void Hide();
  void Reset() override;

 private:
  bool frameRenderingQueued(const Ogre::FrameEvent &evt) final;
  void move(int x, int y) final;
  void move(int x, int y, int dx, int dy, bool left, bool right, bool middle) final;
  void wheel(int x, int y) final;
  void lb_down(int x, int y) final;
  void lb_up(int x, int y) final;

 private:
  static CeguiOverlayManager CeguiOverlayManagerSingleton;

 public:
  static CeguiOverlayManager *GetSingletonPtr() {
    return &CeguiOverlayManagerSingleton;
  }

  static CeguiOverlayManager &GetSingleton() {
    return CeguiOverlayManagerSingleton;
  }

};

}
*/