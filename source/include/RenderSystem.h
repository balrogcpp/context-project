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
  void InitOgrePlugins();
  void InitOgreRenderSystem();
  void InitOgreRenderSystemGL3();
  void InitOgreRenderSystemGLES2();
  void InitRenderWindow();
  void InitResourceLocation();
  void InitTextureSettings();
  void InitShadowSettings();

  std::string render_system;

  std::unique_ptr<Window> window;
  std::unique_ptr<Compositor> compositor;
  std::unique_ptr<Overlay> overlay;
  std::shared_ptr<Ogre::PSSMShadowCameraSetup> pssm;
  std::vector<float> split_points;
  const size_t pssm_split_count = 3;

  view_ptr<Ogre::Root> ogre_root;
  view_ptr<Ogre::SceneManager> scene;
  view_ptr<Ogre::Camera> camera;
  view_ptr<Ogre::Viewport> viewport;
  view_ptr<Ogre::RenderWindow> render_window;
};

}  // namespace glue
