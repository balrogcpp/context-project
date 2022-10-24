/// created by Andrey Vasiliev

#include "pch.h"
#include "Engine.h"
#include "Observer.h"
#include "SinbadCharacterController.h"
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

Engine::~Engine() {}

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
}

void Engine::Capture() {
  static auto &io = InputSequencer::GetInstance();
  io.Capture();
}

// constexpr ResolutionItem ResolutionList[] = {
//    {"360x800", 360, 800},
//    {"800x600", 800, 600},
//    {"810x1080", 810, 1080},
//    {"768x1024", 768, 1024},
//    {"834x1112", 834, 1112},
//    {"1024x768", 1024, 768},
//    {"1024x1366", 1024, 1366},
//    {"1280x720", 1280, 720},
//    {"1280x800", 1280, 800},
//    {"1280x1024", 1280, 1024},
//    {"1360x768", 1360, 768},
//    {"1366x768", 1366, 768},
//    {"1440x900", 1440, 900},
//    {"1536x864", 1536, 864},
//    {"1600x900", 1600, 900},
//    {"1680x1050", 1680, 1050},
//    {"1920x1080", 1920, 1080},
//    {"1920x1200", 1920, 1200},
//    {"2048x1152", 2048, 1152},
//    {"2560x1440", 2560, 1440},
//};

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

void Engine::OnPause() {
  for (auto &it : componentList) it->OnPause();
}

void Engine::OnResume() {
  for (auto &it : componentList) it->OnResume();
}

void Engine::OnMenuOn() { physics->OnPause(); }

void Engine::OnMenuOff() { physics->OnResume(); }

void Engine::OnCleanup() {
  for (auto &it : componentList) it->OnClean();
}

void Engine::Update(float PassedTime) {
  if (paused) return;
  for (auto &it : componentList) it->OnUpdate(PassedTime);
}

void Engine::RenderFrame() { video->RenderFrame(); }

}  // namespace Glue
