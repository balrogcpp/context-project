/// created by Andrey Vasiliev

#pragma once

#include "ImGuiInputListener.h"
#include "SDLListener.h"
#include "System.h"
#include "Window.h"
#include <Ogre.h>
#include <Overlay/OgreImGuiOverlay.h>
#include <Overlay/OgreOverlaySystem.h>

namespace Glue {
class VideoManager final : public System<VideoManager> {
 protected:
  class ShaderResolver;

 public:
  VideoManager();
  virtual ~VideoManager();

  void OnSetUp() override;
  void OnClean() override;
  void OnUpdate(float time) override;

  void RenderFrame();
  Window& GetWindow(int number);
  Window& GetMainWindow();
  void ShowWindow(int num, bool show);

 protected:
  void CreateWindow();
  void InitOgreRoot();
  void InitSDL();
  void InitOgreRTSS();
  void InitOgreOverlay();
  void LoadResources();
  void InitOgreScene();
  void CheckGPU();

  Window* mainWindow = nullptr;
  std::vector<Window> windowList;
  std::string ogreLogFile;
  unsigned short ogreMinLogLevel;
  Ogre::Root* ogreRoot = nullptr;
  Ogre::SceneManager* ogreSceneManager = nullptr;
  Ogre::RenderWindow* ogreWindow = nullptr;
  Ogre::Camera* ogreCamera = nullptr;
  Ogre::Viewport* ogreViewport = nullptr;
  std::unique_ptr<ShaderResolver> shaderResolver;
  Ogre::ShadowTechnique shadowTechnique;
  Ogre::Real shadowFarDistance;
  bool shadowEnabled;
  unsigned short shadowTexSize;
  std::shared_ptr<Ogre::PSSMShadowCameraSetup> pssmSetup;
  std::vector<Ogre::Real> pssmSplitPointList;
  int pssmSplitCount;
  std::unique_ptr<ImGuiInputListener> imguiListener;
  Ogre::ImGuiOverlay* imguiOverlay = nullptr;
};
}  // namespace Glue
