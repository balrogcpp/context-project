/// created by Andrey Vasiliev

#pragma once
#include "SDLListener.h"
#include "Singleton.h"
#include <map>

namespace Glue {
class AppState : public MouseListener, WindowListener {
 public:
  virtual std::string GetName() = 0;
  virtual void OnSetUp() = 0;
  virtual void OnUpdate(float time) = 0;
  virtual void OnFrameStarted(float time) = 0;
  virtual void OnRender(float time) = 0;
  virtual void OnFrameEnded(float time) = 0;
  virtual void OnCleanup() = 0;
};

class AppStateManager : public DynamicSingleton<AppStateManager> {
 public:
  AppStateManager();
  virtual ~AppStateManager();

  void RegAppState(AppState *appState);
  void UnregAppState(AppState *appState);

  void SetActiveAppState(AppState *appState);
  AppState *GetActiveAppState();

 protected:
  AppState *activeAppState = nullptr;
  std::map<std::string, AppState *> appStateList;
};
}  // namespace Glue
