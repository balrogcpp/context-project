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

#include "pcheader.h"
#include <string>
#include <iostream>
#include "BaseApplication.h"

using namespace std;

namespace xio {
BaseApplication::BaseApplication() {
  try {
	auto *logger = new Ogre::LogManager();
	logger->createLog("ogre.log", true, false, true);
	Ogre::LogManager::getSingleton().getDefaultLog()->addListener(this);

#ifdef DEBUG
	Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_BOREME);
#else
	Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_LOW);
#endif

	engine_ = make_unique<Engine>();
	state_manager_ = make_unique<StateManager>();
  }
  catch (Exception &e) {
	Message_("Exception", e.getDescription());
	throw e;
  }
  catch (Ogre::Exception &e) {
	Message_("Exception (OGRE)", e.getFullDescription());
	throw e;
  }
  catch (exception &e) {
	Message_("Exception (exception)", e.what());
	throw e;
  }
}

//----------------------------------------------------------------------------------------------------------------------
BaseApplication::~BaseApplication() {

}

//----------------------------------------------------------------------------------------------------------------------
int BaseApplication::Message_(const string &caption, const string &message) {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  WriteLogToFile_("error.log");
  PrintLogToConsole_();
  cerr << caption << '\n' << message << '\n';
#ifdef _WIN32
  MessageBox(nullptr, message.c_str(), caption.c_str(), MB_ICONERROR);
#endif
#endif

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
void BaseApplication::WriteLogToFile_(const string &file_name) {
  ofstream f;
  f.open(file_name);
  if (f.is_open())
	f << log_;
}
//----------------------------------------------------------------------------------------------------------------------
void BaseApplication::PrintLogToConsole_() {
  cout << log_ << flush;
}

//----------------------------------------------------------------------------------------------------------------------
void BaseApplication::messageLogged(const string &message, Ogre::LogMessageLevel lml, \
        bool maskDebug, const string &logName, bool &skipThisMessage) {
  log_.append(message);
  log_.append("\n");
  if (verbose_)
	cout << message << "\n";
}

//----------------------------------------------------------------------------------------------------------------------
int BaseApplication::GetCurrentFps() const {
  return current_fps_;
}

//----------------------------------------------------------------------------------------------------------------------
void BaseApplication::Go_() {
  OnGo();

  if (state_manager_->IsActive()) {
	state_manager_->InitCurState();
	running_ = true;
	auto duration_before_update = chrono::system_clock::now().time_since_epoch();
	time_of_last_frame_ = chrono::duration_cast<chrono::microseconds>(duration_before_update).count();
	Loop_();

//	if (verbose_) {
//	  WriteLogToFile_("ogre.log");
//	  PrintLogToConsole_();
//	}

  }
}

//----------------------------------------------------------------------------------------------------------------------
void BaseApplication::Loop_() {
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
int BaseApplication::Main(unique_ptr <AppState> &&scene_ptr) {
  try {
#if OGRE_PLATFORM==OGRE_PLATFORM_WIN32
	SetProcessDPIAware();
#endif
#if OGRE_COMPILER==OGRE_COMPILER_MSVC
	SDL_SetMainReady();
#endif

	ios_base::sync_with_stdio(false);

    OnMain();
	state_manager_->SetInitialState(move(scene_ptr));
	Go_();
  }
  catch (Exception &e) {
	return Message_("Exception", e.getDescription());
  }
  catch (Ogre::Exception &e) {
	return Message_("Exception (OGRE)", e.getFullDescription());
  }
  catch (exception &e) {
	return Message_("Exception (exception)", e.what());
  }

  return 0;
}

} //namespace
