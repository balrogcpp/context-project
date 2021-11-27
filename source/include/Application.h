// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Engine.h"
#include "Singleton.h"
#include "StateManager.h"
#include "VerboseListener.h"
#include "view_ptr.h"
#include <OgreLog.h>
#include <memory>
#include <string>

namespace glue {

class Application final : public WindowObserver, public Ogre::LogListener, public Singleton<Application> {
 public:
  Application();
  virtual ~Application();
  int Main(std::unique_ptr<AppState> &&scene_ptr);

 protected:
  void Loop_();
  void Go_();
  int ExceptionMessage_(const std::string &caption, const std::string &message);

  void Event(const SDL_Event &evt) override;
  void Quit() override;
  void Pause() override;
  void Resume() override;

  void messageLogged(const std::string &message, Ogre::LogMessageLevel lml, bool maskDebug, const std::string &logName,
                     bool &skipThisMessage) override;

  void WriteLogToFile_();
  void PrintLogToConsole_();

  std::unique_ptr<StateManager> state_manager_;
  std::unique_ptr<VerboseListener> verbose_listener_;
  view_ptr<Engine> engine_;

  bool running_ = true;
  bool suspend_ = false;
  int64_t time_of_last_frame_ = 0;
  int64_t cumulated_time_ = 0;
  int64_t fps_counter_ = 0;
  int64_t current_fps_ = 0;
  int64_t target_fps_ = 60;
  bool lock_fps_ = true;
  std::string log_;
  std::string log_file_ = "Launch.log";
  bool verbose_ = false;
  bool verbose_input_ = false;
};

}  // namespace glue
