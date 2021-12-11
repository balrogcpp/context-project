// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "AppStateManager.h"
#include "Engine.h"
#include "LazySingleton.h"
#include "PhysicalInput/WindowObserver.h"
#include "PhysicalInput/VerboseListener.h"
#include <OgreLog.h>
#include <memory>
#include <string>

namespace Glue {

class Application final : public WindowObserver, public Ogre::LogListener, public Singleton<Application> {
 public:
  Application();
  virtual ~Application();
  int Main(std::unique_ptr<AppState> &&AppStatePtr);

 private:
  void Loop();
  void Go();
  int ExceptionMessage(const std::string &WindowCaption, const std::string &MessageText);

  void OnEvent(const SDL_Event &Event) override;
  void OnQuit() override;
  void OnPause() override;
  void OnResume() override;

  void messageLogged(const std::string &message, Ogre::LogMessageLevel lml, bool maskDebug, const std::string &logName,
                     bool &skipThisMessage) override;

  void WriteLogToFile();
  void PrintLogToConsole();

  std::unique_ptr<AppStateManager> StateManagerPtr;
  std::unique_ptr<VerboseListener> VerboseListenerPtr;
  Engine *EnginePtr = nullptr;

  bool Running = true;
  bool Suspend = false;
  int64_t TimeAsLastFrame = 0;
  int64_t CumultedTime = 0;
  int64_t FPSCounter = 0;
  int64_t CurrentFPS = 0;
  int64_t TargetFPS = 60;
  bool LockFPS = true;
  std::string LogBuffer;
  std::string LogFileName = "Launch.log";
  bool Verbose = false;

  /// Print all input in cout (pressed key on keyboard, mouse move, gamepad etc.)
  bool VerboseInput = false;
};

}  // namespace Glue
