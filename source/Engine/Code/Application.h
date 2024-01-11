/// created by Andrey Vasiliev

#pragma once
#include "AppStateManager.h"
#include "SDLListener.h"
#include "Singleton.h"
#include "SystemLocator.h"

namespace gge {

class Application final : public WindowListener, public DynamicSingleton<Application> {
 public:
  /// Constructors
  Application();
  virtual ~Application();
  void Init();
  int Main();
  static void EmscriptenLoop(void *arg);

 protected:
  void LoopBody();
  void Loop();
  void Go();

  /// Window observer impl
  void OnQuit() override;
  void OnFocusLost() override;
  void OnFocusGained() override;

  /// Handle components
  std::unique_ptr<AppStateManager> appStateManager;
  std::unique_ptr<SystemLocator> engine;
  bool exiting;
  bool sleep;
};

}  // namespace gge
