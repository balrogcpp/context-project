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

namespace Glue {

Application::Application() {
  try {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
    auto *logger = new Ogre::LogManager();

#ifdef DEBUG
    logger->createLog(LogFileName, true, true, true);
    Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_BOREME);
#else
    logger->createLog(LogFileName, false, false, true);
    Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_NORMAL);
#endif

    Ogre::LogManager::getSingleton().getDefaultLog()->addListener(this);
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
    DesktopIcon icon;
    icon.SaveToFile("GlueSample");
#endif

    EnginePtr = &Engine::GetInstance();
    EnginePtr->InitSystems();

    StateManagerPtr = make_unique<StateManager>();

    GetConf().Get("verbose", Verbose);
    GetConf().Get("verbose_input", VerboseInput);

    if (Verbose) {
      LogBuffer.reserve(10000);
    }

    if (VerboseInput) {
      VerboseListenerPtr = make_unique<VerboseListener>();
    }

    LockFPS = GetConf().Get<bool>("lock_fps");
    TargetFPS = GetConf().Get<int>("target_fps");

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

int Application::ExceptionMessage(const string &WindowCaption, const string &MessageText) {
  //GetWindow().Grab(false);

  SDL_Log("%s", string(WindowCaption + " : " + MessageText).c_str());
  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, WindowCaption.c_str(), MessageText.c_str(), nullptr);

  return 1;
}

void Application::WriteLogToFile() {
  if (!Verbose) {
    return;
  }

  ofstream f;
  f.open(LogFileName);

  if (f.is_open()) {
    f << LogBuffer;
  }
}
void Application::PrintLogToConsole() { cout << LogBuffer << flush; }

void Application::messageLogged(const string &message, Ogre::LogMessageLevel lml, bool maskDebug, const string &logName,
                                bool &skipThisMessage) {
  LogBuffer.append(message);
  LogBuffer.append("\n");

#ifdef DEBUG
  if (Verbose) cout << message << '\n';
#endif
}

void Application::Loop() {
  bool suspend_old = false;

  while (Running && StateManagerPtr->IsActive()) {
    auto before_frame = chrono::system_clock::now().time_since_epoch();
    int64_t TimeBeforeFrame = chrono::duration_cast<chrono::microseconds>(before_frame).count();

    if (CumultedTime > int64_t(1e+6)) {
      CurrentFPS = FPSCounter;
      CumultedTime = 0;
      FPSCounter = 0;
    }

    EnginePtr->Capture();

    if (!Suspend) {
      if (StateManagerPtr->IsDirty()) {
        EnginePtr->Pause();
        EnginePtr->Cleanup();
        StateManagerPtr->InitNextState();
        EnginePtr->Resume();
      } else if (suspend_old) {
        EnginePtr->Resume();
        suspend_old = false;
      }

      auto before_update = chrono::system_clock::now().time_since_epoch();
      int64_t TimeBeforeUpdate = chrono::duration_cast<chrono::microseconds>(before_update).count();
      float frame_time = static_cast<float>(TimeBeforeUpdate - TimeAsLastFrame) / 1e+6;
      TimeAsLastFrame = TimeBeforeUpdate;
      StateManagerPtr->Update(frame_time);
      EnginePtr->Update(frame_time);
      EnginePtr->RenderOneFrame();

    } else {
      EnginePtr->Pause();
      suspend_old = true;
    }

#ifdef DEBUG
    if (Verbose) {
      cout << flush;
    }
#endif

    auto TimeAftetRender = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();
    auto RenderTime = TimeAftetRender - TimeBeforeFrame;

    if (LockFPS) {
      auto delay = static_cast<int64_t>((1e+6 / TargetFPS) - RenderTime);
      if (delay > 0) {
        this_thread::sleep_for(chrono::microseconds(delay));
      }
    }

    int64_t TimeInEndOfLoop = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();

    int64_t TimeSinceLastFrame = TimeInEndOfLoop - TimeBeforeFrame;
    CumultedTime += TimeSinceLastFrame;

    FPSCounter++;
  }

  EnginePtr->Pause();
}

void Application::Go() {
  if (StateManagerPtr->IsActive()) {
    StateManagerPtr->InitCurState();
    Running = true;
    auto duration_before_update = chrono::system_clock::now().time_since_epoch();
    TimeAsLastFrame = chrono::duration_cast<chrono::microseconds>(duration_before_update).count();
    Loop();
  }
}

void Application::OnQuit() { Running = false; }

void Application::OnPause() {
  Suspend = true;
  StateManagerPtr->Pause();
  EnginePtr->Pause();
}

void Application::OnResume() {
  Suspend = false;
  StateManagerPtr->Resume();
  EnginePtr->Resume();
}

void Application::OnEvent(const SDL_Event &Event) {}

int Application::Main(unique_ptr<AppState> &&AppStatePtr) {
  try {
#if OGRE_COMPILER == OGRE_COMPILER_MSVC
    SDL_SetMainReady();
#endif

    ios_base::sync_with_stdio(false);

    StateManagerPtr->SetInitialState(move(AppStatePtr));
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

}  // namespace Glue
