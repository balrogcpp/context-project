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

#include "pcheader.hpp"

#include "Application.hpp"
#include "ContextManager.hpp"

#include "AppStateManager.hpp"
#include "ConfigManager.hpp"
#include "Exception.hpp"
#include "PhysicsManager.hpp"
#include "SoundManager.hpp"

#include <csetjmp>
#include <csignal>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <locale>
#endif

#ifdef _WIN32
extern "C"
{
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

extern "C"
{
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

namespace Context {
Application Application::ApplicationSingleton;
//----------------------------------------------------------------------------------------------------------------------
void Application::Update(float timeSinceLastFrame) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void Application::Setup() {
  ContextManager::GetSingleton().SetupGlobal();
  ContextManager::GetSingleton().Setup();
  ConfigManager::GetSingleton().SetupGlobal();
  AppStateManager::GetSingleton().cur_state_->SetupGlobals();

  ConfigManager::Assign(global_verbose_fps_, "global_verbose_fps");
  ConfigManager::Assign(global_verbose_, "global_verbose_enable");
  ConfigManager::Assign(global_verbose_input_, "global_verbose_input");
  ConfigManager::Assign(application_fkeys_enable_, "application_fkeys_enable");

  ConfigManager::Assign(global_target_fps_, "global_target_fps");
  ConfigManager::Assign(global_lock_fps_, "global_lock_fps");
  ConfigManager::Assign(graphics_vsync_, "graphics_vsync");
  ConfigManager::Assign(application_ask_before_quit_, "application_ask_before_quit");
  ConfigManager::Assign(application_scene_file_, "application_scene_file");
  ConfigManager::Assign(application_ambient_sound_file_, "application_ambient_sound_file");
}
//----------------------------------------------------------------------------------------------------------------------
void Application::Render() {
  ContextManager::GetSingleton().Render();
}
//----------------------------------------------------------------------------------------------------------------------
void Application::Reset() {
  AppStateManager::GetSingleton().Reset();
  AppStateManager::GetSingleton().ResetGlobals();
  ContextManager::GetSingleton().Reset();
  ContextManager::GetSingleton().ResetGlobals();
}
//----------------------------------------------------------------------------------------------------------------------
void Application::Quit() {
  quit_ = false;
}
//----------------------------------------------------------------------------------------------------------------------
void Application::WaitFPS() {
  long delay = static_cast<long> ((1000000 / global_target_fps_) - time_since_last_frame_);

  if (delay > 0) {
    std::this_thread::sleep_for
    (std::chrono::microseconds(delay));
  }
}
//----------------------------------------------------------------------------------------------------------------------
void Application::Loop() {
  while (quit_) {
    if (AppStateManager::GetSingleton().cur_state_) {

      auto duration_before_frame = std::chrono::system_clock::now().time_since_epoch();
      long millis_before_frame = std::chrono::duration_cast<std::chrono::microseconds>(duration_before_frame).count();

      if (delta_time_ > 1000000) {
        if (global_verbose_fps_) {
          std::cout << "FPS " << fps_frames_ << '\n';
        }

        current_fps_ = fps_frames_;
        delta_time_ = 0;
        fps_frames_ = 0;
      }

      if (delta_time_ > 500000)
        started_ = true;

      io::InputManager::GetSingleton().Capture();

      if (!paused_) {
        AppStateManager::GetSingleton().CleanupResources();

        Render();
      }

#ifdef DEBUG
      if (global_verbose_) {
        std::cout << std::flush;
      }
#endif

      auto duration_after_render = std::chrono::system_clock::now().time_since_epoch();
      long millis_after_render = std::chrono::duration_cast<std::chrono::microseconds>(duration_after_render).count();
      long render_time = millis_after_render - millis_before_frame;

      if (global_lock_fps_) {
        long delay = static_cast<long> ((1000000 / global_target_fps_) - render_time);
        if (delay > 0) {
          std::this_thread::sleep_for(std::chrono::microseconds (delay));
        }
      }

      auto duration_after_loop = std::chrono::system_clock::now().time_since_epoch();
      long millis_after_loop = std::chrono::duration_cast<std::chrono::microseconds>(duration_after_loop).count();

      time_since_last_frame_ = millis_after_loop - millis_before_frame;
      delta_time_ += time_since_last_frame_;

      fps_frames_++;
    } else {
      Quit();
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void Application::Go() {
  quit_ = false;

  ConfigManager::GetSingleton().Setup();
  std::ios_base::sync_with_stdio(false);

#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
  putenv("LD_LIBRARY_PATH=.");
#endif

#ifdef DEBUG
  if (!global_verbose_) {
#endif
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
    putenv("ALROUTER_LOGFILE=/dev/null");
    putenv("ALSOFT_LOGLEVEL=LOG_NONE");
#elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    _putenv("ALROUTER_LOGFILE=/dev/null");
    _putenv("ALSOFT_LOGLEVEL=LOG_NONE");
#endif
#ifdef DEBUG
  }
#endif

  if (AppStateManager::GetSingleton().GetCurState()) {
    io::InputManager::GetSingleton().RegisterListener(&dummy_listener_);
    if (global_verbose_input_) {
      dummy_listener_.SetPrint(true);
    }

    Setup();
    SetupGlobal();
    io::InputManager::GetSingleton().RegisterListener(this);
    ContextManager::GetSingleton().GetOgreRootPtr()->addFrameListener(this);

    fullscreen_ = ContextManager::GetSingleton().IsFullscreen();

    if (!application_scene_file_.empty()) {
      Ogre::SceneLoaderManager::getSingleton().load(application_scene_file_,
                                                    Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME,
                                                    ogre_scene_manager_->getRootSceneNode());
    }

    if (!application_ambient_sound_file_.empty()) {
      auto *mSoundManager = OgreOggSound::OgreOggSoundManager::getSingletonPtr();

      mSoundManager->setResourceGroupName(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
      mSoundManager->createSound("AmbientMusic",
                                 application_ambient_sound_file_,
                                 false,
                                 true,
                                 true,
                                 ContextManager::GetSingleton().GetOgreScenePtr());

      if (mSoundManager->getSound("SceneManagerInstance1")) {
        mSoundManager->getSound("SceneManagerInstance1")->play();
      }
    }

    AppStateManager::GetSingleton().cur_state_->Setup();

    quit_ = true;

    Loop();

    io::InputManager::GetSingleton().StopCapture();
    io::InputManager::GetSingleton().Reset();
    io::InputManager::GetSingleton().UnregisterListener(&dummy_listener_);

    Reset();
  }
}
//----------------------------------------------------------------------------------------------------------------------
void Application::SetCurState(std::shared_ptr<AppState> scene_ptr) {
  AppStateManager::GetSingleton().SetInitialState(scene_ptr);
}
//----------------------------------------------------------------------------------------------------------------------
void Application::SetNextState(std::shared_ptr<AppState> scene_ptr) {
  AppStateManager::GetSingleton().SetNextState(scene_ptr);
}
//----------------------------------------------------------------------------------------------------------------------
bool Application::frameRenderingQueued(const Ogre::FrameEvent &evt) {
  return true;
}
//----------------------------------------------------------------------------------------------------------------------
void Application::KeyDown(SDL_Keycode sym) {
  if (application_fkeys_enable_) {
    if (SDL_GetScancodeFromKey(sym) == SDL_SCANCODE_F12) {
      Quit();
    } else if (SDL_GetScancodeFromKey(sym) == SDL_SCANCODE_F11) {
      ContextManager::GetSingleton().GetOgreCamera()->setPolygonMode(Ogre::PM_WIREFRAME);
    } else if (SDL_GetScancodeFromKey(sym) == SDL_SCANCODE_F10) {
      ContextManager::GetSingleton().GetOgreCamera()->setPolygonMode(Ogre::PM_SOLID);
    } else if (SDL_GetScancodeFromKey(sym) == SDL_SCANCODE_F9) {
      PhysicsManager::GetSingleton().SetPhysicsDebugShowCollider(true);
    } else if (SDL_GetScancodeFromKey(sym) == SDL_SCANCODE_F8) {
      PhysicsManager::GetSingleton().SetPhysicsDebugShowCollider(false);
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void Application::KeyUp(SDL_Keycode sym) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void mouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
void Application::Event(const SDL_Event &evt) {
  if (!started_)
    return;

  const int fps_inactive = 30;

  if (evt.type == SDL_WINDOWEVENT) {
    if (!fullscreen_) {
      if (evt.window.event == SDL_WINDOWEVENT_LEAVE || evt.window.event == SDL_WINDOWEVENT_MINIMIZED) {
        paused_ = true;
        global_target_fps_ = fps_inactive;
        global_lock_fps_ = true;
      } else if (evt.window.event == SDL_WINDOWEVENT_TAKE_FOCUS || evt.window.event == SDL_WINDOWEVENT_RESTORED
          || evt.window.event == SDL_WINDOWEVENT_MAXIMIZED) {
        paused_ = false;
        ConfigManager::Assign(global_target_fps_, "global_target_fps");
        ConfigManager::Assign(global_lock_fps_, "global_lock_fps");
      }
    } else {
      if (evt.window.event == SDL_WINDOWEVENT_MINIMIZED) {
        paused_ = true;
        global_target_fps_ = fps_inactive;
        global_lock_fps_ = true;
      } else if (evt.window.event == SDL_WINDOWEVENT_TAKE_FOCUS || evt.window.event == SDL_WINDOWEVENT_RESTORED
          || evt.window.event == SDL_WINDOWEVENT_MAXIMIZED) {
        paused_ = false;
        ConfigManager::Assign(global_target_fps_, "global_target_fps");
        ConfigManager::Assign(global_lock_fps_, "global_lock_fps");
      }
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void Application::Other(Uint8 type, int code, void *data1, void *data2) {
  //
}
//----------------------------------------------------------------------------------------------------------------------
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
static sigjmp_buf point;

static void termination_handler(int signum) {
  ContextManager::GetSingleton().RestoreScreenSize();

  printf("\nSegmentation fault occured!\n");
  std::fflush(stdout);
  std::fflush(stderr);

  longjmp(point, 1);
}

#elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
static void termination_handler(int signum) {
  ContextManager::GetSingleton().RestoreScreenSize();

  printf("\nSegmentation fault occured!\n");
  std::fflush(stdout);
  std::fflush(stderr);
}
#endif

static void do_segv() {
  int *segv;

  segv = 0; /* malloc(a_huge_amount); */

  *segv = 1;
}

int Application::Main(std::shared_ptr<AppState> app_state) {
#ifdef _MSC_VER
  SDL_SetMainReady();
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
  struct sigaction sa{};
  sa.sa_handler = termination_handler;

  sigaction(SIGSEGV, &sa, nullptr);
#elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  typedef void (*SignalHandlerPointer)(int);

  SignalHandlerPointer previousHandler;
  previousHandler = signal(SIGSEGV, termination_handler);
#endif

  int return_value = 0;
  bool exception_occured = false;

  try {
    Application::GetSingleton().SetCurState(app_state);
    Application::GetSingleton().Go();
  }
  catch (Exception &e) {
    const std::string caption = "Exception occurred (Context core)";
    std::cerr << caption << std::endl;
    std::stringstream message;
    message << e.getDescription() << std::endl;
    std::cerr << message.str();

#ifdef _WIN32
    MessageBox(nullptr, message.str().c_str(), caption.c_str(), MB_ICONERROR);
#endif

    return_value = 1;
    exception_occured = true;
  }
  catch (Ogre::Exception &e) {
    const std::string caption = "Exception occurred (OGRE)";
    std::cerr << caption << std::endl;
    std::stringstream message;
    message << e.getFullDescription() << std::endl;
    std::cerr << message.str();

#ifdef _WIN32
    MessageBox(nullptr, message.str().c_str(), caption.c_str(), MB_ICONERROR);
#endif

    return_value = 1;
    exception_occured = true;
  }
  catch (std::exception &e) {
    const std::string caption = "Exception occurred (std::exception)";
    std::cerr << caption << std::endl;
    std::cerr << e.what() << std::endl;
#ifdef _WIN32
    MessageBox(nullptr, e.what(), caption.c_str(), MB_ICONERROR);
#endif

    return_value = 1;
    exception_occured = true;
  }
  catch (...) {
    const std::string caption = "Unhandled exception occurred\n";
    std::cerr << caption << std::endl;
#ifdef _WIN32
    MessageBox(nullptr, caption.c_str(), caption.c_str(), MB_ICONERROR);
#endif

    return_value = 1;
    exception_occured = true;
  }

  if (exception_occured) {
    ContextManager::GetSingleton().RestoreScreenSize();
  }

#if defined _WIN32 && defined DEBUG
  if (ConfigManager::GetSingleton().GetBool("application_ask_before_quit")) {
    std::cout << std::endl;
    std::cout << "Press any key to end program..." << std::endl;
    getchar();
  }
#endif

#ifdef _WIN32
  SDL_Quit();
#endif

  return return_value;
}
int Application::GetFpsFrames() const {
  return current_fps_;
}

}
