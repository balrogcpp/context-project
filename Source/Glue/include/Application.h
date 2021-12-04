// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Engine.h"
#include "Singleton.h"
#include "StateManager.h"
#include "VerboseListener.h"
#include <OgreLog.h>
#include <memory>
#include <string>

namespace Glue {

class Application final : public WindowObserver, public Ogre::LogListener, public Singleton<Application> {
 public:
  Application();
  virtual ~Application();
  int Main(std::unique_ptr<AppState> &&scene_ptr);

 private:
  void Loop();
  void Go();
  int ExceptionMessage(const std::string &caption, const std::string &message);

  void Event(const SDL_Event &evt) override;
  void Quit() override;
  void Pause() override;
  void Resume() override;

  void messageLogged(const std::string &message, Ogre::LogMessageLevel lml, bool maskDebug, const std::string &logName,
                     bool &skipThisMessage) override;

  void WriteLogToFile();
  void PrintLogToConsole();

  std::unique_ptr<StateManager> state_manager_;
  std::unique_ptr<VerboseListener> verbose_listener_;
  Engine* engine;

  bool running = true;
  bool suspend = false;
  int64_t time_of_last_frame = 0;
  int64_t cumulated_time = 0;
  int64_t fps_counter = 0;
  int64_t current_fps = 0;
  int64_t target_fps_ = 60;
  bool lock_fps = true;
  std::string log;
  std::string log_file = "Launch.log";
  bool verbose = false;
  bool verbose_input = false;
};

}  // namespace glue
