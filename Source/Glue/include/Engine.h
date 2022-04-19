// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include "Components/ComponentsAll.h"
#include "Input/InputSequencer.h"
#include "Log.h"
#include "Singleton.h"
#include <memory>
#include <vector>

namespace Glue {

/// Forward declaration
class Engine;

/// Global component getters
Engine& GetEngine();
Physics& GetPhysics();
Sound& GetAudio();
Scene& GetScene();
Overlay& GetOverlay();

class Engine final : public Singleton<Engine> {
 public:
  Engine();
  virtual ~Engine();

  /// Initialized all components. Must be called once after Engine object created
  void InitComponents();

  /// Called every frame to read input
  void Capture();

  /// Set all components to Pause state
  void OnPause();

  /// Called when in-game menu turned on
  void OnMenuOn();

  /// Called when in-game menu turned off
  void OnMenuOff();

  /// Called when game is resumed
  void OnResume();

  /// Called on game exit
  void OnCleanup();

  /// Called every frame
  void Update(float PassedTime);

  /// Called to start render
  void RenderFrame();

  /// Internal function to add component
  void RegComponent(ComponentI* ComponentPtr);

  /// Internal function to unload component
  void UnRegComponent(ComponentI* ComponentPtr);

  void SetFullscreen(bool Fullscreen);
  bool IsFullscreen();
  void ResizeWindow(int Width, int Height);
  void SetWindowCaption(const char* Caption);
  void GrabMouse(bool grab);

 protected:
  /// Internal. Check CPU
  void TestCPUCapabilities();

  /// Internal. Check GPU
  void TestGPUCapabilities();

  /// Internal. Read engine parameters from Config.ini. If not exists create and fill it with default value
  void ReadConfFile();

  /// Creates and initiates Sound component instance. Internal
  void InitSound();

  /// Load resources by list. Internal
  void InitResources();

  /// Creates and initiates Overlay component. Internal
  void InitOverlay();

  /// Creates and initiates Compositor component. Internal
  void InitCompositor();

  /// Creates and initiates Physics component. Internal
  void InitPhysics();

  /// Creates and initiates Scene component. Internal
  void InitScene();

  /// Initiates SDL context
  void InitSDLSubsystems();

  /// Creates one of 3 Ogre render systems: GL, GL3 or GLSLES2
  void InitDefaultRenderSystem();
#ifdef OGRE_BUILD_RENDERSYSTEM_GL3PLUS
  ///
  void InitOgreRenderSystemGL3();
#endif
#ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
  ///
  void InitOgreRenderSystemGLES2();
#endif
#ifdef OGRE_BUILD_RENDERSYSTEM_GL
  ///
  void InitOgreRenderSystemGL();
#endif
  /// Load into memory static Ogre plugins
  void InitOgrePlugins();

  /// Creates SDL window
  void CreateSDLWindow();

  /// Creates Ogre render context attached to SDL window
  void CreateOgreRenderWindow();

  /// Scans Assets and Programs directories for shaders, materials, models, textures, sounds etc.
  void InitResourceLocation();

  /// Ogre texture settings, like filtration
  void InitTextureSettings();

  /// Ogre shadow settings
  void InitShadowSettings();

#ifdef MOBILE
  /// Android helper function
  void AndroidRestoreWindow();
#endif

  std::vector<Ogre::Plugin*> PluginList;
  std::string RenderSystemName;
  Ogre::RenderWindow* OgreRenderWindowPtr = nullptr;
  Ogre::RenderTarget* OgreRenderTarget = nullptr;
  std::shared_ptr<Ogre::PSSMShadowCameraSetup> PSSMSetupPtr;
  std::vector<float> PSSMSplitPointList;
  int PSSMSplitCount = 3;
  Ogre::Root* OgreRoot = nullptr;
  Ogre::SceneManager* OgreSceneManager = nullptr;
  Ogre::Camera* OgreCamera = nullptr;
  Ogre::Viewport* OgreViewport = nullptr;
  SDL_Window* SDLWindowPtr = nullptr;
  SDL_GLContext SDLGLContextPtr = nullptr;
  uint32_t SDLWindowFlags = 0;
  int WindowPositionFlag = SDL_WINDOWPOS_UNDEFINED;
  std::string WindowCaption = "Example0";
  int WindowWidth = 1024;
  int WindowHeight = 768;
  bool WindowFullScreen = false;
  int ScreenWidth = 0;
  int ScreenHeight = 0;
  bool WindowVsync = true;
  bool WindowGammaCorrection = false;
  int WindowFSAA = 0;

  /// Components
  Config* ConfigPtr = nullptr;
  std::unique_ptr<Compositor> CompositorUPtr;
  std::unique_ptr<Overlay> OverlayPtr;
  std::unique_ptr<Physics> PhysicsPtr;
  std::unique_ptr<Sound> SoundPtr;
  std::unique_ptr<Scene> ScenePtr;
  std::vector<ComponentI*> ComponentList;

  /// Global access to base components
  friend Engine& GetEngine();
  friend Physics& GetPhysics();
  friend Sound& GetAudio();
  friend Scene& GetScene();
};

}  // namespace Glue
