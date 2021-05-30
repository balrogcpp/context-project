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
#include "Application.h"
#include "DesktopIcon.h"
#include "Exception.h"
#include "ComponentLocator.h"
#include <iostream>

#ifdef WIN32
extern "C"
{
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

using namespace std;

namespace xio {

//----------------------------------------------------------------------------------------------------------------------
Application::Application() {
  try {

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
	auto *logger = new Ogre::LogManager();


#ifdef DEBUG
	logger->createLog(log_file_, true, true, true);
	Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_BOREME);
#else
	logger->createLog(log_file_, false, false, true);
	Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_NORMAL);
#endif


	Ogre::LogManager::getSingleton().getDefaultLog()->addListener(this);

#endif



#if OGRE_PLATFORM==OGRE_PLATFORM_LINUX
	DesktopIcon icon;
	icon.Init();
	icon.Save("XioDemo");
#endif

	engine_ = &Engine::GetInstance();
	engine_->InitComponents();

	state_manager_ = make_unique<StateManager>();

	GetConf().Get("verbose", verbose_);
	GetConf().Get("verbose_input", verbose_input_);

	if (verbose_)
	  log_.reserve(10000);

	if (verbose_input_)
	  verbose_listener_ = make_unique<VerboseListener>();

	lock_fps_ = GetConf().Get<bool>("lock_fps");
	target_fps_ = GetConf().Get<int>("target_fps");

  }
  catch (Exception &e) {
	ExceptionMessage_("Exception", e.getDescription());
  }
  catch (Ogre::Exception &e) {
	ExceptionMessage_("Exception (OGRE)", e.getFullDescription());
  }
  catch (exception &e) {
	ExceptionMessage_("Exception (std::exception)", e.what());
  }
  catch (...) {
	ExceptionMessage_("Exception", "Unhandled");
  }
}

//----------------------------------------------------------------------------------------------------------------------
Application::~Application() {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
	Ogre::LogManager::getSingleton().getDefaultLog()->removeListener(this);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
int Application::ExceptionMessage_(const string &caption, const string &message) {

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  GetWindow().Grab(false);
#endif

#ifdef _WIN32
  MessageBox(nullptr, message.c_str(), caption.c_str(), MB_ICONERROR);
#else
  SDL_Log("%s", string(caption + " : " + message).c_str());
  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, caption.c_str(), message.c_str(), nullptr);
#endif

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
void Application::WriteLogToFile_() {
  if (!verbose_)
    return;

  ofstream f;
  f.open(log_file_);

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
	cout << message << '\n';
#endif
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
		  engine_->Pause();
		  engine_->Cleanup();
		  state_manager_->InitNextState();
		  engine_->Resume();
		} else if (suspend_old) {
		  engine_->Resume();
		  suspend_old = false;
		}

		auto before_update = chrono::system_clock::now().time_since_epoch();
		int64_t micros_before_update = chrono::duration_cast<chrono::microseconds>(before_update).count();
		float frame_time = static_cast<float>(micros_before_update - time_of_last_frame_)/1e+6;
		time_of_last_frame_ = micros_before_update;
		state_manager_->Update(frame_time);
		engine_->Update(frame_time);

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

  engine_->Pause();
}

//----------------------------------------------------------------------------------------------------------------------
void Application::Go_() {
  if (state_manager_->IsActive()) {
	state_manager_->InitCurState();
	running_ = true;
	auto duration_before_update = chrono::system_clock::now().time_since_epoch();
	time_of_last_frame_ = chrono::duration_cast<chrono::microseconds>(duration_before_update).count();
	Loop_();

  }
}

//----------------------------------------------------------------------------------------------------------------------
void Application::Quit() {
  running_ = false;
}

//----------------------------------------------------------------------------------------------------------------------
void Application::Pause() {
  suspend_ = true;
  state_manager_->Pause();
  engine_->Pause();
}

//----------------------------------------------------------------------------------------------------------------------
void Application::Resume() {
  suspend_ = false;
  state_manager_->Resume();
  engine_->Resume();
}

//----------------------------------------------------------------------------------------------------------------------
void Application::Event(const SDL_Event &evt) {

}

//----------------------------------------------------------------------------------------------------------------------
int Application::Main(unique_ptr <AppState> &&scene_ptr) {
  try {

#if OGRE_COMPILER==OGRE_COMPILER_MSVC
	SDL_SetMainReady();
#endif

	ios_base::sync_with_stdio(false);

	state_manager_->SetInitialState(move(scene_ptr));
	Go_();

	WriteLogToFile_();

	SDL_Quit();

  }
  catch (Exception &e) {
	return ExceptionMessage_("Exception", e.getDescription());
  }
  catch (Ogre::Exception &e) {
	return ExceptionMessage_("Exception (OGRE)", e.getFullDescription());
  }
  catch (exception &e) {
	return ExceptionMessage_("Exception (std::exception)", e.what());
  }
  catch (...) {
	ExceptionMessage_("Exception", "Unhandled");
  }


  return 0;
}

} //namespace
