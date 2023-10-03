/// created by Andrey Vasiliev

#include "pch.h"
#include "Application.h"
#include "SystemLocator.h"

using namespace std;

namespace gge {
Application::Application() : exiting(false), sleep(false) {
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
  auto t1 = chrono::high_resolution_clock::now();
  OgreProfileBegin("Ogre Main Loop");

  engine->Capture();
  if (!sleep) {
    engine->RenderFrame();
  }

  auto t2 = chrono::high_resolution_clock::now();
  engine->FrameControl(chrono::duration_cast<chrono::microseconds>(t2 - t1));

  OgreProfileEnd("Ogre Main Loop");
#ifdef EMSCRIPTEN
  if (exiting) {
    emscripten_cancel_main_loop();
  }
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
#ifndef EMSCRIPTEN
  Loop();
#else
  emscripten_set_main_loop_arg(EmscriptenLoop, this, 0, 1);
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
}  // namespace gge
