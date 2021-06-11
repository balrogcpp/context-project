// This source file is part of context-project
// Created by Andrew Vasiliev

#include "Engine.h"

#include "Config.h"
#include "pcheader.h"

using namespace std;

namespace xio {

//----------------------------------------------------------------------------------------------------------------------
Engine::Engine() {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  config_ = make_unique<Config>("config.json");
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

  System::SetConfig(config_.get());
  components_.reserve(16);

  io_ = make_unique<InputHandler>();
}

//----------------------------------------------------------------------------------------------------------------------
Engine::~Engine() {}

//----------------------------------------------------------------------------------------------------------------------
void Engine::InitSystems() {
  int window_width = config_->Get<int>("window_width");
  int window_high = config_->Get<int>("window_high");
  bool window_fullscreen = config_->Get<bool>("window_fullscreen");
  rs_ = make_unique<RenderSystem>(window_width, window_high, window_fullscreen);

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  bool physics_threaded = false;  // cause strange behavior sometimes
  config_->Get("physics_threaded", physics_threaded);
  ps_ = make_unique<PhysicsSystem>(physics_threaded);
  as_ = make_unique<AudioSystem>(8, 8);
#else
  ps_ = make_unique<PhysicsSystem>(false);
  as_ = make_unique<AudioSystem>(4, 4);
#endif

  loader_ = make_unique<DotSceneLoaderB>();

  rs_->Refresh();
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::Capture() {
  static auto &io = InputSequencer::GetInstance();
  io.Capture();
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::RegSystem(view_ptr<System> system) { components_.push_back(system); }

//----------------------------------------------------------------------------------------------------------------------
void Engine::Pause() {
  for_each(components_.begin(), components_.end(), [](view_ptr<System> it) { it->Pause(); });
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::InMenu() {
  ps_->Pause();
  loader_->Pause();
  io_->Pause();
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::OffMenu() {
  ps_->Resume();
  loader_->Resume();
  io_->Resume();
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::Resume() {
  for_each(components_.begin(), components_.end(), [](view_ptr<System> it) { it->Resume(); });
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::Cleanup() {
  for_each(components_.begin(), components_.end(), [](view_ptr<System> it) { it->Cleanup(); });
  Refresh();
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::Update(float time) {
  for_each(components_.begin(), components_.end(), [time](view_ptr<System> it) { it->Update(time); });
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::RenderOneFrame() { rs_->RenderOneFrame(); }

//----------------------------------------------------------------------------------------------------------------------
void Engine::Refresh() { rs_->Refresh(); }

}  // namespace xio
