// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Application.h"
#include "Engine.h"
#include "Conf.h"
#include "DesktopIcon.h"
#include "Exception.h"
#include "RTSS.h"
#include <iostream>

#ifdef _WIN32
extern "C" {
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

using namespace std;

namespace Glue {

Application::Application() {
  try {
    EnginePtr = &Engine::GetInstance();
    EnginePtr->InitComponents();

    StateManagerPtr = make_unique<AppStateManager>();
    auto &ConfPtr = *Conf::GetInstancePtr();

    Verbose = ConfPtr.GetBool("verbose", Verbose);
    VerboseInput = ConfPtr.GetBool("verbose_input", VerboseInput);

    if (VerboseInput) {
      VerboseListenerPtr = make_unique<VerboseListener>();
    }

    LockFPS = ConfPtr.GetBool("lock_fps", LockFPS);
    TargetFPS = ConfPtr.GetInt("target_fps", TargetFPS);

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

Application::~Application() {}

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

    StateManagerPtr->SetInitialState(move(AppStatePtr));
    Go();

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
