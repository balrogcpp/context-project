// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Engine.h"
#include "Config.h"

using namespace std;

namespace glue {

//----------------------------------------------------------------------------------------------------------------------
Engine::Engine() {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  config = make_unique<Config>("config.json");
#else
  config_ = make_unique<Config>("");
  config_->AddMember("window_fullscreen", true);
  config_->AddMember("compositor_use_bloom", false);
  config_->AddMember("compositor_use_ssao", false);
  config_->AddMember("compositor_use_motion", false);
  config_->AddMember("target_fps", 30);
  config_->AddMember("lock_fps", true);
  config_->AddMember("vsync", false);
  config_->AddMember("shadows_enable", false);
  config_->AddMember("fsaa", 0);
  config_->AddMember("filtration", "bilinear");
  config_->AddMember("anisotropy_level", 8);
  config_->AddMember("shadows_texture_resolution", 512);
  config_->AddMember("shadows_far_distance", 400);
  config_->AddMember("shadows_texture_format", 16);
#endif

  System::SetConfig(config.get());
  components.reserve(16);

  io = make_unique<InputHandler>();
}

//----------------------------------------------------------------------------------------------------------------------
Engine::~Engine() {}

//----------------------------------------------------------------------------------------------------------------------
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
  ps_ = make_unique<PhysicsSystem>(false);
  as_ = make_unique<AudioSystem>(4, 4);
#endif

  loader = make_unique<DotSceneLoaderB>();

  rs->Refresh();
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::Capture() {
  static auto &io = InputSequencer::GetInstance();
  io.Capture();
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::RegSystem(view_ptr<System> system) { components.push_back(system); }

//----------------------------------------------------------------------------------------------------------------------
void Engine::Pause() {
  for_each(components.begin(), components.end(), [](view_ptr<System> it) { it->Pause(); });
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::InMenu() {
  ps->Pause();
  loader->Pause();
  io->Pause();
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::OffMenu() {
  ps->Resume();
  loader->Resume();
  io->Resume();
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::Resume() {
  for_each(components.begin(), components.end(), [](view_ptr<System> it) { it->Resume(); });
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::Cleanup() {
  for_each(components.begin(), components.end(), [](view_ptr<System> it) { it->Cleanup(); });
  Refresh();
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::Update(float time) {
  for_each(components.begin(), components.end(), [time](view_ptr<System> it) { it->Update(time); });
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::RenderOneFrame() { rs->RenderOneFrame(); }

//----------------------------------------------------------------------------------------------------------------------
void Engine::Refresh() { rs->Refresh(); }

}  // namespace glue
