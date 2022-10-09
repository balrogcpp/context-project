/// created by Andrey Vasiliev

#include "pch.h"
#include "Application.h"
#include "Engine.h"
extern "C" {
#ifdef _MSC_VER
#define SDL_MAIN_HANDLED
#endif
#include <SDL2/SDL.h>
}

using namespace std;

namespace Glue {

Application::Application() {
  InputSequencer::GetInstance().RegWinObserver(this);
  EnginePtr = make_unique<Engine>();
  EnginePtr->Init();
}

Application::~Application() { InputSequencer::GetInstance().UnregWinObserver(this); }

void Application::LoopBody() {
  auto before_frame = chrono::system_clock::now().time_since_epoch();
  int64_t TimeBeforeFrame = chrono::duration_cast<chrono::microseconds>(before_frame).count();

  if (CumultedTime > int64_t(1e+6)) CumultedTime = 0;

  EnginePtr->Capture();

  if (!Suspend) {
    if (WasSuspended) {
      EnginePtr->OnResume();
      WasSuspended = false;
    }

    auto before_update = chrono::system_clock::now().time_since_epoch();
    int64_t TimeBeforeUpdate = chrono::duration_cast<chrono::microseconds>(before_update).count();
    float frame_time = static_cast<float>(TimeBeforeUpdate - TimeOfLastFrame) / 1e+6;
    TimeOfLastFrame = TimeBeforeUpdate;
    EnginePtr->Update(frame_time);
    EnginePtr->RenderFrame();
  } else {
    EnginePtr->OnPause();
    WasSuspended = true;
  }

  auto TimeAftetRender = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();
  auto RenderTime = TimeAftetRender - TimeBeforeFrame;

  if (LockFPS) {
    auto delay = static_cast<int64_t>((1e+6 / TargetFPS) - RenderTime);
    if (delay > 0) this_thread::sleep_for(chrono::microseconds(delay));
  }

  int64_t TimeInEndOfLoop = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();
  int64_t TimeSinceLastFrame = TimeInEndOfLoop - TimeBeforeFrame;
  CumultedTime += TimeSinceLastFrame;

#ifdef EMSCRIPTEN
  if (!Running) emscripten_cancel_main_loop();
#endif
}

void Application::Loop() {
  while (Running) {
    LoopBody();
  }
}

void Application::EmscriptenLoop(void *arg) {
  auto *app = static_cast<Application *>(arg);
  app->LoopBody();
}

void Application::Go() {
  Running = true;
  auto duration_before_update = chrono::system_clock::now().time_since_epoch();
  TimeOfLastFrame = chrono::duration_cast<chrono::microseconds>(duration_before_update).count();
#ifdef MOBILE
  TargetFPS = 30;
#endif
#ifndef EMSCRIPTEN
  Loop();
#else
  LockFPS = false;
  emscripten_set_main_loop_arg(Application::EmscriptenLoop, GetInstancePtr(), 0, 1);
#endif
  EnginePtr->OnCleanup();
  EnginePtr->OnPause();
}

void Application::OnQuit() { Running = false; }

void Application::OnPause() {
  Suspend = true;
  EnginePtr->OnPause();
}

void Application::OnResume() {
  Suspend = false;
  EnginePtr->OnResume();
}

int Application::Main() {
#ifdef _MSC_VER
  SDL_SetMainReady();
#endif
  ios_base::sync_with_stdio(false);
  Go();
  SDL_Quit();
  return 0;
}

}  // namespace Glue
