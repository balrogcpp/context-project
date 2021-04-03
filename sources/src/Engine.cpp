//MIT License
//
//Copyright (c) 2021 Andrew Vasiliev
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#include "pcheader.h"
#include "Engine.h"

using namespace std;

namespace xio {

Engine::Engine() {
#if OGRE_PLATFORM!=OGRE_PLATFORM_ANDROID
  conf_ = make_unique<Configurator>("config.json");
#else
  conf_ = make_unique<Configurator>("");
  conf_->AddMember("window_caption", "MyDemo");
  conf_->AddMember("window_width", 1024);
  conf_->AddMember("window_high", 768);
  conf_->AddMember("window_fullscreen", true);
  conf_->AddMember("compositor_use_bloom", false);
  conf_->AddMember("compositor_use_ssao", false);
  conf_->AddMember("compositor_use_motion", false);
  conf_->AddMember("global_target_fps", 30);
  conf_->AddMember("global_lock_fps", true);
  conf_->AddMember("graphics_vsync", false);
  conf_->AddMember("graphics_shadows_enable", false);
  conf_->AddMember("graphics_fsaa", 0);
  conf_->AddMember("graphics_filtration", "bilinear");
  conf_->AddMember("graphics_anisotropy_level", 8);
  conf_->AddMember("graphics_shadows_texture_resolution", 512);
  conf_->AddMember("graphics_shadows_far_distance", 400);
  conf_->AddMember("graphics_shadows_texture_format", 16);
#endif


  Component::SetConfigurator(conf_.get());
  components_.reserve(128);

}

//----------------------------------------------------------------------------------------------------------------------
Engine::~Engine() {

}

//----------------------------------------------------------------------------------------------------------------------
void Engine::InitComponents() {
  int window_width = conf_->Get<int>("window_width");
  int window_high = conf_->Get<int>("window_high");
  bool window_fullscreen = conf_->Get<bool>("window_fullscreen");
  renderer_ = make_unique<Renderer>(window_width, window_high, window_fullscreen);

  // Shadows param
  bool shadow_enable = conf_->Get<bool>("graphics_shadows_enable");
  float shadow_far = conf_->Get<float>("graphics_shadows_far_distance");
  int16_t tex_size = conf_->Get<int>("graphics_shadows_texture_resolution");
  int tex_format = conf_->Get<int>("graphics_shadows_texture_format");

  renderer_->GetShadowSettings().UpdateParams(shadow_enable, shadow_far, tex_size, tex_format);

  input_ = &InputSequencer::GetInstance();

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  physics_ = make_unique<Physics>(true);
  sound_ = make_unique<Sound>(8, 8);
#else
  physics_ = make_unique<Physics>(false);
  sound_ = make_unique<Sound>(4, 4);
#endif
  loader_ = make_unique<DotSceneLoaderB>();


  string graphics_filtration = conf_->Get<string>("graphics_filtration");
  Ogre::TextureFilterOptions tfo = Ogre::TFO_BILINEAR;
  if (graphics_filtration=="anisotropic")
	tfo = Ogre::TFO_ANISOTROPIC;
  else if (graphics_filtration=="bilinear")
	tfo = Ogre::TFO_BILINEAR;
  else if (graphics_filtration=="trilinear")
	tfo = Ogre::TFO_TRILINEAR;
  else if (graphics_filtration=="none")
	tfo = Ogre::TFO_NONE;

  renderer_->UpdateParams(tfo, conf_->Get<int>("graphics_anisotropy_level"));
  renderer_->GetWindow().SetCaption(conf_->Get<string>("window_caption"));
  renderer_->Refresh();

  ComponentLocator::LocateComponents(conf_, input_, renderer_, physics_, sound_, overlay_, loader_);
}

//----------------------------------------------------------------------------------------------------------------------
Engine& Engine::GetInstance() {
	static Engine engine;
	return engine;
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::Capture() {
  input_->Capture();
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::RegComponent(view_ptr<Component> component) {
  components_.push_back(component);
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::Pause() {
  for_each(components_.begin(), components_.end(), [](view_ptr<Component> it) { it->Pause(); });
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::Resume() {
  for_each(components_.begin(), components_.end(), [](view_ptr<Component> it) { it->Resume(); });
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::Cleanup() {
  for_each(components_.begin(), components_.end(), [](view_ptr<Component> it) { it->Cleanup(); });
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::Update(float time) {
  for_each(components_.begin(), components_.end(), [time](view_ptr<Component> it) { it->Update(time); });
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::RenderOneFrame() {
  renderer_->RenderOneFrame();
}

//----------------------------------------------------------------------------------------------------------------------
void Engine::Refresh() {
  renderer_->Refresh();
}

} //namespace
