/// created by Andrey Vasiliev

#pragma once

#include "System.h"
#include <Ogre.h>
#ifdef OGRE_BUILD_COMPONENT_OVERLAY
#include <Overlay/OgreImGuiOverlay.h>
#include <Overlay/OgreOverlaySystem.h>
#include "ImGuiInputListener.h"
#endif
extern "C" {
#include <SDL2/SDL_video.h>
}



namespace Glue {

inline bool RenderSystemIsGL() { return Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL Rendering Subsystem"; };
inline bool RenderSystemIsGL3() { return Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL 3+ Rendering Subsystem"; };
inline bool RenderSystemIsGLES2() { return Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL ES 2.x Rendering Subsystem"; };
inline Ogre::SceneManager* OgreSceneManager() { return Ogre::Root::getSingleton().getSceneManager("Default"); }
inline Ogre::SceneNode* OgreRootNode() { return OgreSceneManager()->getRootSceneNode(); }
inline Ogre::Camera* OgreCamera() { return OgreSceneManager()->getCamera("Default"); }
inline Ogre::SceneNode* OgreCameraNode() { return OgreCamera()->getParentSceneNode(); }

#ifdef OGRE_BUILD_RENDERSYSTEM_GL3PLUS
void InitOgreRenderSystemGL3();
bool CheckGL3Version(int major, int minor);
#endif

#ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
void InitOgreRenderSystemGLES2();
bool CheckGLES2Version(int major, int minor);
#endif

#ifdef OGRE_BUILD_RENDERSYSTEM_GL
void InitOgreRenderSystemGL();
bool CheckGLVersion(int major, int minor);
#endif

class VideoManager : public System<VideoManager> {
 public:
  VideoManager();
  virtual ~VideoManager();

  void InitSky();
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
