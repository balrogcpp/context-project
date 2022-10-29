/// created by Andrey Vasiliev

#pragma once
#include "AudioManager.h"
#include "CompositorManager.h"
#include "PhysicsManager.h"
#include "SDLListener.h"
#include "SceneManager.h"
#include "Singleton.h"
#include "SkyManager.h"
#include "TerrainManager.h"
#include "VideoManager.h"
#include <OgreFrameListener.h>

namespace Glue {
class Locator final : public Singleton<Locator>, Ogre::FrameListener, WindowListener {
 public:
  Locator();
  virtual ~Locator();

  void Init();
  void Capture();
  void OnCleanup();
  void OnUpdate(float time);
  void RenderFrame();

  void RegComponent(SystemI* component);
  void UnRegComponent(SystemI* component);

 protected:
  /// Ogre::FrameListener impl
  bool frameStarted(const Ogre::FrameEvent& evt) override;
  bool frameRenderingQueued(const Ogre::FrameEvent& evt) override;
  bool frameEnded(const Ogre::FrameEvent& evt) override;

  /// Window observer impl
  void OnQuit() override;
  void OnFocusLost() override;
  void OnFocusGained() override;

  bool sleep = false;
  std::vector<SystemI*> componentList;
  std::unique_ptr<VideoManager> video;
  std::unique_ptr<SceneManager> scene;
  std::unique_ptr<CompositorManager> compositor;
  std::unique_ptr<PhysicsManager> physics;
  std::unique_ptr<AudioManager> audio;
  std::unique_ptr<SkyManager> sky;
  std::unique_ptr<TerrainManager> terrain;
};
}  // namespace Glue
