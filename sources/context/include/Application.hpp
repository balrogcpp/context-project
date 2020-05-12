/*
MIT License

Copyright (c) 2020 Andrey Vasiliev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <OgreFrameListener.h>
#include <OgreRenderTargetListener.h>

#include "InputListener.hpp"
#include "DummyListener.hpp"
#include "ManagerCommon.hpp"

namespace Context {
class AppState;
}

namespace Context {

class Application : public ManagerCommon {
 public:

  void Update(float timeSinceLastFrame);
  void Setup() final;
  void Reset() final;
  void Render();
  void WaitFPS();
  void Loop();
  void Go();
  void SetCurState(std::shared_ptr<AppState> scene_ptr);
  void SetNextState(std::shared_ptr<AppState> scene_ptr);
  bool frameRenderingQueued(const Ogre::FrameEvent &evt) override;
  void KeyDown(SDL_Keycode sym) override;
  void KeyUp(SDL_Keycode sym) override;
  void Event(const SDL_Event &evt) override;
  void Quit() override;
  void User(Uint8 type, int code, void *data1, void *data2) override;

 public:
  static int Main(std::shared_ptr<AppState> app_state);

 protected:
  DummyListener dummy_listener_;
  bool running_ = true;
  int fps_frames_ = 0;
  int global_target_fps_ = 60;
  long delta_time_ = 0;
  long time_since_last_frame_ = 0;
  bool global_verbose_fps_ = false;
  bool global_verbose_ = false;
  bool global_verbose_input_ = false;
  bool application_fkeys_enable_ = false;
  bool application_ask_before_quit_ = false;
  bool graphics_vsync_ = true;
  bool global_lock_fps_ = true;
  std::string application_scene_file_;
  std::string application_ambient_sound_file_;

  //Singleton section
 private:
  static Application ApplicationSingleton;

 public:
  static Application &GetSingleton() {
    return ApplicationSingleton;
  }

  static Application *GetSingletonPtr() {
    return &ApplicationSingleton;
  }

};

}
