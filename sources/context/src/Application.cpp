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

#include "pcheader.h"

#include "Application.h"
#include "Graphics.h"
#include "StaticForest.h"
#include "Physics.h"
#include "DotSceneLoaderB.h"
#include "AppStateManager.h"
#include "ConfiguratorJson.h"
#include "Exception.h"

#include <csetjmp>
#include <csignal>

#ifdef _WIN32
#include <windows.h>
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
Application::Application() = default;
Application::~Application() = default;

//----------------------------------------------------------------------------------------------------------------------
void Application::Init_() {
  Graphics::GetSingleton();
  AppStateManager::GetSingleton().GetCurState()->SetupGlobals();

  ConfiguratorJson::Assign(global_verbose_fps_, "global_verbose_fps");
  ConfiguratorJson::Assign(global_verbose_, "global_verbose_enable");
  ConfiguratorJson::Assign(global_verbose_input_, "global_verbose_input");
  ConfiguratorJson::Assign(application_fkeys_enable_, "application_fkeys_enable");

  ConfiguratorJson::Assign(global_target_fps_, "global_target_fps");
  ConfiguratorJson::Assign(global_lock_fps_, "global_lock_fps");
  ConfiguratorJson::Assign(graphics_vsync_, "graphics_vsync");
  ConfiguratorJson::Assign(application_ask_before_quit_, "application_ask_before_quit");

  io::InputSequencer::GetSingleton().RegEventListener(this);

  if (!global_verbose_) {
    auto *logger = new Ogre::LogManager();
    std::string log_name = ConfiguratorJson::GetSingleton().GetString("log_name");
    if (log_name.empty())
      log_name = "Ogre.log";

    logger->createLog(log_name, true, false, true);
    Ogre::LogManager::getSingleton().getDefaultLog()->addListener(this);
    Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_BOREME);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void Application::Render_() {
  Graphics::GetSingleton().Render();
}
//----------------------------------------------------------------------------------------------------------------------
void Application::Reset_() {
  StaticForest::GetSingleton().Reset();
  DotSceneLoaderB::GetSingleton().Reset();
  Ogre::Root::getSingleton().getSceneManager("Default")->destroyAllEntities();
  Ogre::Root::getSingleton().getSceneManager("Default")->destroyAllLights();
  Ogre::Root::getSingleton().getSceneManager("Default")->destroyAllParticleSystems();
  Ogre::Root::getSingleton().getSceneManager("Default")->destroyAllAnimations();
  Ogre::Root::getSingleton().getSceneManager("Default")->destroyAllAnimationStates();
  Ogre::Root::getSingleton().getSceneManager("Default")->destroyAllStaticGeometry();
  Ogre::Root::getSingleton().getSceneManager("Default")->destroyAllManualObjects();
  Ogre::Root::getSingleton().getSceneManager("Default")->destroyAllInstanceManagers();
  Ogre::Root::getSingleton().getSceneManager("Default")->destroyAllBillboardChains();
  Ogre::Root::getSingleton().getSceneManager("Default")->destroyAllBillboardSets();
  Ogre::Root::getSingleton().getSceneManager("Default")->destroyAllMovableObjects();
  Ogre::Root::getSingleton().getSceneManager("Default")->destroyAllCameras();
  Ogre::Root::getSingleton().getSceneManager("Default")->getRootSceneNode()->removeAndDestroyAllChildren();
  Ogre::ResourceGroupManager::getSingleton().unloadResourceGroup(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  AppStateManager::GetSingleton().ResetGlobals();
}
//----------------------------------------------------------------------------------------------------------------------
void Application::Quit() {
  quit_ = false;
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
        ConfiguratorJson::Assign(global_target_fps_, "global_target_fps");
        ConfiguratorJson::Assign(global_lock_fps_, "global_lock_fps");
      }
    } else {
      if (evt.window.event == SDL_WINDOWEVENT_MINIMIZED) {
        paused_ = true;
        global_target_fps_ = fps_inactive;
        global_lock_fps_ = true;
      } else if (evt.window.event == SDL_WINDOWEVENT_TAKE_FOCUS || evt.window.event == SDL_WINDOWEVENT_RESTORED
          || evt.window.event == SDL_WINDOWEVENT_MAXIMIZED) {
        paused_ = false;
        ConfiguratorJson::Assign(global_target_fps_, "global_target_fps");
        ConfiguratorJson::Assign(global_lock_fps_, "global_lock_fps");
      }
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void Application::Other(Uint8 type, int32_t code, void *data1, void *data2) {}
//----------------------------------------------------------------------------------------------------------------------
void Application::Loop_() {
  while (quit_) {
    if (AppStateManager::GetSingleton().GetCurState()) {

      auto duration_before_frame = std::chrono::system_clock::now().time_since_epoch();
      long millis_before_frame = std::chrono::duration_cast<std::chrono::microseconds>(duration_before_frame).count();

      if (delta_time_ > 1000000) {
        if (global_verbose_fps_)
          std::cout << "FPS " << fps_frames_ << '\n';

        current_fps_ = fps_frames_;
        delta_time_ = 0;
        fps_frames_ = 0;
      }

      if (delta_time_ > 500000)
        started_ = true;

      io::InputSequencer::GetSingleton().Capture();

      if (!paused_) {
        if (AppStateManager::GetSingleton().IsWaiting()) {
          StaticForest::GetSingleton().Reset();
          DotSceneLoaderB::GetSingleton().Reset();

          if (ConfiguratorJson::GetSingleton().GetBool("physics_enable")) {
            Physics::GetSingleton().Reset();
          }

          Ogre::Root::getSingleton().getSceneManager("Default")->setShadowTechnique(Ogre::SHADOWTYPE_NONE);

          Ogre::Root::getSingleton().getSceneManager("Default")->destroyAllEntities();
          Ogre::Root::getSingleton().getSceneManager("Default")->destroyAllLights();
          Ogre::Root::getSingleton().getSceneManager("Default")->destroyAllParticleSystems();
          Ogre::Root::getSingleton().getSceneManager("Default")->destroyAllAnimations();
          Ogre::Root::getSingleton().getSceneManager("Default")->destroyAllAnimationStates();
          Ogre::Root::getSingleton().getSceneManager("Default")->destroyAllStaticGeometry();
          Ogre::Root::getSingleton().getSceneManager("Default")->destroyAllRibbonTrails();
          Ogre::Root::getSingleton().getSceneManager("Default")->destroyAllManualObjects();
          Ogre::Root::getSingleton().getSceneManager("Default")->destroyAllInstanceManagers();
          Ogre::Root::getSingleton().getSceneManager("Default")->destroyAllBillboardChains();
          Ogre::Root::getSingleton().getSceneManager("Default")->destroyAllBillboardSets();
          Ogre::Root::getSingleton().getSceneManager("Default")->destroyAllMovableObjects();

          Ogre::Root::getSingleton().getSceneManager("Default")->getRootSceneNode()->removeAndDestroyAllChildren();

          Graphics::GetSingleton().CreateScene();
          AppStateManager::GetSingleton().InitCurrState();
          Physics::GetSingleton().Start();
          AppStateManager::GetSingleton().ClearWaiting();
        }

        Render_();
      }

#ifdef DEBUG
      if (global_verbose_)
        std::cout << std::flush;
#endif

      auto duration_after_render = std::chrono::system_clock::now().time_since_epoch();
      long millis_after_render = std::chrono::duration_cast<std::chrono::microseconds>(duration_after_render).count();
      long render_time = millis_after_render - millis_before_frame;

      if (global_lock_fps_) {
        long delay = static_cast<long> ((1000000 / global_target_fps_) - render_time);
        if (delay > 0)
          std::this_thread::sleep_for(std::chrono::microseconds(delay));
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
void Application::Go_() {
  quit_ = false;

  if (AppStateManager::GetSingleton().GetCurState()) {
    io::InputSequencer::GetSingleton().RegisterListener(&dummy_listener_);
    if (global_verbose_input_) {
      dummy_listener_.SetPrint(true);
    }

    Init_();

    AppStateManager::GetSingleton().GetCurState()->Setup();

    quit_ = true;

    Loop_();

    io::InputSequencer::GetSingleton().Reset();

    Reset_();
  }
}
//----------------------------------------------------------------------------------------------------------------------
void Application::SetCurState(std::shared_ptr<AppState> scene_ptr) {
  AppStateManager::GetSingleton().SetInitialState(scene_ptr);
}
//----------------------------------------------------------------------------------------------------------------------
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
static sigjmp_buf point;

static void termination_handler(int signum) {

  printf("\nSegmentation fault occured!\n");
  std::fflush(stdout);
  std::fflush(stderr);

  longjmp(point, 1);
}
#elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
static void termination_handler(int signum) {
  printf("\nSegmentation fault occured!\n");
  std::fflush(stdout);
  std::fflush(stderr);
}
#endif
//----------------------------------------------------------------------------------------------------------------------
int Application::Main(std::shared_ptr<AppState> scene_ptr) {
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

#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
  putenv("LD_LIBRARY_PATH=.");
#endif

#ifdef DEBUG
#endif
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
  putenv("ALROUTER_LOGFILE=/dev/null");
  putenv("ALSOFT_LOGLEVEL=LOG_NONE");
#elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  _putenv("ALROUTER_LOGFILE=/dev/null");
  _putenv("ALSOFT_LOGLEVEL=LOG_NONE");
#endif
#ifdef DEBUG
#endif

  std::ios_base::sync_with_stdio(false);

  try {
    Application::GetSingleton().SetCurState(scene_ptr);
    Application::GetSingleton().Go_();
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
  }
  catch (std::exception &e) {
    const std::string caption = "Exception occurred (std::exception)";
    std::cerr << caption << std::endl;
    std::cerr << e.what() << std::endl;
#ifdef _WIN32
    MessageBox(nullptr, e.what(), caption.c_str(), MB_ICONERROR);
#endif

    return_value = 1;
  }
  catch (...) {
    const std::string caption = "Unhandled exception occurred\n";
    std::cerr << caption << std::endl;
#ifdef _WIN32
    MessageBox(nullptr, caption.c_str(), caption.c_str(), MB_ICONERROR);
#endif

    return_value = 1;
  }

#if defined _WIN32 && defined DEBUG
  if (ConfigManager::GetSingleton().GetBool("application_ask_before_quit")) {
    std::cout << std::endl;
    std::cout << "Press any key to end program..." << std::endl;
    getchar();
  }
#endif

  return return_value;
} //class Application
} //namespace Context
