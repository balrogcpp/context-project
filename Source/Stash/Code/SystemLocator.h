/// created by Andrey Vasiliev

#pragma once
#include "CompositorManager.h"
#include "SDLListener.h"
#include "SceneManager.h"
#include "Singleton.h"
#include "VideoManager.h"
#include <OgreFrameListener.h>

namespace gge {

class SystemLocator final : public System<SystemLocator>, Ogre::FrameListener, WindowListener {
 public:
  SystemLocator();
  virtual ~SystemLocator();

  void Init();
  void Capture();
  void RenderFrame();
  void FrameControl();

  void RegComponent(SystemI* component, bool preRender = false);
  void UnregComponent(SystemI* component);

  /// System impl
  void OnSetUp() override;
  void OnUpdate(float time) override {}
  void OnClean() override;
  void SetSleep(bool sleep) override;

  /// Frame control
  void EnableFpsLock(bool enable);
  void SetFpsFreq(int fps);
  bool IsFpsLockEnabled();
  int GetFpsFreq();

 protected:
  /// Ogre::FrameListener impl
  bool frameStarted(const Ogre::FrameEvent& evt) override;
  bool frameRenderingQueued(const Ogre::FrameEvent& evt) override;
  bool frameEnded(const Ogre::FrameEvent& evt) override;

  /// Window observer impl
  void OnQuit() override;
  void OnFocusLost() override;
  void OnFocusGained() override;

  bool lockFps;
  int targetFps;
  std::vector<SystemI*> componentList;
  std::vector<SystemI*> preRenderList;
  std::vector<SystemI*> queueRenderList;
  std::vector<SystemI*> postRenderList;
  std::unique_ptr<VideoManager> video;
  std::unique_ptr<SceneManager> scene;
  std::unique_ptr<CompositorManager> compositor;
};
}  // namespace gge
