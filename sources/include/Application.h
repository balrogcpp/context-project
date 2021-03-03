//MIT License
//
//Copyright (c) 2020 Andrey Vasiliev
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#pragma once
#include <OgreFrameListener.h>
#include <OgreRenderTargetListener.h>
#include <OgreLog.h>
#include "Input.h"
#include "Renderer.h"
#include "Sound.h"
#include "Physics.h"
#include "DotSceneLoaderB.h"
#include "Overlay.h"
#include "AppState.h"
#include "YamlConfigurator.h"
#include "view_ptr.h"

namespace xio {
class Application final : public WindowObserver, public Ogre::LogListener {
 public:
  explicit Application(int argc, char *argv[]);
  virtual ~Application();
  int Main(std::unique_ptr<AppState> &&scene_ptr);
  void SetInitialState(std::unique_ptr<AppState> &&state);
  int GetCurrentFps() const;

 private:
  void Init_();
  void Reset_();
  void Loop_();
  void Go_();
  void InitState_(std::unique_ptr<AppState> &&next_state);
  int Message_(const std::string &caption, const std::string &message);
  void Event(const SDL_Event &evt) final;
  void Other(uint8_t type, int32_t code, void *data1, void *data2) final;
  void Quit() final;

  void messageLogged(const std::string &message, Ogre::LogMessageLevel lml, \
        bool maskDebug, const std::string &logName, bool &skipThisMessage) final;

  void WriteLogToFile(const std::string &file_name);
  void PrintLogToConsole();

  view_ptr<InputSequencer> input_;
  std::unique_ptr<YamlConfigurator> conf_;
  std::unique_ptr<Renderer> renderer_;
  std::unique_ptr<Physics> physics_;
  std::unique_ptr<Sound> sound_;
  std::unique_ptr<Overlay> overlay_;
  std::unique_ptr<DotSceneLoaderB> loader_;
  std::unique_ptr<AppState> cur_state_;
  bool quit_;
  bool suspend_;
  int64_t time_of_last_frame_;
  int64_t cumulated_time_;
  int64_t fps_counter_;
  int current_fps_;
  int target_fps_;
  bool verbose_;
  bool lock_fps_;
  std::vector<view_ptr<Component>> components_;
  std::string log_;
};
}
