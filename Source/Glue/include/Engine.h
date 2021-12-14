// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "AppState.h"
//#include "Components/Audio.h"
//#include "Components/DotSceneLoaderB.h"
//#include "Components/Overlay.h"
//#include "Components/Physics.h"
#include "Engine.h"
#include "LazySingleton.h"
//#include "Render.h"
#include <memory>
#include <vector>

/// Components
namespace Glue {
class Component;
class Compositor;
class Overlay;
class Physics;
class Audio;
class DotSceneLoaderB;
class Config;
}

namespace Glue {

class Engine final : public LazySingleton<Engine> {
 public:
  Engine();
  virtual ~Engine();

  void InitSystems();
  void Capture();
  void Pause();
  void InMenu();
  void OffMenu();
  void Resume();
  void Cleanup();
  void Refresh();
  void Update(float PassedTime);
  void RenderOneFrame();
  void RegSystem(Component* system);

  void CreateWindow();
  void SetFullscreen();
  void SetWindowed();
  void ResizeWindow(int Width, int Height);
  void SetWindowCaption(const char *Caption);
  /// Android helper
  void WindowRestoreFullscreenAndroid();

  std::pair<int, int> GetSize() const;
  void Grab(bool grab);
  Ogre::RenderWindow* OgreRenderWindowPtr = nullptr;

 private:
  void InitOgrePlugins();
  void InitDefaultRenderSystem();
  void InitOgreRenderSystemGL3();
  void InitOgreRenderSystemGLES2();
  void InitRenderWindow();
  void InitResourceLocation();
  void InitTextureSettings();
  void InitShadowSettings();


  std::string RenderSystemName;
  std::shared_ptr<Ogre::PSSMShadowCameraSetup> PSSMSetupPtr;
  std::vector<float> PSSMSplitPointList;
  const size_t PSSMSplitCount = 3;
  Ogre::Root* OgreRoot = nullptr;
  Ogre::SceneManager* OgreSceneManager = nullptr;
  Ogre::Camera* OgreCamera = nullptr;
  Ogre::Viewport* OgreViewport = nullptr;

  SDL_Window *SDLWindowPtr = nullptr;
  SDL_GLContext SDLGLContextPtr = nullptr;
  uint32_t SDLWindowFlags = 0;
  std::string caption_;
  int WindowWidth = 1024;
  int WindowHeight = 768;
  bool FullScreen = false;
  int ScreenWidth = 0;
  int ScreenHeight = 0;



  std::unique_ptr<Config> ConfPtr;
//  std::unique_ptr<Render> RS;
  std::unique_ptr<Compositor> CompositorUPtr;
  std::unique_ptr<Overlay> OverlayPtr;
  std::unique_ptr<Physics> ps;
  std::unique_ptr<Audio> as;
  std::unique_ptr<DotSceneLoaderB> loader;
  std::vector<Component*> ComponentList;

  friend Config& GetConf();
//  friend Render& GetRS();
  friend Physics& GetPhysics();
  friend Audio& GetAudio();
  friend DotSceneLoaderB& GetLoader();

};

}  // namespace Glue
