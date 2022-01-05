// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Components/ComponentsAll.h"
#include "Input/InputSequencer.h"
#include "Singleton.h"
#include "Log.h"
#include <memory>
#include <vector>

namespace Glue {

/// Forward declaration
class Engine;

/// Template helper function
template <typename T>
T* GetComponentPtr() {
  static T* ptr = Component<T>::GetInstancePtr();
  return ptr;
}

template <typename T>
T& GetComponent() {
  static T& ref = *Component<T>::GetInstancePtr();
  return ref;
}

/// Global component getters
Engine& GetEngine();
Physics& GetPhysics();
Sound& GetAudio();
Scene& GetScene();

class Engine final : public Singleton<Engine> {
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
  void ReadConfFile();
  void InitSound();
  void InitOverlay();
  void InitCompositor();
  void InitPhysics();
  void InitScene();
  void InitSDLSubsystems();
  void InitDefaultRenderSystem();
#ifdef OGRE_BUILD_RENDERSYSTEM_GL3PLUS
  void InitOgreRenderSystemGL3();
#endif
#ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
  void InitOgreRenderSystemGLES2();
#endif
  void InitOgrePlugins();
  void CreateSDLWindow();
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
  int WindowPositionFlag = SDL_WINDOWPOS_UNDEFINED;
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
  std::unique_ptr<Config> ConfPtr;
  std::unique_ptr<Compositor> CompositorUPtr;
  std::unique_ptr<Overlay> OverlayPtr;
  std::unique_ptr<Physics> PhysicsPtr;
  std::unique_ptr<Sound> SoundPtr;
  std::unique_ptr<Scene> ScenePtr;
  std::vector<ComponentI*> ComponentList;
#ifdef DESKTOP
  std::unique_ptr<Log> LogPtr;
#endif

  /// Global access to base components
  friend Engine& GetEngine();
  friend Physics& GetPhysics();
  friend Sound& GetAudio();
  friend Scene& GetScene();
};

}  // namespace Glue
