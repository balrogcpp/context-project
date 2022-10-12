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
  engine = make_unique<Engine>();
  engine->Init();
}

Application::~Application() { InputSequencer::GetInstance().UnregWinObserver(this); }

void Application::LoopBody() {
  static int64_t cumulatedTime = 0;
  auto duration_before_frame = chrono::system_clock::now().time_since_epoch();
  static int64_t timeOfLastFrame = chrono::duration_cast<chrono::microseconds>(duration_before_frame).count();
  static bool wasSuspended = false;
  int64_t timeBeforeFrame = chrono::duration_cast<chrono::microseconds>(duration_before_frame).count();

  if (cumulatedTime > int64_t(1e+6)) cumulatedTime = 0;

  engine->Capture();

  if (!suspend) {
    if (wasSuspended) {
      engine->OnResume();
      wasSuspended = false;
    }

    auto duration_before_update = chrono::system_clock::now().time_since_epoch();
    int64_t timeBeforeUpdate = chrono::duration_cast<chrono::microseconds>(duration_before_update).count();
    float frameTime = static_cast<float>(timeBeforeUpdate - timeOfLastFrame) / 1e+6;
    timeOfLastFrame = timeBeforeUpdate;
    engine->Update(frameTime);
    engine->RenderFrame();
  } else {
    engine->OnPause();
    wasSuspended = true;
  }

  auto TimeAftetRender = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();
  auto RenderTime = TimeAftetRender - timeBeforeFrame;

  if (lockFps) {
    auto delay = static_cast<int64_t>((1e+6 / targetFps) - RenderTime);
    if (delay > 0) this_thread::sleep_for(chrono::microseconds(delay));
  }

  int64_t TimeInEndOfLoop = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();
  int64_t TimeSinceLastFrame = TimeInEndOfLoop - timeBeforeFrame;
  cumulatedTime += TimeSinceLastFrame;

#ifdef EMSCRIPTEN
  if (!Running) emscripten_cancel_main_loop();
#endif
}

void Application::Loop() {
  while (running) {
    LoopBody();
  }
}

void Application::EmscriptenLoop(void *arg) {
  auto *app = static_cast<Application *>(arg);
  app->LoopBody();
}

void Application::Go() {
  running = true;
#ifdef MOBILE
  TargetFPS = 30;
#endif
#ifndef EMSCRIPTEN
  Loop();
#else
  LockFPS = false;
  emscripten_set_main_loop_arg(Application::EmscriptenLoop, GetInstancePtr(), 0, 1);
#endif
  engine->OnCleanup();
  engine->OnPause();
}

void Application::OnQuit() { running = false; }

void Application::OnPause() {
  suspend = true;
  engine->OnPause();
}

void Application::OnResume() {
  suspend = false;
  engine->OnResume();
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
