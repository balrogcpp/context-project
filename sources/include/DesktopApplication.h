//MIT License
//
//Copyright (c) 2021 Andrew Vasiliev
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#pragma once
#include <OgreLog.h>
#include "Singleton.h"
#include "Engine.h"
#include "StateManager.h"
#include "ComponentLocator.h"
#include "BaseApplication.h"
#include "view_ptr.h"

namespace xio {

class DesktopApplication final : public WindowObserver, public BaseApplication {
 public:
  DesktopApplication();
  virtual ~DesktopApplication();

 private:
  void OnMain() override;
  void OnLoop() override;
  void OnGo() override;

  //WindowObserver callback
  void Event(const SDL_Event &evt) override;
  void Other(uint8_t type, int32_t code, void *data1, void *data2) override;
  void Quit() override;
};

} //namespace
