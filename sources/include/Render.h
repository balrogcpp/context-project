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

#include "NoCopy.h"
#include "Window.h"
#include "ShadowSettings.h"
#include "Compositor.h"
#include <vector>
#include <string>
#include <memory>

namespace Ogre {
class Root;
class SceneManager;
class RenderWindow;
}

namespace Context {
class Render final : public NoCopy {
 public:
  Render();
  virtual ~Render();

  void UpdateParams();
  void RenderOneFrame();
  void Resize(int32_t w, int32_t h);
  void Fullscreen(bool f);

 private:
  void CreateCamera();
  Window window_;
  std::unique_ptr<ShadowSettings> shadow_;
  std::unique_ptr<Compositor> compositor_;

  Ogre::Root *root_ = nullptr;
  Ogre::SceneManager *scene_ = nullptr;
  Ogre::RenderWindow *ogre_ = nullptr;

 public:
  Window &GetWindow() {
    return window_;
  }
}; //class ContextManager
} //namespace Context
