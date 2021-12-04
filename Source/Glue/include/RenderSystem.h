// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Compositor.h"
#include "Overlay.h"
#include "Singleton.h"
#include "Component.h"
#include "Window.h"
#include <memory>
#include <vector>

namespace Ogre {
class Root;
class SceneManager;
class Camera;
class Viewport;
class RenderWindow;
}  // namespace Ogre

namespace Glue {

class RenderSystem final : public Component, public Singleton<RenderSystem> {
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
  Ogre::Root* ogre_root;
  Ogre::SceneManager* scene;
  Ogre::Camera* camera;
  Ogre::Viewport* viewport;
  Ogre::RenderWindow* render_window;
};

}  // namespace glue
