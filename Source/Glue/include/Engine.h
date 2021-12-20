// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Engine.h"
#include "LazySingleton.h"
#include <memory>
#include <vector>

/// Components
namespace Glue {
class Component;
class Compositor;
class Overlay;
class Physics;
class Sound;
class DotSceneLoaderB;
class Conf;
}

namespace Glue {

class Engine final : public LazySingleton<Engine> {
 public:
  Engine();
  virtual ~Engine();
  void InitComponents();

  void Capture();
  void Pause();
  void InMenu();
  void OffMenu();
  void Resume();
  void Cleanup();
  void Update(float PassedTime);
  void RenderOneFrame();
  void RegComponent(Component* ComponentPtr);
  void UnRegComponent(Component* ComponentPtr);

  void CreateWindow();
  void SetFullscreen();
  void SetWindowed();
  void ResizeWindow(int Width, int Height);
  void SetWindowCaption(const char *Caption);
  std::pair<int, int> GetWindowSize() const;
  void GrabMouse(bool grab);
  void GrabMouse();
  void FreeMouse();

 private:
  void InitOgrePlugins();
  void InitDefaultRenderSystem();
  void InitOgreRenderSystemGL3();
  void InitOgreRenderSystemGLES2();
  void InitRenderWindow();
  void InitResourceLocation();
  void InitTextureSettings();
  void InitShadowSettings();
  /// Android helper
  void WindowRestoreFullscreenAndroid();

  std::string RenderSystemName;
  Ogre::RenderWindow* OgreRenderWindowPtr = nullptr;
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

  /// Components
  std::unique_ptr<Conf> ConfPtr;
  std::unique_ptr<Compositor> CompositorUPtr;
  std::unique_ptr<Overlay> OverlayPtr;
  std::unique_ptr<Physics> PhysicsPtr;
  std::unique_ptr<Sound> SoundPtr;
  std::unique_ptr<DotSceneLoaderB> LoaderPtr;
  std::vector<Component*> ComponentList;

  friend Conf& GetConf();
  friend Physics& GetPhysics();
  friend Sound& GetAudio();
  friend DotSceneLoaderB& GetLoader();
};

}  // namespace Glue
