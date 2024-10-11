/// created by Andrey Vasiliev

#include "pch.h"
#include "Application.h"

#ifdef _WIN32
extern "C" {
__declspec(dllexport) unsigned long NvOptimusEnablement = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

using namespace std;

namespace gge {

Application::Application() : exiting(false), sleep(false) {
  std::ios_base::sync_with_stdio(false);
  appStateManager = make_unique<AppStateManager>();
}

Application::~Application() { InputSequencer::GetInstance().UnregWindowListener(this); }

void Application::Init() {
  video = make_unique<VideoComponent>();
  video->OnSetUp();
  componentList.push_back(video.get());
  preRenderList.push_back(video.get());

  InputSequencer::GetInstance().RegWindowListener(this);
  Ogre::Root::getSingleton().addFrameListener(this);

  compositor = make_unique<CompositorComponent>();
  compositor->OnSetUp();
  componentList.push_back(compositor.get());
  preRenderList.push_back(compositor.get());

  scene = make_unique<SceneComponent>();
  scene->OnSetUp();
  componentList.push_back(scene.get());
  preRenderList.push_back(scene.get());

  appStateManager->Init();
}

void Application::LoopBody() {
  InputSequencer::GetInstance().Capture();
  if (!sleep) video->RenderFrame();
  FrameControl();

#ifdef EMSCRIPTEN
  if (exiting) {
    emscripten_cancel_main_loop();
  }
#endif
}

void Application::FrameControl() {
  static auto t1 = chrono::steady_clock::now();
  auto t2 = chrono::steady_clock::now();

  if (lockFps || sleep) {
    auto frameTime = t2 - t1;

    if (!sleep) {
      auto remainingTime = chrono::nanoseconds(1000000000 / targetFps) - frameTime;
      this_thread::sleep_for(remainingTime - remainingTime % 1ms);
      while (chrono::steady_clock::now() < t2 + remainingTime) {
      }
    } else {
      auto remainingTime = chrono::nanoseconds(1000000000 / targetFps * 2) - frameTime;
      this_thread::sleep_for(remainingTime);
    }
  }

  t1 = chrono::steady_clock::now();
}

void Application::EnableFpsLock(bool enable) { lockFps = enable; }

void Application::SetFpsFreq(int fps) { targetFps = fps; }

bool Application::IsFpsLockEnabled() { return lockFps; }

int Application::GetFpsFreq() { return targetFps; }

void Application::Loop() {
  while (!exiting) {
    LoopBody();
  }
}

void Application::EmscriptenLoop(void *arg) {
  static auto *app = (Application *)arg;
  app->LoopBody();
}

void Application::Go() {
  InputSequencer::GetInstance().RegWindowListener(this);
#ifndef EMSCRIPTEN
  Loop();
#else
  emscripten_set_main_loop_arg(EmscriptenLoop, this, 0, 1);
#endif
  // engine->OnClean();
}

void Application::OnQuit() { exiting = true; }

void Application::OnFocusLost() { sleep = true; }

void Application::OnFocusGained() { sleep = false; }

bool Application::frameEnded(const Ogre::FrameEvent &evt) {
  for (auto it : postRenderList) {
    if (!it->IsSleeping() && !sleep) {
      it->OnUpdate(evt.timeSinceLastFrame);
    }
  }

  return true;
}

bool Application::frameStarted(const Ogre::FrameEvent &evt) {
  for (auto it : preRenderList) {
    if (!it->IsSleeping() && !sleep) {
      it->OnUpdate(evt.timeSinceLastFrame);
    }
  }

  return true;
}

bool Application::frameRenderingQueued(const Ogre::FrameEvent &evt) {
  for (auto it : queueRenderList) {
    if (!it->IsSleeping() && !sleep) {
      it->OnUpdate(evt.timeSinceLastFrame);
    }
  }

  return true;
}

int Application::Main() {
  Go();
  return 0;
}

}  // namespace gge
