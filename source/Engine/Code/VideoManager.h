/// created by Andrey Vasiliev

#pragma once

#include "Observer.h"
#include "System.h"
#include <Ogre.h>
#ifdef OGRE_BUILD_COMPONENT_OVERLAY
#include "ImGuiInputListener.h"
#include <Overlay/OgreImGuiOverlay.h>
#include <Overlay/OgreOverlaySystem.h>
#endif

struct SDL_Window;

namespace Glue {

class VideoManager;

class Window : public WindowObserver {
 public:
  Window();
  virtual ~Window();
  void Create(const std::string& caption, Ogre::Camera* camera, int monitor, int width, int height, uint32_t sdlFlags);
  void Show(bool show);
  void Delete();
  void RenderFrame() const;
  void GrabCursor(bool grab);
  void ShowCursor(bool show);
  void Resize(int width, int height);
  void SetFullscreen(bool fullscreen);
  void SetWindowCaption(const char* caption);

 protected:
  friend class VideoManager;

  void OnEvent(const SDL_Event& event) override;
  void OnQuit() override;
  void OnFocusLost() override;
  void OnFocusGained() override;
  void OnSizeChanged(int x, int y, uint32_t id) override;
  void OnExposed() override;

  std::string caption;
  bool fullscreen;
  int width;
  int height;
  uint32_t sdlFlags;
  bool vsync;
  uint32_t id;
  Ogre::Root* ogreRoot = nullptr;
  SDL_Window* sdlWindow = nullptr;
  Ogre::RenderWindow* ogreWindow = nullptr;
  Ogre::RenderTarget* renderTarget = nullptr;
  Ogre::Viewport* ogreViewport = nullptr;
  Ogre::Camera* ogreCamera = nullptr;
};

class VideoManager final : public System<VideoManager>, public Ogre::RenderTargetListener {
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

  void RenderFrame();
  Window& GetWindow(int number);
  Window& GetMainWindow();
  void ShowMainWindow(bool show);

 protected:
  void CreateWindow();
  void InitOgreRoot();
  void InitSDL();
  void InitOgreRTSS();
  void InitOgreOverlay();
  void LoadResources();
  void InitOgreScene();
  void CheckGPU();

  Window* mainWindow = nullptr;
  std::vector<Window> windowList;
  std::string ogreLogFile;
  unsigned short ogreMinLogLevel;
  Ogre::Root* ogreRoot = nullptr;
  Ogre::SceneManager* sceneManager = nullptr;
  Ogre::RenderWindow* ogreWindow = nullptr;
  Ogre::Camera* ogreCamera = nullptr;
  Ogre::Viewport* ogreViewport = nullptr;
  std::unique_ptr<ShaderResolver> shaderResolver;
  Ogre::ShadowTechnique shadowTechnique;
  Ogre::Real shadowFarDistance;
  bool shadowEnabled;
  unsigned short shadowTexSize;
  std::shared_ptr<Ogre::PSSMShadowCameraSetup> pssmSetup;
  std::vector<Ogre::Real> pssmSplitPointList;
  int pssmSplitCount;
  std::unique_ptr<ImGuiInputListener> imguiListener;
  Ogre::ImGuiOverlay* imguiOverlay = nullptr;
};

}  // namespace Glue
