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
#include "Compositor.h"
#include "Overlay.h"
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
class Render final : public Component, public Singleton<Render> {
 public:
  Render(int w, int h, bool f);
  virtual ~Render();

  void Cleanup() override;
  void Pause() override;
  void Resume() override;
  void Update(float time) override;

  void Refresh();
  void RenderOneFrame();
  void Resize(int w, int h, bool f);
  void RestoreFullscreenAndroid_();

  Window &GetWindow();
  Compositor &GetCompositor();


 private:
  void InitOgrePlugins_();
  void InitOgreRenderSystem_();
  void InitOgreRenderSystem_GL3_();
  void InitOgreRenderSystem_GLES2_();
  void InitRenderWindow_();
  void InitResourceLocation_();
  void InitTextureSettings_();
  void InitShadowSettings_();

  std::string render_system_;

  std::unique_ptr<Window> window_;
  std::unique_ptr<Compositor> compositor_;
  std::unique_ptr<Overlay> overlay_;
  std::shared_ptr<Ogre::PSSMShadowCameraSetup> pssm_;
  std::vector<float> split_points_;
  const size_t pssm_split_count_ = 3;

  view_ptr<Ogre::Root> root_;
  view_ptr<Ogre::SceneManager> scene_;
  view_ptr<Ogre::Camera> camera_;
  view_ptr<Ogre::Viewport> viewport_;
  view_ptr<Ogre::RenderWindow> render_window_;
};

} //namespace
