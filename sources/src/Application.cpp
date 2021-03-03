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

#include "pcheader.h"
#include "Application.h"
#include "Overlay.h"
#include "HwCheck.h"
#include "ComponentLocator.h"
#include "DesktopIcon.h"

#ifdef _WIN32
extern "C"
{
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

using namespace std;

namespace xio {
Application::Application(int argc, char *argv[])
	: target_fps_(60),
	  lock_fps_(true),
	  quit_(false),
	  suspend_(false),
	  time_of_last_frame_(0),
	  cumulated_time_(0),
	  fps_counter_(0),
	  current_fps_(0),
	  verbose_(false),
	  input_(nullptr) {
  try {
	Init_();
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

Application::~Application() = default;
//----------------------------------------------------------------------------------------------------------------------
void Application::WriteLogToFile(const string &file_name) {
  ofstream f;
  f.open(file_name);
  if (f.is_open()) {
	f << log_;
  }
}
//----------------------------------------------------------------------------------------------------------------------
void Application::PrintLogToConsole() {
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
void Application::Init_() {
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

  conf_ = make_unique<YamlConfigurator>("config.yaml");
  Renderer::SetConfigurator(conf_.get());
  int window_width = conf_->Get<int>("window_width");
  int window_high = conf_->Get<int>("window_high");
  bool window_fullscreen = conf_->Get<bool>("window_fullscreen");
  renderer_ = make_unique<Renderer>(window_width, window_high, window_fullscreen);

  // Shadows param
  bool shadow_enable = conf_->Get<bool>("graphics_shadows_enable");
  float shadow_far = conf_->Get<float>("graphics_shadows_far_distance");
  int16_t tex_size = conf_->Get<int16_t>("graphics_shadows_texture_resolution");
  int tex_format = conf_->Get<int>("graphics_shadows_texture_format");
  renderer_->GetShadowSettings().UpdateParams(shadow_enable, shadow_far, tex_size, tex_format);

  input_ = &InputSequencer::GetInstance();
  physics_ = make_unique<Physics>();
  sound_ = make_unique<Sound>();
  overlay_ = make_unique<Overlay>();
  loader_ = make_unique<DotSceneLoaderB>();

  ComponentLocator::LocateComponents(conf_.get(),
									 input_,
									 renderer_.get(),
									 physics_.get(),
									 sound_.get(),
									 overlay_.get(),
									 loader_.get());
  loader_->LocateComponents(conf_.get(), input_, renderer_.get(), physics_.get(), sound_.get(), overlay_.get());

  components_.push_back(sound_.get());
  components_.push_back(loader_.get());
  components_.push_back(physics_.get());
  components_.push_back(renderer_.get());
  components_.push_back(overlay_.get());

  for (auto &it : components_) {
	it->Create();
  }

  // Texture filtering
  string graphics_filtration = conf_->Get<string>("graphics_filtration");
  Ogre::TextureFilterOptions tfo = Ogre::TFO_BILINEAR;
  if (graphics_filtration=="anisotropic")
	tfo = Ogre::TFO_ANISOTROPIC;
  else if (graphics_filtration=="bilinear")
	tfo = Ogre::TFO_BILINEAR;
  else if (graphics_filtration=="trilinear")
	tfo = Ogre::TFO_TRILINEAR;
  else if (graphics_filtration=="none")
	tfo = Ogre::TFO_NONE;

  renderer_->UpdateParams(tfo, conf_->Get<int>("graphics_anisotropy_level"));
  verbose_ = conf_->Get<bool>("global_verbose_enable");
  lock_fps_ = conf_->Get<bool>("global_lock_fps");
  target_fps_ = conf_->Get<int>("global_target_fps");
  renderer_->GetWindow().SetCaption(conf_->Get<string>("window_caption"));
  renderer_->Refresh();
}
//----------------------------------------------------------------------------------------------------------------------
void Application::Reset_() {
  input_->Clear();

  for (auto &it : components_)
	it->Clean();

  for (auto &it : components_)
	it->Reset();

  renderer_.reset();
  physics_.reset();
  sound_.reset();
  overlay_.reset();
  loader_.reset();

  this_thread::sleep_for(chrono::microseconds(200));
}
//----------------------------------------------------------------------------------------------------------------------
void Application::InitState_(unique_ptr <AppState> &&next_state) {
  if (cur_state_) {
	cur_state_->Clear();
	renderer_->Refresh();
	Ogre::Root::getSingleton().removeFrameListener(cur_state_.get());
  }

  cur_state_ = move(next_state);
  Ogre::Root::getSingleton().addFrameListener(cur_state_.get());

  cur_state_->LocateComponents(conf_.get(),
							   input_,
							   renderer_.get(),
							   physics_.get(),
							   sound_.get(),
							   overlay_.get(),
							   loader_.get());
  cur_state_->Create();
}
//----------------------------------------------------------------------------------------------------------------------
int Application::GetCurrentFps() const {
  return current_fps_;
}
//----------------------------------------------------------------------------------------------------------------------
void Application::SetInitialState(std::unique_ptr<AppState> &&state) {
  cur_state_ = move(state);
  Ogre::Root::getSingleton().addFrameListener(cur_state_.get());
}

//----------------------------------------------------------------------------------------------------------------------
void Application::Quit() {
  quit_ = false;
}
//----------------------------------------------------------------------------------------------------------------------
void Application::Event(const SDL_Event &evt) {
  if (evt.type==SDL_WINDOWEVENT) {
	static bool fullscreen = renderer_->GetWindow().IsFullscreen();

	if (!fullscreen) {
	  if (evt.window.event==SDL_WINDOWEVENT_LEAVE || evt.window.event==SDL_WINDOWEVENT_MINIMIZED) {
		suspend_ = true;
		renderer_->GetWindow().SetCursorStatus(true, false, false);
		cur_state_->Pause();
	  } else if (evt.window.event==SDL_WINDOWEVENT_TAKE_FOCUS || evt.window.event==SDL_WINDOWEVENT_RESTORED
		  || evt.window.event==SDL_WINDOWEVENT_MAXIMIZED) {
		suspend_ = false;
		renderer_->GetWindow().SetCursorStatus(false, true, true);
		cur_state_->Unpause();
	  }
	} else {
	  if (evt.window.event==SDL_WINDOWEVENT_MINIMIZED) {
		suspend_ = true;
		renderer_->GetWindow().SetCursorStatus(true, false, false);
		cur_state_->Pause();
	  } else if (evt.window.event==SDL_WINDOWEVENT_TAKE_FOCUS || evt.window.event==SDL_WINDOWEVENT_RESTORED
		  || evt.window.event==SDL_WINDOWEVENT_MAXIMIZED) {
		suspend_ = false;
		renderer_->GetWindow().SetCursorStatus(false, true, true);
		cur_state_->Unpause();
	  }
	}
  }
}
//----------------------------------------------------------------------------------------------------------------------
void Application::Other(uint8_t type, int32_t code, void *data1, void *data2) {}
//----------------------------------------------------------------------------------------------------------------------
void Application::Loop_() {
  while (quit_) {
	if (cur_state_) {
	  auto before_frame = chrono::system_clock::now().time_since_epoch();
	  int64_t micros_before_frame = chrono::duration_cast<chrono::microseconds>(before_frame).count();

	  if (cumulated_time_ > 1e+6) {
		current_fps_ = fps_counter_;
		cumulated_time_ = 0;
		fps_counter_ = 0;
	  }

	  input_->Capture();

	  if (!suspend_) {
		if (cur_state_->IsDirty()) {
		  for (auto &it : components_)
			it->Clean();

		  InitState_(move(cur_state_->GetNextState()));
		} else {
		  for (auto &it : components_)
			it->Resume();
		}

		auto before_update = chrono::system_clock::now().time_since_epoch();
		int64_t micros_before_update = chrono::duration_cast<chrono::microseconds>(before_update).count();
		float frame_time = static_cast<float>(micros_before_update - time_of_last_frame_)/1e+6;
		time_of_last_frame_ = micros_before_update;

		for (auto *it : components_)
		  it->Update(frame_time);

		cur_state_->Update(frame_time);

		renderer_->RenderOneFrame();
	  } else {
		for (auto &it : components_)
		  it->Pause();
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
	  quit_ = true;
	}
  }
}
//----------------------------------------------------------------------------------------------------------------------
void Application::Go_() {
  if (cur_state_) {
	InitState_(move(cur_state_));
	quit_ = true;
	auto duration_before_update = chrono::system_clock::now().time_since_epoch();
	time_of_last_frame_ = chrono::duration_cast<chrono::microseconds>(duration_before_update).count();
	Loop_();
	Reset_();
	if (verbose_) {
	  WriteLogToFile("ogre.log");
	  PrintLogToConsole();
	}
  }
}
//----------------------------------------------------------------------------------------------------------------------
int Application::Message_(const string &caption, const string &message) {
  WriteLogToFile("error.log");
  PrintLogToConsole();
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
#ifndef DEBUG
	ios_base::sync_with_stdio(false);
#endif
	SetInitialState(move(scene_ptr));
	Go_();
  }
  catch (Ogre::Exception &e) {
	return Message_("Exception occurred (OGRE)", e.getFullDescription());
  }
  catch (exception &e) {
	return Message_("Exception occurred (exception)", e.what());
  }

  return 0;
}
}