//MIT License
//
//Copyright (c) 2020 Andrey Vasiliev
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
#include "Renderer.h"
#include "Exception.h"
#include "Storage.h"
#include "RtssUtils.h"

namespace xio {
Renderer::Renderer(int32_t w, int32_t h, bool f) {
  window_ = std::make_unique<Window>(w, h, f);
  root_ = new Ogre::Root("", "", "");
  bool global_sso_enable_ = true;
  bool global_octree_enable_ = true;
  bool global_stbi_enable_ = true;
  bool global_freeimage_enable_ = false;
  bool global_particlefx_enable_ = true;

#ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
  auto *mGlesRenderSystem = new Ogre::GLES2RenderSystem();
  Ogre::Root::getSingleton().setRenderSystem(mGlesRenderSystem);
#else
#ifdef OGRE_BUILD_RENDERSYSTEM_GL3PLUS
  auto *mGL3PlusRenderSystem = new Ogre::GL3PlusRenderSystem();
  if (global_sso_enable_)
    mGL3PlusRenderSystem->setConfigOption("Separate Shader Objects", "Yes");
  else
    mGL3PlusRenderSystem->setConfigOption("Separate Shader Objects", "No");
  Ogre::Root::getSingleton().setRenderSystem(mGL3PlusRenderSystem);
#else
#ifdef OGRE_BUILD_RENDERSYSTEM_GL
  auto *mGLRenderSystem = new Ogre::GLRenderSystem();
  Ogre::Root::getSingleton().setRenderSystem(mGLRenderSystem);
#endif
#endif //OGRE_BUILD_RENDERSYSTEM_GL3PLUS
#endif //OGRE_BUILD_RENDERSYSTEM_GLES2
#ifdef OGRE_BUILD_PLUGIN_OCTREE
  if (global_octree_enable_) {
    auto *mOctreePlugin = new Ogre::OctreeSceneManagerFactory();
    Ogre::Root::getSingleton().addSceneManagerFactory(mOctreePlugin);
  }
#endif // OGRE_BUILD_PLUGIN_OCTREE
#ifdef OGRE_BUILD_PLUGIN_PFX
  if (global_particlefx_enable_) {
    auto *mParticlePlugin = new Ogre::ParticleFXPlugin();
    Ogre::Root::getSingleton().installPlugin(mParticlePlugin);
  }
#endif // OGRE_BUILD_PLUGIN_PFX
#ifdef OGRE_BUILD_PLUGIN_STBI
  if (global_stbi_enable_) {
    auto *mSTBIImageCodec = new Ogre::STBIPlugin();
    Ogre::Root::getSingleton().installPlugin(mSTBIImageCodec);
  }
#endif // OGRE_BUILD_PLUGIN_STBI
#ifdef OGRE_BUILD_PLUGIN_FREEIMAGE
  if (global_freeimage_enable_) {
    auto *mFreeImageCodec = new Ogre::FreeImagePlugin();
    Ogre::Root::getSingleton().installPlugin(mFreeImageCodec);
  }
#endif
  root_->initialise(false);
  Ogre::NameValuePairList params;

  SDL_SysWMinfo info = window_->GetInfo();
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  if (!reinterpret_cast<size_t>(info.info.win.window)) {
    throw Exception("Cast from info.info.win.window to size_t failed");
  }

  params["externalWindowHandle"] = std::to_string(reinterpret_cast<size_t>(info.info.win.window));
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
  if (!reinterpret_cast<unsigned long>(info.info.x11.window)) {
    throw Exception("Cast from info.info.x11.window to size_t failed");
  }

  params["externalWindowHandle"] = std::to_string(reinterpret_cast<unsigned long>(info.info.x11.window));
#endif

  const char true_str[] = "true";
  const char false_str[] = "false";

  bool vsync_ = conf_->Get<bool>("graphics_vsync");
  bool gamma_ = false;
  int fsaa_ = conf_->Get<int>("graphics_fsaa");
  int msaa_ = 0;

  params["vsync"] = vsync_ ? true_str : false_str;
  params["gamma"] = gamma_ ? true_str : false_str;
  params["FSAA"] = std::to_string(fsaa_);
  params["MSAA"] = std::to_string(msaa_);

  ogre_ = root_->createRenderWindow(window_->GetCaption(), window_->GetSize().first, \
                       window_->GetSize().second, false, &params);

#ifdef OGRE_BUILD_PLUGIN_OCTREE
  if (global_octree_enable_) {
    scene_ = root_->createSceneManager("OctreeSceneManager", "Default");
  } else {
    scene_ = root_->createSceneManager("Default", "Default");
  }
#else
  scene_ = root_->createSceneManager("Default", "Default");
#endif

  camera_ = scene_->createCamera("Default");
  auto *renderTarget = root_->getRenderTarget(window_->GetCaption());
  viewport_ = renderTarget->addViewport(camera_);
  viewport_->setBackgroundColour(Ogre::ColourValue::Black);
  camera_->setAspectRatio(static_cast<float>(viewport_->getActualWidth()) / viewport_->getActualHeight());
  camera_->setAutoAspectRatio(true);

#ifndef DEBUG
  Storage::InitGeneralResources({"./programs", "./scenes"}, "resources.list");
#else
  Storage::InitGeneralResources({"../../../programs", "../../../scenes"}, "resources.list");
#endif
  rtss::InitRtss();
  Storage::LoadResources();
  rtss::CreateRtssShaders();
  rtss::InitInstansing();

  shadow_ = std::make_unique<ShadowSettings>();
  compositor_ = std::make_unique<Compositor>();
}
Renderer::~Renderer() {}
//----------------------------------------------------------------------------------------------------------------------
void Renderer::Create() {
  compositor_->EnableEffect("ssao", conf_->Get<bool>("compositor_use_ssao"));
  compositor_->EnableEffect("bloom", conf_->Get<bool>("compositor_use_bloom"));
  compositor_->EnableEffect("hdr", conf_->Get<bool>("compositor_use_hdr"));
  compositor_->EnableEffect("motion", conf_->Get<bool>("compositor_use_motion"));
  compositor_->Init();
  rtss::InitPssm(shadow_->GetSplitPoints());
}
//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreateCamera() {
  if (!scene_->hasCamera("Default")) {
    camera_ = scene_->createCamera("Default");
    auto *renderTarget = root_->getRenderTarget(window_->GetCaption());
    renderTarget->removeViewport(0);
    renderTarget->addViewport(camera_);
  } else {
    camera_ = scene_->getCamera("Default");
  }

  if (camera_) {
    camera_->setNearClipDistance(0.1f);
    camera_->setFarClipDistance(10000.0f);
  }

  camera_->setAutoAspectRatio(true);
  scene_->setSkyBoxEnabled(false);
  scene_->setSkyDomeEnabled(false);
  scene_->setAmbientLight(Ogre::ColourValue::Black);
}
//----------------------------------------------------------------------------------------------------------------------
void Renderer::Refresh() {
  CreateCamera();
}
//----------------------------------------------------------------------------------------------------------------------
void Renderer::UpdateShadow(bool enable, float far_distance, int tex_size, int tex_format){
  shadow_->UpdateParams(enable, far_distance, tex_size, tex_format);
}
//----------------------------------------------------------------------------------------------------------------------
void Renderer::UpdateParams(Ogre::TextureFilterOptions filtering, int anisotropy) {
  Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(filtering);
  if (filtering == Ogre::TFO_ANISOTROPIC)
    Ogre::MaterialManager::MaterialManager::getSingleton().setDefaultAnisotropy(anisotropy);
}
//----------------------------------------------------------------------------------------------------------------------
void Renderer::Resize(int32_t w, int32_t h, bool f) {
  if (f) {
    window_->Fullscreen(f);
    ogre_->resize(window_->GetSize().first, window_->GetSize().second);
    ogre_->setFullscreen(f, window_->GetSize().first, window_->GetSize().second);
  } else {
    window_->Resize(w, h);
    ogre_->resize(w, h);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void Renderer::RenderOneFrame() {
  root_->renderOneFrame();
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  window_->SwapBuffers();
#endif
}
}