/// created by Andrey Vasiliev

#pragma once

#include "FSHelpers.h"
#include "System.h"
#include <Ogre.h>
#ifdef OGRE_BUILD_COMPONENT_OVERLAY
#include "ImGuiInputListener.h"
#include <Overlay/OgreImGuiOverlay.h>
#include <Overlay/OgreOverlaySystem.h>
#endif

namespace Glue {

class Window {
 public:
  Window(const std::string &caption);
  virtual ~Window();
  void Create(int monitor, bool fullscreen, int width, int height);
  void Delete();
  void GrabCursor(bool grab);
  void ShowCursor(bool show);
  void Resize(int width, int height);
  void SetFullscreen(bool fullscreen);
  void SetWindowCaption(const char* caption);

  SDL_Window* sdlWindow;
  std::string caption;
  bool fullscreen;
  int width;
  int height;
  uint32_t sdlFlags;
  bool vsync;
  Ogre::RenderWindow *ogreWindow;
  Ogre::RenderTarget *renderTarget;
};

class VideoManager : public System<VideoManager> {
 public:
  VideoManager();
  virtual ~VideoManager();

  void InitSky();
  void InitOgreRoot();
  void InitSDL();
  void CheckGPU();
  Ogre::Vector3 GetSunPosition();

  void OnSetUp() override;
  void OnClean() override;
  void OnPause() override;
  void OnResume() override;
  void OnUpdate(float time) override;

  void RenderFrame();

 protected:
  std::vector<Window> windowList;
  Ogre::Root* ogreRoot = nullptr;
  Ogre::SceneManager* sceneManager = nullptr;
  Ogre::RenderWindow* ogreWindow = nullptr;
  Ogre::Camera* ogreCamera = nullptr;
  Ogre::Viewport* ogreViewport = nullptr;
  std::shared_ptr<Ogre::PSSMShadowCameraSetup> pssmSetup;
  std::vector<float> pssmSplitPointList;
  const int PSSM_SPLITS = 3;
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
