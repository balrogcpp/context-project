// This source file is part of Glue Engine. Created by Andrey Vasiliev

#include "PCHeader.h"
#include "Application.h"
#include "Config.h"
#include "DesktopIcon.h"
#include "Engine.h"
#include "Exception.h"
#include "RTSS.h"
#include "SDL2.hpp"
#include "System.h"
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

namespace Glue {

Application::Application(int argc, char *args[]) {
  InputSequencer::GetInstance().RegWinObserver(this);

#ifdef DESKTOP
  string BinaryDir;

  if (args)
    BinaryDir = GetCurrentDirectoryB(args[0]);
  else
    BinaryDir = GetCurrentDirectoryB("");

  if (BinaryDir.empty()) {
    BinaryDir = GetUserDirectory();
    if (!BinaryDir.empty()) BinaryDir = PathAppend(BinaryDir, ".Glue");
    if (!DirectoryExists(BinaryDir)) CreateDirectory(BinaryDir);
  }

  LogPtr = make_unique<Log>(PathAppend(BinaryDir, "Runtime.log"));
  ConfigPtr = make_unique<Config>(PathAppend(BinaryDir, "Config.ini"));

  bool Verbose = ConfigPtr->GetBool("verbose", false);
  LogPtr->WriteLogToConsole(Verbose);
  LogPtr->WriteLogToFile(Verbose);
#else
  ConfigPtr = make_unique<Config>("");
#endif

  EnginePtr = make_unique<Engine>();
  EnginePtr->InitComponents();

  StateManagerPtr = make_unique<AppStateManager>();

  Verbose = ConfigPtr->GetBool("verbose", Verbose);
  VerboseInput = ConfigPtr->GetBool("verbose_input", VerboseInput);

#ifdef DESKTOP
  if (VerboseInput) VerboseListenerPtr = make_unique<VerboseListener>();
#endif

  LockFPS = ConfigPtr->GetBool("lock_fps", LockFPS);
  TargetFPS = ConfigPtr->GetInt("target_fps", TargetFPS);
}

Application::~Application() { InputSequencer::GetInstance().UnregWinObserver(this); }

void Application::Loop() {
  bool WasSuspend = false;

  if (Running && StateManagerPtr->IsActive()) {
    StateManagerPtr->Update(0);
    EnginePtr->RenderOneFrame();
    CleanRTSSRuntime();
  }

  while (Running && StateManagerPtr->IsActive()) {
    auto before_frame = chrono::system_clock::now().time_since_epoch();
    int64_t TimeBeforeFrame = chrono::duration_cast<chrono::microseconds>(before_frame).count();

    if (CumultedTime > int64_t(1e+6)) {
      CumultedTime = 0;
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
      float frame_time = static_cast<float>(TimeBeforeUpdate - TimeOfLastFrame) / 1e+6;
      TimeOfLastFrame = TimeBeforeUpdate;
      StateManagerPtr->Update(frame_time);
      EnginePtr->Update(frame_time);
      EnginePtr->RenderOneFrame();

    } else {
      EnginePtr->Pause();
      WasSuspend = true;
    }

#ifdef DEBUG
    if (Verbose) cout << flush;
#endif

    auto TimeAftetRender = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();
    auto RenderTime = TimeAftetRender - TimeBeforeFrame;

    if (LockFPS) {
      auto delay = static_cast<int64_t>((1e+6 / TargetFPS) - RenderTime);
      if (delay > 0) this_thread::sleep_for(chrono::microseconds(delay));
    }

    int64_t TimeInEndOfLoop = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();

    int64_t TimeSinceLastFrame = TimeInEndOfLoop - TimeBeforeFrame;
    CumultedTime += TimeSinceLastFrame;
  }

  EnginePtr->Cleanup();
  EnginePtr->Pause();
}

void Application::Go() {
  if (StateManagerPtr->IsActive()) {
    StateManagerPtr->InitCurState();
    Running = true;
    auto duration_before_update = chrono::system_clock::now().time_since_epoch();
    TimeOfLastFrame = chrono::duration_cast<chrono::microseconds>(duration_before_update).count();
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
#if OGRE_COMPILER == OGRE_COMPILER_MSVC
  SDL_SetMainReady();
#endif

  ios_base::sync_with_stdio(false);

  if (StateManagerPtr && AppStatePtr) {
    StateManagerPtr->SetInitialState(move(AppStatePtr));
    Go();
  }

  SDL_Quit();

  return 0;
}

}  // namespace Glue
