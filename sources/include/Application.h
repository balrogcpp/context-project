// MIT License
//
// Copyright (c) 2021 Andrew Vasiliev
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once
#include <OgreLog.h>

#include "Engine.h"
#include "Singleton.h"
#include "StateManager.h"
#include "VerboseListener.h"
#include "view_ptr.h"

namespace xio {

class Application final : public WindowObserver, public Ogre::LogListener, public Singleton<Application> {
 public:
  explicit Application();
  virtual ~Application();
  int Main(std::unique_ptr<AppState> &&scene_ptr);

 private:
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
  int current_fps_ = 0;
  int target_fps_ = 60;
  bool lock_fps_ = true;
  std::string log_;
  std::string log_file_ = "Launch.log";
  bool verbose_ = false;
  bool verbose_input_ = false;
};

}  // namespace xio
