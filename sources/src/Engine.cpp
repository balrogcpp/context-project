// MIT License
//
// Copyright (c) 2021 Andrew Vasiliev
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "Engine.h"

#include "Config.h"
#include "pcheader.h"

using namespace std;

namespace xio {

//----------------------------------------------------------------------------------------------------------------------
Engine::Engine() {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  conf_ = make_unique<Config>("config.json");
#else
  conf_ = make_unique<Config>("");
  conf_->AddMember("window_fullscreen", true);
  conf_->AddMember("compositor_use_bloom", false);
  conf_->AddMember("compositor_use_ssao", false);
  conf_->AddMember("compositor_use_motion", false);
  conf_->AddMember("target_fps", 30);
  conf_->AddMember("lock_fps", true);
  conf_->AddMember("vsync", false);
  conf_->AddMember("shadows_enable", false);
  conf_->AddMember("fsaa", 0);
  conf_->AddMember("filtration", "bilinear");
  conf_->AddMember("anisotropy_level", 8);
  conf_->AddMember("shadows_texture_resolution", 512);
  conf_->AddMember("shadows_far_distance", 400);
  conf_->AddMember("shadows_texture_format", 16);
#endif

  System::SetConfig(conf_.get());
  components_.reserve(16);

  io_ = make_unique<InputHandler>();
}

//----------------------------------------------------------------------------------------------------------------------
Engine::~Engine() {}

//----------------------------------------------------------------------------------------------------------------------
void Engine::InitComponents() {
  int window_width = conf_->Get<int>("window_width");
  int window_high = conf_->Get<int>("window_high");
  bool window_fullscreen = conf_->Get<bool>("window_fullscreen");
  renderer_ =
      make_unique<RenderSystem>(window_width, window_high, window_fullscreen);

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  bool physics_threaded = false;  // cause strange behavior sometimes
  conf_->Get("physics_threaded", physics_threaded);
  physics_ = make_unique<PhysicsSystem>(physics_threaded);
  audio_ = make_unique<AudioSystem>(8, 8);
#else
  physics_ = make_unique<PhysicsSystem>(false);
  audio_ = make_unique<AudioSystem>(4, 4);
#endif

  loader_ = make_unique<DotSceneLoaderB>();

  renderer_->Refresh();
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::Capture() {
  static auto &io = InputSequencer::GetInstance();
  io.Capture();
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::RegComponent(view_ptr<System> component) {
  components_.push_back(component);
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::Pause() {
  for_each(components_.begin(), components_.end(),
           [](view_ptr<System> it) { it->Pause(); });
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::InMenu() {
  physics_->Pause();
  loader_->Pause();
  io_->Pause();
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::OffMenu() {
  physics_->Resume();
  loader_->Resume();
  io_->Resume();
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::Resume() {
  for_each(components_.begin(), components_.end(),
           [](view_ptr<System> it) { it->Resume(); });
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::Cleanup() {
  for_each(components_.begin(), components_.end(),
           [](view_ptr<System> it) { it->Cleanup(); });
  Refresh();
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::Update(float time) {
  for_each(components_.begin(), components_.end(),
           [time](view_ptr<System> it) { it->Update(time); });
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::RenderOneFrame() { renderer_->RenderOneFrame(); }

//----------------------------------------------------------------------------------------------------------------------
void Engine::Refresh() { renderer_->Refresh(); }

}  // namespace xio
