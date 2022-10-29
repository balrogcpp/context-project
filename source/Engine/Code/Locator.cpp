/// created by Andrey Vasiliev

#include "pch.h"
#include "Locator.h"
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

namespace Glue {
Locator::Locator() {}
Locator::~Locator() { Ogre::Root::getSingleton().removeFrameListener(this); }

void Locator::Init() {
  video = make_unique<VideoManager>();
  RegComponent(video.get());

  scene = make_unique<SceneManager>();
  RegComponent(scene.get());

  compositor = make_unique<CompositorManager>();
  RegComponent(compositor.get());

  physics = make_unique<PhysicsManager>();
  RegComponent(physics.get());

  audio = make_unique<AudioManager>();
  RegComponent(audio.get());

  sky = make_unique<SkyManager>();
  RegComponent(sky.get());

  terrain = make_unique<TerrainManager>();
  RegComponent(terrain.get());

  Ogre::Root::getSingleton().addFrameListener(this);
}

void Locator::Capture() {
  static auto &io = InputSequencer::GetInstance();
  io.Capture();
}

void Locator::RegComponent(SystemI *component) {
  auto it = find(componentList.begin(), componentList.end(), component);
  if (it == componentList.end()) {
    component->OnSetUp();
    componentList.push_back(component);
  }
}

void Locator::UnRegComponent(SystemI *component) {
  auto it = find(componentList.begin(), componentList.end(), component);
  if (it != componentList.end()) {
    component->OnClean();
    componentList.erase(it);
  }
}

bool Locator::frameRenderingQueued(const Ogre::FrameEvent &evt) {
  for (auto &it : componentList) it->OnUpdate(evt.timeSinceLastFrame);
  return true;
}

bool Locator::frameEnded(const Ogre::FrameEvent &evt) { return true; }

bool Locator::frameStarted(const Ogre::FrameEvent &evt) { return true; }

void Locator::OnCleanup() {
  for (auto &it : componentList) it->OnClean();
}

void Locator::OnUpdate(float time) {
  if (sleep) return;
  RenderFrame();
}

void Locator::OnQuit() {}

void Locator::OnFocusLost() { sleep = true; }

void Locator::OnFocusGained() { sleep = false; }

void Locator::RenderFrame() { video->RenderFrame(); }
}  // namespace Glue
