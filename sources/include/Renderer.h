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

#include "Singleton.h"
#include "Window.h"
#include "ShadowSettings.h"
#include "Compositor.h"
#include "Component.h"
#include "view_ptr.h"

namespace Ogre {
class Root;
class SceneManager;
class Camera;
class Viewport;
class RenderWindow;
}

namespace xio {
class Renderer final : public Component, public Singleton<Renderer> {
 public:
  Renderer(int w, int h, bool f);
  virtual ~Renderer();

  void Cleanup() override;
  void Pause() override;
  void Resume() override;
  void Update(float time) override;

  void Refresh();
  void UpdateParams(Ogre::TextureFilterOptions filtering, int anisotropy);
  void UpdateShadow(bool enable, float far_distance, int tex_size, int tex_format);
  void RenderOneFrame();
  void Resize(int w, int h, bool f);

 private:
  std::unique_ptr<Window> window_;
  std::unique_ptr<ShadowSettings> shadow_settings_;
  std::unique_ptr<Compositor> compositor_;

  view_ptr<Ogre::Root> root_;
  view_ptr<Ogre::SceneManager> scene_;
  view_ptr<Ogre::Camera> camera_;
  view_ptr<Ogre::Viewport> viewport_;
  view_ptr<Ogre::RenderWindow> render_window_;

 public:
  Window &GetWindow() {
    return *window_;
  }

  ShadowSettings &GetShadowSettings() {
    return *shadow_settings_;
  }

  Compositor &GetCompositor() {
    return *compositor_;
  }
};

} //namespace
