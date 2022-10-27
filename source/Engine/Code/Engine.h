/// created by Andrey Vasiliev

#pragma once

#include "AudioManager.h"
#include "CompositorManager.h"
#include "Observer.h"
#include "PhysicsManager.h"
#include "SceneManager.h"
#include "Singleton.h"
#include "VideoManager.h"
#include "SkyManager.h"
#include "TerrainManager.h"

namespace Glue {

class Engine final : public Singleton<Engine>, public Ogre::FrameListener {
 public:
  Engine();
  virtual ~Engine();

  bool frameStarted(const Ogre::FrameEvent &evt) override;
  bool frameRenderingQueued(const Ogre::FrameEvent &evt) override;
  bool frameEnded(const Ogre::FrameEvent& evt) override;

  void Init();
  void Capture();
  void OnPause();
  void OnMenuOn();
  void OnMenuOff();
  void OnResume();
  void OnCleanup();
  void Update(float PassedTime);
  void RenderFrame();

  void RegComponent(SystemI* component);
  void UnRegComponent(SystemI* component);

 protected:
  bool paused = false;

  /// Components
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
