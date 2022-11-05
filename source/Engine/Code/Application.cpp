/// created by Andrey Vasiliev

#include "pch.h"
#include "Application.h"
#include "SystemLocator.h"

using namespace std;

namespace Glue {
Application::Application() : targetFps(60), lockFps(true), exiting(false), sleep(false) {
  std::ios_base::sync_with_stdio(false);
  engine = make_unique<SystemLocator>();
  appStateManager = make_unique<AppStateManager>();
}

Application::~Application() { InputSequencer::GetInstance().UnregWindowListener(this); }

void Application::Init() {
  engine->Init();
  appStateManager->Init();
}

void Application::LoopBody() {
  auto tpBeforeFrame = chrono::steady_clock::now();

  engine->Capture();

  if (!sleep) {
    engine->OnUpdate(0);
  }

  auto tpAfterFrame = chrono::steady_clock::now();
  auto frameDuration = tpAfterFrame - tpBeforeFrame;

  if (lockFps) {
    auto delay = chrono::microseconds{static_cast<int64_t>(1e+6 / targetFps)} - frameDuration;
    this_thread::sleep_for(delay);
  }

#ifdef EMSCRIPTEN
  if (exiting) emscripten_cancel_main_loop();
#endif
}

void Application::Loop() {
  while (!exiting) {
    LoopBody();
  }
}

void Application::EmscriptenLoop(void *arg) {
  static auto *app = static_cast<Application *>(arg);
  app->LoopBody();
}

void Application::Go() {
  InputSequencer::GetInstance().RegWindowListener(this);
#ifdef MOBILE
  targetFps = 30;
#endif
#ifndef EMSCRIPTEN
  Loop();
#else
  lockFps = false;
  emscripten_set_main_loop_arg(EmscriptenLoop, GetInstancePtr(), 0, 1);
#endif
  engine->OnClean();
}

void Application::OnQuit() { exiting = true; }
void Application::OnFocusLost() { sleep = true; }
void Application::OnFocusGained() { sleep = false; }

int Application::Main() {
  Go();
  return 0;
}
}  // namespace Glue
