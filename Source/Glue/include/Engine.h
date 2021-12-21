// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Engine.h"
#include "LazySingleton.h"
#include <memory>
#include <vector>

/// Components
namespace Glue {
class ComponentI;
class Compositor;
class Overlay;
class Physics;
class Sound;
class Scene;
class Conf;
}  // namespace Glue

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
  void RenderFirstFrame();
  void RegComponent(ComponentI* ComponentPtr);
  void UnRegComponent(ComponentI* ComponentPtr);

  void InitSDLSubsystems();
  void CreateSDLWindow();
  void SetFullscreen(bool Fullscreen);
  void SetFullscreen();
  void SetWindowed();
  void ResizeWindow(int Width, int Height);
  void SetWindowCaption(const char* Caption);
  std::pair<int, int> GetWindowSize() const;
  void GrabMouse(bool grab);
  void GrabMouse();
  void FreeMouse();

 protected:
  void InitOgrePlugins();
  void InitDefaultRenderSystem();
  void InitOgreRenderSystemGL3();
  void InitOgreRenderSystemGLES2();
  void CreateOgreRenderWindow();
  void InitResourceLocation();
  void InitTextureSettings();
  void InitShadowSettings();
  /// Android helper
  void WindowRestoreFullscreenAndroid();

  std::string RenderSystemName;
  Ogre::RenderWindow* OgreRenderWindowPtr = nullptr;
  Ogre::RenderTarget* OgreRenderTarget = nullptr;
  std::shared_ptr<Ogre::PSSMShadowCameraSetup> PSSMSetupPtr;
  std::vector<float> PSSMSplitPointList;
  size_t PSSMSplitCount = 3;
  Ogre::Root* OgreRoot = nullptr;
  Ogre::SceneManager* OgreSceneManager = nullptr;
  Ogre::Camera* OgreCamera = nullptr;
  Ogre::Viewport* OgreViewport = nullptr;
  SDL_Window* SDLWindowPtr = nullptr;
  SDL_GLContext SDLGLContextPtr = nullptr;
  uint32_t SDLWindowFlags = 0;
  uint32_t WindowPositionFlag = SDL_WINDOWPOS_UNDEFINED;
  std::string WindowCaption;
  int WindowWidth = 1024;
  int WindowHeight = 768;
  bool WindowFullScreen = false;
  int ScreenWidth = 0;
  int ScreenHeight = 0;
  bool WindowsVsync = true;
  bool WindowGammaCorrection = false;
  int FSAA = 0;

  /// Components
  std::unique_ptr<Conf> ConfPtr;
  std::unique_ptr<Compositor> CompositorUPtr;
  std::unique_ptr<Overlay> OverlayPtr;
  std::unique_ptr<Physics> PhysicsPtr;
  std::unique_ptr<Sound> SoundPtr;
  std::unique_ptr<Scene> ScenePtr;
  std::vector<ComponentI*> ComponentList;

  friend Conf& GetConf();
  friend Physics& GetPhysics();
  friend Sound& GetAudio();
  friend Scene& GetScene();
};

}  // namespace Glue
