/// created by Andrey Vasiliev

#include "pch.h"
#include "Application.h"
#include "Engine.h"

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

  int64_t timeAftetRender = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();
  int64_t renderDuration = timeAftetRender - timeBeforeFrame;

  if (lockFps) {
    int64_t delay = static_cast<int64_t>((1e+6 / targetFps) - renderDuration);
    if (delay > 0) this_thread::sleep_for(chrono::microseconds(delay));
  }

  int64_t TimeInEndOfLoop = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();
  int64_t TimeSinceLastFrame = TimeInEndOfLoop - timeBeforeFrame;
  cumulatedTime += TimeSinceLastFrame;

#ifdef EMSCRIPTEN
  if (quit) emscripten_cancel_main_loop();
#endif
}

void Application::Loop() {
  while (!quit) {
    LoopBody();
  }
}

void Application::EmscriptenLoop(void *arg) {
  static auto *app = static_cast<Application *>(arg);
  app->LoopBody();
}

void Application::Go() {
#ifdef MOBILE
  targetFps = 30;
#endif
#ifndef EMSCRIPTEN
  Loop();
#else
  lockFps = false;
  emscripten_set_main_loop_arg(EmscriptenLoop, GetInstancePtr(), 0, 1);
#endif
  engine->OnCleanup();
}

void Application::OnQuit() { quit = true; }

void Application::OnFocusLost() {
  suspend = true;
  engine->OnPause();
}

void Application::OnFocusGained() {
  suspend = false;
  engine->OnResume();
}

int Application::Main() {
#ifndef DEBUG
  ios_base::sync_with_stdio(false);
#endif
  Go();
  return 0;
}

}  // namespace Glue
