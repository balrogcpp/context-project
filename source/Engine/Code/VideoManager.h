/// created by Andrey Vasiliev

#pragma once

#include "FSHelpers.h"
#include "SDL2.hpp"
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

  void grabCursor(bool grab);
  void showCursor(bool show);
  void resizeWindow(int width, int height);
  void setFullscreen(bool fullscreen);
  void setWindowCaption(const char* caption);
  void renderFrame();

 protected:
  std::vector<Window> windowList;
  std::string windowCaption = "Example0";
  int windowWidth = 1270;
  int windowHeight = 720;
  bool windowFullScreen = false;
  int screenWidth = 0;
  int screenHeight = 0;
  bool windowVsync = true;
  int currentDisplay = 0;
  std::vector<SDL_DisplayMode> sdlMonitorList;
  SDL_Window* sdlWindow = nullptr;
  uint32_t sdlWindowFlags = 0;
  int sdlWindowPositionFlag = SDL_WINDOWPOS_CENTERED;

  Ogre::Root* ogreRoot = nullptr;
  Ogre::SceneManager* sceneManager = nullptr;
  Ogre::RenderWindow* ogreWindow = nullptr;
  Ogre::Camera* camera = nullptr;
  Ogre::Viewport* viewport = nullptr;
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
