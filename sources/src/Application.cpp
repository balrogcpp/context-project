//MIT License
//
//Copyright (c) 2021 Andrew Vasiliev
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

#include "Configurator.h"
#include "pcheader.h"
#include "Application.h"
#include "Overlay.h"
#include "DesktopIcon.h"
#include "Exception.h"
#include "SDL2.hpp"
#include "VerboseListener.h"
#include <iostream>

#ifdef _WIN32
extern "C"
{
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

using namespace std;

namespace xio {
Application::Application() {
  try {
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
    state_ = (android_app*)(state);
#endif

	auto *logger = new Ogre::LogManager();
	logger->createLog("ogre.log", true, false, true);
	Ogre::LogManager::getSingleton().getDefaultLog()->addListener(this);
#ifdef DEBUG
	Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_BOREME);
#else
	Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_LOW);
#endif

#if OGRE_PLATFORM==OGRE_PLATFORM_LINUX
	DesktopIcon icon;
	icon.Init();
	icon.Save("XioDemo");
#endif

	engine_ = make_unique<Engine>();
	state_manager_ = make_unique<StateManager>();

	verbose_ = conf_->Get<bool>("global_verbose");
	lock_fps_ = conf_->Get<bool>("global_lock_fps");
	target_fps_ = conf_->Get<int>("global_target_fps");
  }
  catch (Exception &e) {
	Message_("Exception occurred", e.getDescription());
	throw e;
  }
  catch (Ogre::Exception &e) {
	Message_("Exception occurred (OGRE)", e.getFullDescription());
	throw e;
  }
  catch (exception &e) {
	Message_("Exception occurred (exception)", e.what());
	throw e;
  }
}

//----------------------------------------------------------------------------------------------------------------------
Application::~Application() {

}

//----------------------------------------------------------------------------------------------------------------------
void Application::WriteLogToFile_(const string &file_name) {
  ofstream f;
  f.open(file_name);
  if (f.is_open())
	f << log_;
}
//----------------------------------------------------------------------------------------------------------------------
void Application::PrintLogToConsole_() {
  cout << log_ << flush;
}

//----------------------------------------------------------------------------------------------------------------------
void Application::messageLogged(const string &message, Ogre::LogMessageLevel lml, \
        bool maskDebug, const string &logName, bool &skipThisMessage) {
  log_.append(message);
  log_.append("\n");
#ifdef DEBUG
  if (verbose_)
	cout << message << "\n";
#endif
}

//----------------------------------------------------------------------------------------------------------------------
int Application::GetCurrentFps() const {
  return current_fps_;
}

//----------------------------------------------------------------------------------------------------------------------
void Application::Quit() {
  running_ = false;
}

//----------------------------------------------------------------------------------------------------------------------
void Application::Event(const SDL_Event &evt) {
  if (evt.type==SDL_WINDOWEVENT) {
	if (evt.window.event==SDL_WINDOWEVENT_FOCUS_LOST || evt.window.event==SDL_WINDOWEVENT_MINIMIZED) {
	  suspend_ = true;
//		renderer_->GetWindow().SetCursorStatus(true, false, false);
	  state_manager_->Pause();
	} else if (evt.window.event==SDL_WINDOWEVENT_FOCUS_GAINED) {
	  suspend_ = false;
//		renderer_->GetWindow().SetCursorStatus(false, true, true);
	  state_manager_->Resume();
	}
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Application::Other(uint8_t type, int32_t code, void *data1, void *data2) {

}

//----------------------------------------------------------------------------------------------------------------------
void Application::Loop_() {
  bool suspend_old = false;

  while (running_) {

	if (state_manager_->IsActive()) {
	  auto before_frame = chrono::system_clock::now().time_since_epoch();
	  int64_t micros_before_frame = chrono::duration_cast<chrono::microseconds>(before_frame).count();

	  if (cumulated_time_ > 1e+6) {
		current_fps_ = fps_counter_;
		cumulated_time_ = 0;
		fps_counter_ = 0;
	  }

	  engine_->Capture();

	  if (!suspend_) {
		if (state_manager_->IsDirty()) {
		  engine_->Cleanup();
		  engine_->Refresh();
		  state_manager_->InitNextState();
		} else if (suspend_old) {
		  engine_->Resume();
		  suspend_old = false;
		}

		auto before_update = chrono::system_clock::now().time_since_epoch();
		int64_t micros_before_update = chrono::duration_cast<chrono::microseconds>(before_update).count();
		double frame_time = static_cast<double>(micros_before_update - time_of_last_frame_)/1e+6;
		time_of_last_frame_ = micros_before_update;
		engine_->Update(frame_time);
		state_manager_->Update(frame_time);
		engine_->RenderOneFrame();
	  } else {
		engine_->Pause();
		suspend_old = true;
	  }

#ifdef DEBUG
	  if (verbose_)
		cout << flush;
#endif

	  auto duration_after_render = chrono::system_clock::now().time_since_epoch();
	  auto micros_after_render = chrono::duration_cast<chrono::microseconds>(duration_after_render).count();
	  auto render_time = micros_after_render - micros_before_frame;

	  if (lock_fps_) {
		auto delay = static_cast<int64_t> ((1e+6/target_fps_) - render_time);
		if (delay > 0)
		  this_thread::sleep_for(chrono::microseconds(delay));
	  }

	  auto duration_after_loop = chrono::system_clock::now().time_since_epoch();
	  int64_t micros_after_loop = chrono::duration_cast<chrono::microseconds>(duration_after_loop).count();

	  int64_t time_since_last_frame_ = micros_after_loop - micros_before_frame;
	  cumulated_time_ += time_since_last_frame_;

	  fps_counter_++;
	} else {
	  running_ = false;
	}
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Application::Go_() {
  if (state_manager_->IsActive()) {
	state_manager_->InitCurState();
	running_ = true;
	auto duration_before_update = chrono::system_clock::now().time_since_epoch();
	time_of_last_frame_ = chrono::duration_cast<chrono::microseconds>(duration_before_update).count();
	Loop_();
	if (verbose_) {
	  WriteLogToFile_("ogre.log");
	  PrintLogToConsole_();
	}
  }
}

//----------------------------------------------------------------------------------------------------------------------
int Application::Message_(const string &caption, const string &message) {
  WriteLogToFile_("error.log");
  PrintLogToConsole_();
  cerr << caption << '\n';
  cerr << message << '\n';
#ifdef _WIN32
  MessageBox(nullptr, message.c_str(), caption.c_str(), MB_ICONERROR);
#endif
  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
int Application::Main(unique_ptr <AppState> &&scene_ptr) {
  try {
#if OGRE_COMPILER==OGRE_COMPILER_MSVC
	SDL_SetMainReady();
#endif
#ifndef DEBUG
	ios_base::sync_with_stdio(false);
#else
	auto verbose_listener = make_unique<VerboseListener>();
	//verbose_listener->verbose = true;
#endif
	state_manager_->SetInitialState(move(scene_ptr));
	Go_();
  }
  catch (Exception &e) {
	return Message_("Exception occurred", e.getDescription());
  }
  catch (Ogre::Exception &e) {
	return Message_("Exception occurred (OGRE)", e.getFullDescription());
  }
  catch (exception &e) {
	return Message_("Exception occurred (exception)", e.what());
  }

  return 0;
}

} //namespace
