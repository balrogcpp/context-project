/// created by Andrey Vasiliev

#include "pch.h"
#include "Engine.h"
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

Engine::Engine() {}

Engine::~Engine() { Ogre::Root::getSingleton().removeFrameListener(this); }

void Engine::Init() {
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

void Engine::Capture() {
  static auto &io = InputSequencer::GetInstance();
  io.Capture();
}

void Engine::RegComponent(SystemI *component) {
  component->OnSetUp();
  auto it = find(componentList.begin(), componentList.end(), component);
  if (it == componentList.end()) componentList.push_back(component);
}

void Engine::UnRegComponent(SystemI *component) {
  component->OnClean();
  auto it = find(componentList.begin(), componentList.end(), component);
  if (it != componentList.end()) componentList.erase(it);
}

bool Engine::frameRenderingQueued(const Ogre::FrameEvent &evt) {
  for (auto &it : componentList) it->OnUpdate(evt.timeSinceLastFrame);
  return true;
}

bool Engine::frameEnded(const Ogre::FrameEvent &evt) { return true; }

bool Engine::frameStarted(const Ogre::FrameEvent &evt) { return true; }

void Engine::OnPause() {
  // for (auto &it : componentList) it->OnPause();
}

void Engine::OnResume() {
  // for (auto &it : componentList) it->OnResume();
}

void Engine::OnCleanup() {
  for (auto &it : componentList) it->OnClean();
}

void Engine::OnUpdate(float time) {
  if (sleep) return;
  RenderFrame();
}

void Engine::OnQuit() {}

void Engine::OnFocusLost() { sleep = true; }

void Engine::OnFocusGained() { sleep = false; }

void Engine::RenderFrame() { video->RenderFrame(); }

}  // namespace Glue
