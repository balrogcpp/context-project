/// created by Andrey Vasiliev

#pragma once
#include "SDLListener.h"
#include "Singleton.h"
#include "Component.h"
#include <OgreFrameListener.h>
#include <map>
#include <memory>

namespace gge {

class AppState : public DeviceListener, public WindowListener {
 public:
  virtual std::string GetName() = 0;
  virtual void OnSetUp() = 0;
  virtual void OnUpdate(float time) = 0;
  virtual void BeforeFrame(float time) {}
  virtual void AfterFrame(float time) {}
  virtual void OnClean() = 0;
};

class AppStateImpl : public AppState {
 public:
  std::string GetName() override { return "NULL"; }
  void OnSetUp() override {}
  void OnUpdate(float time) override {}
  void OnClean() override {}
};

class AppStateManager final : public Ogre::FrameListener, public Component<AppStateManager> {
 public:
  AppStateManager();
  virtual ~AppStateManager();
  void Init();

  void RegAppState(std::shared_ptr<AppState> appState);
  void UnregAppState(std::shared_ptr<AppState> appState);
  void UnregAppState(const std::string &name);

  void SetActiveAppState(std::shared_ptr<AppState> appState);
  void SetActiveAppState(const std::string &name);
  AppState *GetActiveAppState();

 protected:
  /// System impl
  void OnSetUp() override;
  void OnUpdate(float time) override;
  void OnClean() override;

  /// Ogre::FrameListener impl
  bool frameStarted(const Ogre::FrameEvent &evt) override;
  bool frameRenderingQueued(const Ogre::FrameEvent &evt) override;
  bool frameEnded(const Ogre::FrameEvent &evt) override;

  std::shared_ptr<AppState> activeAppState;
  std::map<std::string, std::shared_ptr<AppState>> appStateList;
};

}  // namespace gge