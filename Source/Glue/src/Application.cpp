// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Application.h"
#include "ComponentLocator.h"
#include "DesktopIcon.h"
#include "Exception.h"
#include <fstream>
#include <iostream>

#ifdef WIN32
extern "C" {
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

using namespace std;

namespace glue {

Application::Application() {
  try {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
    auto *logger = new Ogre::LogManager();

#ifdef DEBUG
    logger->createLog(log_file, true, true, true);
    Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_BOREME);
#else
    logger->createLog(log_file, false, false, true);
    Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_NORMAL);
#endif

    Ogre::LogManager::getSingleton().getDefaultLog()->addListener(this);

#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
    DesktopIcon icon;
    icon.SetUp();
    icon.Save("GlueSample");
#endif

    engine = &Engine::GetInstance();
    engine->InitSystems();

    state_manager_ = make_unique<StateManager>();

    GetConf().Get("verbose", verbose);
    GetConf().Get("verbose_input", verbose_input);

    if (verbose) {
      log.reserve(10000);
    }

    if (verbose_input) {
      verbose_listener_ = make_unique<VerboseListener>();
    }

    lock_fps = GetConf().Get<bool>("lock_fps");
    target_fps_ = GetConf().Get<int>("target_fps");

  } catch (Exception &e) {
    ExceptionMessage("Exception", e.GetDescription());
  } catch (Ogre::Exception &e) {
    ExceptionMessage("Exception", string("Ogre: ") + e.getFullDescription());
  } catch (exception &e) {
    ExceptionMessage("Exception", string("std::exception: ") + e.what());
  } catch (...) {
    ExceptionMessage("Exception", "unhandled");
  }
}

Application::~Application() {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  Ogre::LogManager::getSingleton().getDefaultLog()->removeListener(this);
#endif
}

int Application::ExceptionMessage(const string &caption, const string &message) {
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

void Application::WriteLogToFile() {
  if (!verbose) {
    return;
  }

  ofstream f;
  f.open(log_file);

  if (f.is_open()) {
    f << log;
  }
}
void Application::PrintLogToConsole() { cout << log << flush; }

void Application::messageLogged(const string &message, Ogre::LogMessageLevel lml, bool maskDebug, const string &logName,
                                bool &skipThisMessage) {
  log.append(message);
  log.append("\n");

#ifdef DEBUG
  if (verbose) cout << message << '\n';
#endif
}

void Application::Loop() {
  bool suspend_old = false;

  while (running && state_manager_->IsActive()) {
    auto before_frame = chrono::system_clock::now().time_since_epoch();
    int64_t micros_before_frame = chrono::duration_cast<chrono::microseconds>(before_frame).count();

    if (cumulated_time > int64_t(1e+6)) {
      current_fps = fps_counter;
      cumulated_time = 0;
      fps_counter = 0;
    }

    engine->Capture();

    if (!suspend) {
      if (state_manager_->IsDirty()) {
        engine->Pause();
        engine->Cleanup();
        state_manager_->InitNextState();
        engine->Resume();
      } else if (suspend_old) {
        engine->Resume();
        suspend_old = false;
      }

      auto before_update = chrono::system_clock::now().time_since_epoch();
      int64_t micros_before_update = chrono::duration_cast<chrono::microseconds>(before_update).count();
      float frame_time = static_cast<float>(micros_before_update - time_of_last_frame) / 1e+6;
      time_of_last_frame = micros_before_update;
      state_manager_->Update(frame_time);
      engine->Update(frame_time);
      engine->RenderOneFrame();

    } else {
      engine->Pause();
      suspend_old = true;
    }

#ifdef DEBUG
    if (verbose) {
      cout << flush;
    }
#endif

    auto duration_after_render = chrono::system_clock::now().time_since_epoch();
    auto micros_after_render = chrono::duration_cast<chrono::microseconds>(duration_after_render).count();
    auto render_time = micros_after_render - micros_before_frame;

    if (lock_fps) {
      auto delay = static_cast<int64_t>((1e+6 / target_fps_) - render_time);
      if (delay > 0) {
        this_thread::sleep_for(chrono::microseconds(delay));
      }
    }

    auto duration_after_loop = chrono::system_clock::now().time_since_epoch();
    int64_t micros_after_loop = chrono::duration_cast<chrono::microseconds>(duration_after_loop).count();

    int64_t time_since_last_frame_ = micros_after_loop - micros_before_frame;
    cumulated_time += time_since_last_frame_;

    fps_counter++;
  }

  engine->Pause();
}

void Application::Go() {
  if (state_manager_->IsActive()) {
    state_manager_->InitCurState();
    running = true;
    auto duration_before_update = chrono::system_clock::now().time_since_epoch();
    time_of_last_frame = chrono::duration_cast<chrono::microseconds>(duration_before_update).count();
    Loop();
  }
}

void Application::Quit() { running = false; }

void Application::Pause() {
  suspend = true;
  state_manager_->Pause();
  engine->Pause();
}

void Application::Resume() {
  suspend = false;
  state_manager_->Resume();
  engine->Resume();
}

void Application::Event(const SDL_Event &evt) {}

int Application::Main(unique_ptr<AppState> &&scene_ptr) {
  try {
#if OGRE_COMPILER == OGRE_COMPILER_MSVC
    SDL_SetMainReady();
#endif

    ios_base::sync_with_stdio(false);

    state_manager_->SetInitialState(move(scene_ptr));
    Go();

    WriteLogToFile();

    SDL_Quit();

  } catch (Exception &e) {
    ExceptionMessage("Exception", e.GetDescription());
  } catch (Ogre::Exception &e) {
    ExceptionMessage("Exception", string("Ogre: ") + e.getFullDescription());
  } catch (exception &e) {
    ExceptionMessage("Exception", string("std::exception: ") + e.what());
  } catch (...) {
    ExceptionMessage("Exception", "unhandled");
  }

  return 0;
}

}  // namespace glue
