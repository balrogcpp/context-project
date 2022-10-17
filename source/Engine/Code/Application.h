/// created by Andrey Vasiliev

#pragma once

#include "Engine.h"
#include "Observer.h"
#include "Singleton.h"
#include <memory>
#include <string>

namespace Glue {

class Application : public WindowObserver, public InputObserver, public Singleton<Application> {
 public:
  /// Constructors
  Application();
  virtual ~Application();
  int Main();
  static void EmscriptenLoop(void *arg);

 protected:
  void LoopBody();
  void Loop();
  void Go();

  /// Window observer impl
  void OnQuit() override;
  void OnPause() override;
  void OnResume() override;

  /// Handle components
  std::unique_ptr<Engine> engine;
  bool quit = false;
  bool suspend = false;
  int64_t targetFps = 60;
  bool lockFps = true;
};

}  // namespace Glue
