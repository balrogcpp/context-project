/// created by Andrey Vasiliev

#pragma once

#include "System.h"
#include <Ogre.h>
#ifdef OGRE_BUILD_COMPONENT_OVERLAY
#include "ImGuiInputListener.h"
#include <Overlay/OgreImGuiOverlay.h>
#include <Overlay/OgreOverlaySystem.h>
#endif

struct SDL_Window;

namespace Glue {

class Window {
 public:
  Window(const std::string& caption);
  virtual ~Window();
  void Create(Ogre::Camera* ogreCamera, int monitor, bool fullscreen, int width, int height);
  void Delete();
  void GrabCursor(bool grab);
  void ShowCursor(bool show);
  void Resize(int width, int height);
  void SetFullscreen(bool fullscreen);
  void SetWindowCaption(const char* caption);

  std::string caption;
  bool fullscreen;
  int width;
  int height;
  uint32_t sdlFlags;
  bool vsync;
  SDL_Window* sdlWindow = nullptr;
  Ogre::RenderWindow* ogreWindow = nullptr;
  Ogre::RenderTarget* renderTarget = nullptr;
  Ogre::Viewport* ogreViewport = nullptr;
  Ogre::Camera* ogreCamera = nullptr;
};

class VideoManager : public System<VideoManager> {
 protected:
  class ShaderResolver;

 public:
  VideoManager();
  virtual ~VideoManager();

  void OnSetUp() override;
  void OnClean() override;
  void OnPause() override;
  void OnResume() override;
  void OnUpdate(float time) override;

  void InitSky();

  void RenderFrame();
  Ogre::Vector3 GetSunPosition();

 protected:
  void CreateWindow();
  void InitOgreRoot();
  void InitSDL();
  void InitOgreRTSS();
  void InitOgreOverlay();
  void LoadResources();
  void InitOgreScene();
  void CheckGPU();

  std::vector<Window> windowList;
  std::string ogreLogFile;
  unsigned short ogreMinLogLevel;
  Ogre::Root* ogreRoot = nullptr;
  Ogre::SceneManager* sceneManager = nullptr;
  Ogre::RenderWindow* ogreWindow = nullptr;
  Ogre::Camera* ogreCamera = nullptr;
  Ogre::Viewport* ogreViewport = nullptr;
  ShaderResolver* shaderResolver = nullptr;
  Ogre::ShadowTechnique shadowTechnique;
  Ogre::Real shadowFarDistance;
  unsigned short shadowTexSize;
  std::shared_ptr<Ogre::PSSMShadowCameraSetup> pssmSetup;
  std::vector<float> pssmSplitPointList;
  int pssmSplitCount;
  std::unique_ptr<ImGuiInputListener> imguiListener;
  Ogre::ImGuiOverlay* imguiOverlay = nullptr;
  bool skyNeedsUpdate = false;
  Ogre::GpuProgramParametersSharedPtr skyBoxFpParams;
  const std::array<const char*, 10> hosekParamList{"A", "B", "C", "D", "E", "F", "G", "H", "I", "Z"};
  std::array<Ogre::Vector3, 10> hosekParams;
  std::vector<Ogre::GpuProgramParametersSharedPtr> gpuFpParams;
  std::vector<Ogre::GpuProgramParametersSharedPtr> gpuVpParams;
};

}  // namespace Glue
