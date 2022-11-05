/// created by Andrey Vasiliev

#pragma once
#include "ImGuiListener.h"
#include "SDLListener.h"
#include "System.h"
#include "Window.h"
#include <Ogre.h>

namespace Glue {
class VideoManager final : public System<VideoManager> {
 protected:
  class ShaderResolver;

 public:
  VideoManager();
  virtual ~VideoManager();

  void RenderFrame();
  Window& GetWindow(int number = 0);
  Window& GetMainWindow();
  void ShowWindow(int num, bool show);

  void UnloadResources();
  void ClearScene();

 protected:
  void MakeWindow();
  void InitOgreRoot();
  void InitSDL();
  void InitOgreRTSS();
  void InitOgreOverlay();
  void LoadResources();
  void InitOgreScene();
  void CheckGPU();

  /// System impl
  void OnSetUp() override;
  void OnClean() override;
  void OnUpdate(float time) override;

  std::unique_ptr<ImGuiListener> imguiListener;
  Window* mainWindow = nullptr;
  std::vector<Window> windowList;
  std::string ogreLogFile;
  unsigned short ogreMinLogLevel;
  std::unique_ptr<Ogre::LogManager> ogreLogManager;
  std::unique_ptr<ShaderResolver> shaderResolver;
  Ogre::ShadowTechnique shadowTechnique;
  Ogre::Real shadowFarDistance;
  bool shadowEnabled;
  unsigned short shadowTexSize;
  std::shared_ptr<Ogre::PSSMShadowCameraSetup> pssmSetup;
  std::vector<Ogre::Real> pssmSplitPointList;
  int pssmSplitCount;

  Ogre::Root* ogreRoot = nullptr;
  Ogre::SceneManager* ogreSceneManager = nullptr;
  Ogre::RenderWindow* ogreWindow = nullptr;
  Ogre::Camera* ogreCamera = nullptr;
  Ogre::Viewport* ogreViewport = nullptr;
};
}  // namespace Glue
