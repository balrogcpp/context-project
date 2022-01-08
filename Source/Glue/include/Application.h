// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "AppStateManager.h"
#include "Engine.h"
#include "Singleton.h"
#include "Input/WindowObserver.h"
#include "Input/VerboseListener.h"
#include <memory>
#include <string>

namespace Glue {

class Application final : public WindowObserver, public Singleton<Application> {
 public:
  explicit Application(int argc = 0, char* args[] = nullptr);
  virtual ~Application();
  int Main(std::unique_ptr<AppState> &&AppStatePtr);

 protected:
  void Loop();
  void Go();

  void OnQuit() override;
  void OnPause() override;
  void OnResume() override;

  std::unique_ptr<AppStateManager> StateManagerPtr;
#ifdef DESKTOP
  std::unique_ptr<VerboseListener> VerboseListenerPtr;
#endif
  std::unique_ptr<Engine> EnginePtr;
  std::unique_ptr<Config> ConfigPtr;

  bool Running = false;
  bool Suspend = false;
  int64_t TimeAsLastFrame = 0;
  int64_t CumultedTime = 0;
  int64_t FPSCounter = 0;
  int64_t CurrentFPS = 0;
  int64_t TargetFPS = 60;
  bool LockFPS = true;
  bool Verbose = false;

  /// Print all input in cout (pressed key on keyboard, mouse move, gamepad etc.)
  bool VerboseInput = false;
};

}  // namespace Glue
