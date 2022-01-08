// This source file is part of "glue project". Created by Andrew Vasiliev

#include "PCHeader.h"
#include "Application.h"
#include "Config.h"
#include "DesktopIcon.h"
#include "Engine.h"
#include "Exception.h"
#include "RTSS.h"
#include "SDL2.hpp"
#include <iostream>

#ifdef _WIN32
extern "C" {
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

using namespace std;

#ifdef DESKTOP
static inline std::string GetCurrentDirectory(const string &args = "./") {
#ifdef UNIX
  // Get the last position of '/'
  std::string aux(args);

  // get '/' or '\\' depending on unix/mac or windows.
#if defined(_WIN32) || defined(WIN32)
  int pos = aux.rfind('\\');
#else
  int pos = aux.rfind('/');
#endif

  // Get the path and the name
  std::string path = aux.substr(0, pos + 1);
  std::string name = aux.substr(pos + 1);

  return path.append("/");
#elif defined(WINDOWS)
  char buffer[MAX_PATH];
  GetModuleFileNameA(NULL, buffer, MAX_PATH);
  std::string::size_type pos = std::string(buffer).find_last_of("\\/");

  std::string path = std::string(buffer).substr(0, pos);

  return path.append("\\");
#endif

  return std::string();
}
#endif

namespace Glue {

Application::Application(int argc, char *args[]) {
  try {
    InputSequencer::GetInstance().RegWinObserver(this);

#ifdef DESKTOP
    string BinaryDir;
    if (args) {
      BinaryDir = GetCurrentDirectory(args[0]);
    }
    else {
      BinaryDir = GetCurrentDirectory();
    }

    ConfigPtr = make_unique<Config>(BinaryDir + "Config.ini");
#else
    ConfigPtr = make_unique<Config>("");
#endif

    EnginePtr = make_unique<Engine>();
    EnginePtr->InitComponents();

    StateManagerPtr = make_unique<AppStateManager>();

    Verbose = ConfigPtr->GetBool("verbose", Verbose);
    VerboseInput = ConfigPtr->GetBool("verbose_input", VerboseInput);

#ifdef DESKTOP
    if (VerboseInput) {
      VerboseListenerPtr = make_unique<VerboseListener>();
    }
#endif

    LockFPS = ConfigPtr->GetBool("lock_fps", LockFPS);
    TargetFPS = ConfigPtr->GetInt("target_fps", TargetFPS);

  } catch (Exception &e) {
    ErrorWindow("Exception", e.GetDescription());
  } catch (Ogre::Exception &e) {
    ErrorWindow("Exception", string("Ogre: ") + e.getFullDescription());
  } catch (exception &e) {
    ErrorWindow("Exception", string("std::exception: ") + e.what());
  } catch (...) {
    ErrorWindow("Exception", "unhandled");
  }
}

Application::~Application() { InputSequencer::GetInstance().UnregWinObserver(this); }

void Application::Loop() {
  bool WasSuspend = false;

  if (Running && StateManagerPtr->IsActive()) {
    StateManagerPtr->Update(0);
    EnginePtr->RenderFirstFrame();
  }

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
        EnginePtr->Cleanup();
        StateManagerPtr->InitNextState();
      } else if (WasSuspend) {
        EnginePtr->Resume();
        WasSuspend = false;
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
      WasSuspend = true;
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

  EnginePtr->Cleanup();
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

int Application::Main(unique_ptr<AppState> &&AppStatePtr) {
  try {
#if OGRE_COMPILER == OGRE_COMPILER_MSVC
    SDL_SetMainReady();
#endif

    ios_base::sync_with_stdio(false);

    if (StateManagerPtr && AppStatePtr) {
      StateManagerPtr->SetInitialState(move(AppStatePtr));
      Go();
    }

    SDL_Quit();

  } catch (Exception &e) {
    ErrorWindow("Exception", e.GetDescription());
  } catch (Ogre::Exception &e) {
    ErrorWindow("Exception", string("Ogre: ") + e.getFullDescription());
  } catch (exception &e) {
    ErrorWindow("Exception", string("std::exception: ") + e.what());
  } catch (...) {
    ErrorWindow("Exception", "unhandled");
  }

  return 0;
}

}  // namespace Glue
