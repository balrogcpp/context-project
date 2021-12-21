// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "AppStateManager.h"
#include "Engine.h"
#include "LazySingleton.h"
#include "Input/WindowObserver.h"
#include "Input/VerboseListener.h"
#include "Log.h"
#include <memory>
#include <string>

namespace Glue {

class Application final : public WindowObserver, public Singleton<Application> {
 public:
  Application();
  virtual ~Application();
  int Main(std::unique_ptr<AppState> &&AppStatePtr);

 protected:
  void Loop();
  void Go();

  void OnQuit() override;
  void OnPause() override;
  void OnResume() override;

  std::unique_ptr<AppStateManager> StateManagerPtr;
  std::unique_ptr<VerboseListener> VerboseListenerPtr;
  std::unique_ptr<Log> LogPtr;
  Engine *EnginePtr = nullptr;

  bool Running = true;
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
