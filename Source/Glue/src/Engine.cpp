// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Engine.h"
#include "Config.h"

using namespace std;

namespace Glue {

Engine::Engine() {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  config = make_unique<Config>("config.json");
#else
  config = make_unique<Config>("");
  config->AddMember("window_fullscreen", true);
  config->AddMember("compositor_use_bloom", false);
  config->AddMember("compositor_use_ssao", false);
  config->AddMember("compositor_use_motion", false);
  config->AddMember("target_fps", 30);
  config->AddMember("lock_fps", true);
  config->AddMember("vsync", false);
  config->AddMember("shadows_enable", false);
  config->AddMember("fsaa", 0);
  config->AddMember("filtration", "bilinear");
  config->AddMember("anisotropy_level", 8);
  config->AddMember("shadows_texture_resolution", 512);
  config->AddMember("shadows_far_distance", 400);
  config->AddMember("shadows_texture_format", 16);
#endif

  Component::SetConfig(config.get());
  components.reserve(16);

  io = make_unique<InputHandler>();
}

Engine::~Engine() {}

void Engine::InitSystems() {
  int window_width = config->Get<int>("window_width");
  int window_high = config->Get<int>("window_high");
  bool window_fullscreen = config->Get<bool>("window_fullscreen");
  rs = make_unique<RenderSystem>(window_width, window_high, window_fullscreen);

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  bool physics_threaded = false;  // cause strange behavior sometimes
  config->Get("physics_threaded", physics_threaded);
  ps = make_unique<PhysicsSystem>(physics_threaded);
  as = make_unique<AudioSystem>(8, 8);
#else
  ps = make_unique<PhysicsSystem>(false);
  as = make_unique<AudioSystem>(4, 4);
#endif

  loader = make_unique<DotSceneLoaderB>();

  rs->Refresh();
}

void Engine::Capture() {
  static auto &io = InputSequencer::GetInstance();
  io.Capture();
}

void Engine::RegSystem(ViewPtr<Component> system) { components.push_back(system); }

void Engine::Pause() {
  for_each(components.begin(), components.end(), [](ViewPtr<Component> it) { it->Pause(); });
}

void Engine::InMenu() {
  ps->Pause();
  loader->Pause();
  io->Pause();
}

void Engine::OffMenu() {
  ps->Resume();
  loader->Resume();
  io->Resume();
}

void Engine::Resume() {
  for_each(components.begin(), components.end(), [](ViewPtr<Component> it) { it->Resume(); });
}

void Engine::Cleanup() {
  for_each(components.begin(), components.end(), [](ViewPtr<Component> it) { it->Cleanup(); });
  Refresh();
}

void Engine::Update(float time) {
  for_each(components.begin(), components.end(), [time](ViewPtr<Component> it) { it->Update(time); });
}

void Engine::RenderOneFrame() { rs->RenderOneFrame(); }

void Engine::Refresh() { rs->Refresh(); }

}  // namespace glue
