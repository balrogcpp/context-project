/// created by Andrey Vasiliev

#include "pch.h"
#include "SystemLocator.h"
#include "SDLListener.h"
#include "SinbadCharacterController.h"
#include <Ogre.h>
#include <SDL2/SDL_messagebox.h>
#ifdef _WIN32
extern "C" {
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

using namespace std;

namespace gge {
SystemLocator::SystemLocator() : sleep(false), lockFps(false), targetFps(60) {
  componentList.reserve(50);
#ifdef MOBILE
  lockFps = true;
  targetFps = 30;
#endif
}
SystemLocator::~SystemLocator() {
  OnClean();
  Ogre::Root::getSingleton().removeFrameListener(this);
}

void SystemLocator::Init() {
  video = make_unique<VideoManager>();
  RegComponent(video.get());

  compositor = make_unique<CompositorManager>();
  RegComponent(compositor.get());

  physics = make_unique<PhysicsManager>();
  RegComponent(physics.get());

#ifndef __EMSCRIPTEN__
  audio = make_unique<AudioManager>();
  RegComponent(audio.get());
#endif

  scene = make_unique<SceneManager>();
  RegComponent(scene.get());

  terrain = make_unique<TerrainManager>();
  RegComponent(terrain.get());

  sky = make_unique<SkyManager>();
  RegComponent(sky.get());

  forests = make_unique<ForestsManager>();
  RegComponent(forests.get());

  Ogre::Root::getSingleton().addFrameListener(this);
}

void SystemLocator::OnQuit() {}
void SystemLocator::OnFocusLost() { sleep = true; }
void SystemLocator::OnFocusGained() { sleep = false; }
void SystemLocator::RenderFrame() { video->RenderFrame(); }

void SystemLocator::Capture() {
  static auto &io = InputSequencer::GetInstance();
  io.Capture();
}

void SystemLocator::RegComponent(SystemI *component) {
  auto it = find(componentList.begin(), componentList.end(), component);
  if (it == componentList.end()) {
    component->OnSetUp();
    componentList.push_back(component);
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

bool SystemLocator::frameEnded(const Ogre::FrameEvent &evt) { return true; }
bool SystemLocator::frameStarted(const Ogre::FrameEvent &evt) { return true; }
bool SystemLocator::frameRenderingQueued(const Ogre::FrameEvent &evt) {
  for (auto it : componentList) {
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
  for (vector<SystemI *>::reverse_iterator it = componentList.rbegin(); it != componentList.rend(); ++it) {
    (*it)->OnClean();
  }
}

void SystemLocator::EnableFpsLock(bool enable) { lockFps = enable; }
void SystemLocator::SetFpsFreq(int fps) { targetFps = fps; }
bool SystemLocator::IsFpsLockEnabled() { return lockFps; }
int SystemLocator::GetFpsFreq() { return targetFps; }

void SystemLocator::FrameControl(chrono::microseconds frameDuration) {
  if (lockFps) {
    auto delay = chrono::microseconds(static_cast<long int>(1e+6 / targetFps)) - frameDuration;
    this_thread::sleep_for(delay);
  }
}
}  // namespace gge
