/// created by Andrey Vasiliev

#pragma once
#include "Engine.h"
#include "Observer.h"
#include "Singleton.h"
#include <memory>
#include <string>

namespace Glue {

class Application final : public WindowObserver, public InputObserver, public Singleton<Application> {
 public:
  /// Constructors
  explicit Application();
  virtual ~Application();
  int Main();
#ifdef EMSCRIPTEN
  static void EmscriptenLoop(void *arg);
#endif

 protected:
  void LoopBody();
  void Loop();
  void Go();

  /// Window observer impl
  void OnQuit() override;
  void OnPause() override;
  void OnResume() override;

  /// Handle components
  std::unique_ptr<Engine> EnginePtr;
  bool Running = false;
  bool Suspend = false;
  bool WasSuspended = false;
  int64_t TimeOfLastFrame = 0;
  int64_t CumultedTime = 0;
  int64_t TargetFPS = 60;
  bool LockFPS = true;
};

}  // namespace Glue
