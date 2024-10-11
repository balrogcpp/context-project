/// created by Andrey Vasiliev

#pragma once
#include "AppStateManager.h"
#include "CompositorComponent.h"
#include "SDLListener.h"
#include "SceneComponent.h"
#include "Singleton.h"
#include "VideoComponent.h"
#include <OgreFrameListener.h>

namespace gge {

class Application final : Ogre::FrameListener, WindowListener, DynamicSingleton<Application> {
 public:
  /// Constructors
  Application();
  virtual ~Application();
  void Init();
  int Main();
  static void EmscriptenLoop(void *arg);

  /// Frame control
  void EnableFpsLock(bool enable);
  void SetFpsFreq(int fps);
  bool IsFpsLockEnabled();
  int GetFpsFreq();

 protected:
  void LoopBody();
  void Loop();
  void Go();
  void FrameControl();

  /// Ogre::FrameListener impl
  bool frameStarted(const Ogre::FrameEvent &evt) override;
  bool frameRenderingQueued(const Ogre::FrameEvent &evt) override;
  bool frameEnded(const Ogre::FrameEvent &evt) override;

  /// Window observer impl
  void OnQuit() override;
  void OnFocusLost() override;
  void OnFocusGained() override;

  /// Handle components
  std::unique_ptr<AppStateManager> appStateManager;
  bool exiting;
  bool sleep;
  bool lockFps;
  int targetFps;
  std::vector<ComponentI *> componentList;
  std::vector<ComponentI *> preRenderList;
  std::vector<ComponentI *> queueRenderList;
  std::vector<ComponentI *> postRenderList;
  std::unique_ptr<VideoComponent> video;
  std::unique_ptr<SceneComponent> scene;
  std::unique_ptr<CompositorComponent> compositor;
};

}  // namespace gge
