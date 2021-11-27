// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Compositor.h"
#include "Overlay.h"
#include "Singleton.h"
#include "System.h"
#include "Window.h"
#include "view_ptr.h"
#include <memory>
#include <vector>

namespace Ogre {
class Root;
class SceneManager;
class Camera;
class Viewport;
class RenderWindow;
}  // namespace Ogre

namespace glue {

class RenderSystem final : public System, public Singleton<RenderSystem> {
 public:
  RenderSystem(int w, int h, bool f);
  virtual ~RenderSystem();

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

 protected:
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

}  // namespace glue
