// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Component.h"
#include "Compositor.h"
#include "LazySingleton.h"
#include "Overlay.h"
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

class RenderSystem final : public Component, public DynamicSingleton<RenderSystem> {
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

  Window& GetWindow();
  Compositor& GetCompositor();

 private:
  void InitOgrePlugins();
  void InitOgreRenderSystem();
  void InitOgreRenderSystemGL3();
  void InitOgreRenderSystemGLES2();
  void InitRenderWindow();
  void InitResourceLocation();
  void InitTextureSettings();
  void InitShadowSettings();

  std::string RenderSystemName;
  std::unique_ptr<Window> window;
  std::unique_ptr<Compositor> compositor;
  std::unique_ptr<Overlay> OverlayPtr;
  std::shared_ptr<Ogre::PSSMShadowCameraSetup> PSSMSetupPtr;
  std::vector<float> PSSMSplitPointList;
  const size_t PSSMSplitCount = 3;
  Ogre::Root* OgreRoot = nullptr;
  Ogre::SceneManager* OgreSceneManager = nullptr;
  Ogre::Camera* OgreCamera = nullptr;
  Ogre::Viewport* OgreViewport = nullptr;
  Ogre::RenderWindow* OgreRenderWindowPtr = nullptr;
};

}  // namespace Glue
