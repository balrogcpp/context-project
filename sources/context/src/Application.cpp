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
#include "StaticForest.h"
#include "Compositors.h"
#include "AppStateManager.h"
#include "Storage.h"
#include "GorillaOverlay.h"
#include "ConfiguratorJson.h"
#include "Exception.h"
#include "CameraMan.h"

#ifdef _WIN32
#include <windows.h>

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
Application::Application() {
  Init_();
};
Application::~Application() = default;

//----------------------------------------------------------------------------------------------------------------------
void Application::Init_() {
#ifndef DEBUG
  Storage::InitGeneralResources({"./programs", "./scenes"}, "resources.list");
#else
  Storage::InitGeneralResources({"../../../programs", "../../../scenes"}, "resources.list");
#endif

  bool physics_enable_ = true;
  bool sound_enable_ = true;
  auto &conf = ConfiguratorJson::GetSingleton();
  conf.Assign(physics_enable_, "physics_enable");
  conf.Assign(sound_enable_, "sound_enable");

  if (!compositor_) compositor_ = std::make_unique<Compositors>();
  GorillaOverlay::GetSingleton().Setup();
  graphics_.UpdateParams();
  loader_.SetWorld(&physics_);
  loader_.SetCamera(graphics_.GetCameraMan());
  graphics_.GetCameraMan()->SetStyle(CameraStyle::FPS);
  conf.Assign(verbose_, "global_verbose_enable");
  conf.Assign(target_fps_, "global_target_fps");
  conf.Assign(lock_fps_, "global_lock_fps");

  io::InputSequencer::GetSingleton().RegEventListener(this);

  if (!verbose_) {
    auto *logger = new Ogre::LogManager();
    std::string log_name = conf.GetString("log_name");
    if (log_name.empty())
      log_name = "Ogre.log";

    logger->createLog(log_name, true, false, true);
    Ogre::LogManager::getSingleton().getDefaultLog()->addListener(this);
    Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_BOREME);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void Application::Render_() {
  graphics_.Render();
}
//----------------------------------------------------------------------------------------------------------------------
void Application::Reset_() {
  auto *root = Ogre::Root::getSingleton().getSceneManager("Default");
  root->destroyAllEntities();
  root->destroyAllLights();
  root->destroyAllParticleSystems();
  root->destroyAllAnimations();
  root->destroyAllAnimationStates();
  root->destroyAllStaticGeometry();
  root->destroyAllManualObjects();
  root->destroyAllInstanceManagers();
  root->destroyAllBillboardChains();
  root->destroyAllBillboardSets();
  root->destroyAllMovableObjects();
  root->destroyAllCameras();
  root->getRootSceneNode()->removeAndDestroyAllChildren();
  Ogre::ResourceGroupManager::getSingleton().unloadResourceGroup(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
}
//----------------------------------------------------------------------------------------------------------------------
void Application::Quit() {
  quit_ = false;
}
//----------------------------------------------------------------------------------------------------------------------
void Application::Event(const SDL_Event &evt) {
  const int fps_inactive = 30;

  if (evt.type == SDL_WINDOWEVENT) {
    auto &conf = ConfiguratorJson::GetSingleton();
    static bool fullscreen = graphics_.GetWindow().GetFullscreen();

    if (!fullscreen) {
      if (evt.window.event == SDL_WINDOWEVENT_LEAVE || evt.window.event == SDL_WINDOWEVENT_MINIMIZED) {
        suspend_ = true;
        target_fps_ = fps_inactive;
        lock_fps_ = true;
      } else if (evt.window.event == SDL_WINDOWEVENT_TAKE_FOCUS || evt.window.event == SDL_WINDOWEVENT_RESTORED
          || evt.window.event == SDL_WINDOWEVENT_MAXIMIZED) {
        suspend_ = false;
        conf.Assign(target_fps_, "global_target_fps");
        conf.Assign(lock_fps_, "global_lock_fps");
      }
    } else {
      if (evt.window.event == SDL_WINDOWEVENT_MINIMIZED) {
        suspend_ = true;
        target_fps_ = fps_inactive;
        lock_fps_ = true;
      } else if (evt.window.event == SDL_WINDOWEVENT_TAKE_FOCUS || evt.window.event == SDL_WINDOWEVENT_RESTORED
          || evt.window.event == SDL_WINDOWEVENT_MAXIMIZED) {
        suspend_ = false;
        conf.Assign(target_fps_, "global_target_fps");
        conf.Assign(lock_fps_, "global_lock_fps");
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
      auto *root = Ogre::Root::getSingleton().getSceneManager("Default");
      auto duration_before_frame = std::chrono::system_clock::now().time_since_epoch();
      long millis_before_frame = std::chrono::duration_cast<std::chrono::microseconds>(duration_before_frame).count();

      int fps_frames_;
      long delta_time_;

      if (delta_time_ > 1000000) {
        current_fps_ = fps_frames_;
        delta_time_ = 0;
        fps_frames_ = 0;
      }

      io::InputSequencer::GetSingleton().Capture();

      if (!suspend_) {
        if (AppStateManager::GetSingleton().IsWaiting()) {
          loader_.Reset();
          physics_.Reset();

          root->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
          root->destroyAllEntities();
          root->destroyAllLights();
          root->destroyAllParticleSystems();
          root->destroyAllAnimations();
          root->destroyAllAnimationStates();
          root->destroyAllStaticGeometry();
          root->destroyAllRibbonTrails();
          root->destroyAllManualObjects();
          root->destroyAllInstanceManagers();
          root->destroyAllBillboardChains();
          root->destroyAllBillboardSets();
          root->destroyAllMovableObjects();
          root->getRootSceneNode()->removeAndDestroyAllChildren();

          graphics_.UpdateParams();
          AppStateManager::GetSingleton().InitCurrState();
          physics_.Start();
          AppStateManager::GetSingleton().ClearWaiting();
        }

        Render_();
      }

#ifdef DEBUG
      if (verbose_)
        std::cout << std::flush;
#endif

      auto duration_after_render = std::chrono::system_clock::now().time_since_epoch();
      long millis_after_render = std::chrono::duration_cast<std::chrono::microseconds>(duration_after_render).count();
      long render_time = millis_after_render - millis_before_frame;

      if (lock_fps_) {
        long delay = static_cast<long> ((1000000 / target_fps_) - render_time);
        if (delay > 0)
          std::this_thread::sleep_for(std::chrono::microseconds(delay));
      }

      auto duration_after_loop = std::chrono::system_clock::now().time_since_epoch();
      long millis_after_loop = std::chrono::duration_cast<std::chrono::microseconds>(duration_after_loop).count();

      long time_since_last_frame_ = millis_after_loop - millis_before_frame;
      delta_time_ += time_since_last_frame_;

      fps_frames_++;
    } else {
      Quit();
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void Application::Go_() {
  if (AppStateManager::GetSingleton().GetCurState()) {
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
int Application::Main(std::shared_ptr<AppState> scene_ptr) {
#ifdef _MSC_VER
  SDL_SetMainReady();
#endif
  int return_value = 0;
  std::ios_base::sync_with_stdio(false);

  try {
    SetCurState(scene_ptr);
    Go_();
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
