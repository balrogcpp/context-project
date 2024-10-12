/// created by Andrey Vasiliev

#include "pch.h"
#include "SystemLocator.h"

using namespace std;

namespace gge {

SystemLocator::SystemLocator() : lockFps(true), targetFps(60) {
#ifdef MOBILE
  lockFps = true;
  targetFps = 30;
#endif
}

SystemLocator::~SystemLocator() { OnClean(); }

void SystemLocator::Init() {
  video = make_unique<VideoManager>();
  RegComponent(video.get(), true);

  InputSequencer::GetInstance().RegWindowListener(this);
  Ogre::Root::getSingleton().addFrameListener(this);

  compositor = make_unique<CompositorManager>();
  RegComponent(compositor.get(), true);

  scene = make_unique<SceneManager>();
  RegComponent(scene.get(), true);
}

void SystemLocator::OnQuit() {}

void SystemLocator::OnFocusLost() { _sleep = true; }

void SystemLocator::OnFocusGained() { _sleep = false; }

void SystemLocator::RenderFrame() {
  if (!_sleep) video->RenderFrame();
  FrameControl();
}

void SystemLocator::Capture() { InputSequencer::GetInstance().Capture(); }

void SystemLocator::RegComponent(SystemI *component, bool preRender) {
  auto it = find(componentList.begin(), componentList.end(), component);
  if (it == componentList.end()) {
    component->OnSetUp();
    componentList.push_back(component);
  }

  if (preRender) {
    auto jt = find(preRenderList.begin(), preRenderList.end(), component);
    if (jt == preRenderList.end()) preRenderList.push_back(component);
  } else {
    auto jt = find(queueRenderList.begin(), queueRenderList.end(), component);
    if (jt == queueRenderList.end()) queueRenderList.push_back(component);
  }
}

void SystemLocator::UnregComponent(SystemI *component) {
  auto it = find(componentList.begin(), componentList.end(), component), end = componentList.end();
  if (it != end) {
    swap(it, --end);
    componentList.pop_back();
    component->OnClean();
  }
}

bool SystemLocator::frameEnded(const Ogre::FrameEvent &evt) {
  for (auto it : postRenderList) {
    if (!it->IsSleeping() && !IsSleeping()) {
      it->OnUpdate(evt.timeSinceLastFrame);
    }
  }

  return true;
}

bool SystemLocator::frameStarted(const Ogre::FrameEvent &evt) {
  for (auto it : preRenderList) {
    if (!it->IsSleeping() && !IsSleeping()) {
      it->OnUpdate(evt.timeSinceLastFrame);
    }
  }

  return true;
}

bool SystemLocator::frameRenderingQueued(const Ogre::FrameEvent &evt) {
  for (auto it : queueRenderList) {
    if (!it->IsSleeping() && !IsSleeping()) {
      it->OnUpdate(evt.timeSinceLastFrame);
    }
  }

  return true;
}

void SystemLocator::OnSetUp() {
  for (auto it : componentList) {
    it->OnSetUp();
  }
}

void SystemLocator::OnClean() {
  for (auto it = componentList.rbegin(); it != componentList.rend(); ++it) {
    (*it)->OnClean();
  }
}

void SystemLocator::SetSleep(bool sleep) {
  for (auto it = componentList.rbegin(); it != componentList.rend(); ++it) {
    (*it)->SetSleep(sleep);
  }
}

void SystemLocator::EnableFpsLock(bool enable) { lockFps = enable; }

void SystemLocator::SetFpsFreq(int fps) { targetFps = fps; }

bool SystemLocator::IsFpsLockEnabled() { return lockFps; }

int SystemLocator::GetFpsFreq() { return targetFps; }

void SystemLocator::FrameControl() {
  static auto t1 = chrono::steady_clock::now();
  auto t2 = chrono::steady_clock::now();

  if (lockFps || _sleep) {
    auto frameTime = t2 - t1;

    if (!_sleep) {
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
}  // namespace gge