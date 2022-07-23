// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include "AppStateManager.h"
#include "Engine.h"
#include "Input/InputObserver.h"
#include "Input/VerboseListener.h"
#include "Input/WindowObserver.h"
#include "Singleton.h"
#include <memory>
#include <string>

namespace Glue {

class Application final : public WindowObserver, public InputObserver, public Singleton<Application> {
 public:
  /// Constructors
  explicit Application(int argc = 0, char *args[] = nullptr);
  virtual ~Application();
  int Main(std::unique_ptr<AppState> &&AppStatePtr);
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

  /// Input observer impl
  void OnKeyDown(SDL_Keycode sym) override;
  void OnKeyUp(SDL_Keycode sym) override;
  void OnMouseMove(int dx, int dy) override;
  void OnMouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) override;
  void OnMouseWheel(int x, int y) override;
  void OnMouseLbDown(int x, int y) override;
  void OnMouseLbUp(int x, int y) override;
  void OnMouseRbDown(int x, int y) override;
  void OnMouseRbUp(int x, int y) override;
  void OnMouseMbDown(int x, int y) override;
  void OnMouseMbUp(int x, int y) override;
  void OnTextInput(const char *text) override;
  void OnGamepadAxis(int which, int axis, int value) override;
  void OnGamepadBtDown(int which, int button) override;
  void OnGamepadBtUp(int which, int button) override;
  void OnGamepadHat(int which, int hat, int value) override;
  void OnGamepadBall(int which, int ball, int xrel, int yrel) override;

  /// Handle AppStates
  std::unique_ptr<AppStateManager> StateManagerPtr;
  /// Handle components
  std::unique_ptr<Engine> EnginePtr;
  /// Read global variables from file
  std::unique_ptr<Config> ConfigPtr;
#ifdef DESKTOP
  /// Enable/Disable Log
  bool Verbose = false;
  /// Print all input in cout (pressed key on keyboard, mouse move, gamepad etc.)
  bool VerboseInput = false;
  std::unique_ptr<VerboseListener> VerboseListenerPtr;
  std::unique_ptr<Log> LogPtr;
#endif
  bool Running = false;
  bool Suspend = false;
  bool WasSuspended = false;
  int64_t TimeOfLastFrame = 0;
  int64_t CumultedTime = 0;
  int64_t TargetFPS = 60;
  bool LockFPS = true;
};

}  // namespace Glue
