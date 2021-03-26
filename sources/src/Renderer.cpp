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
#include "Renderer.h"
#include "Assets.h"
#include "RtssUtils.h"
#include "Exception.h"


using namespace std;

namespace xio {
Renderer::Renderer(int w, int h, bool f) {
  window_ = make_unique<Window>(w, h, f);

  root_ = new Ogre::Root("", "", "");

#ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
  Ogre::Root::getSingleton().setRenderSystem(new Ogre::GLES2RenderSystem());
#elif defined OGRE_BUILD_RENDERSYSTEM_GL3PLUS
  auto *gl3plus_render_system = new Ogre::GL3PlusRenderSystem();
  gl3plus_render_system->setConfigOption("Separate Shader Objects", "Yes");

  Ogre::Root::getSingleton().setRenderSystem(gl3plus_render_system);
#endif
#ifdef OGRE_BUILD_PLUGIN_OCTREE
  Ogre::Root::getSingleton().addSceneManagerFactory(new Ogre::OctreeSceneManagerFactory());
#endif // OGRE_BUILD_PLUGIN_OCTREE
#ifdef OGRE_BUILD_PLUGIN_PFX
  Ogre::Root::getSingleton().installPlugin(new Ogre::ParticleFXPlugin());
#endif // OGRE_BUILD_PLUGIN_PFX
#ifdef OGRE_BUILD_PLUGIN_STBI
  Ogre::Root::getSingleton().installPlugin(new Ogre::STBIPlugin());
#endif // OGRE_BUILD_PLUGIN_STBI
#ifdef OGRE_BUILD_PLUGIN_FREEIMAGE
  Ogre::Root::getSingleton().installPlugin(new Ogre::FreeImagePlugin());
#endif
#ifdef OGRE_BUILD_PLUGIN_ASSIMP
  Ogre::Root::getSingleton().installPlugin(new Ogre::AssimpPlugin());
#endif

  root_->initialise(false);
  Ogre::NameValuePairList params;


#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  SDL_SysWMinfo info = window_->GetInfo();

  if (!reinterpret_cast<size_t>(info.info.win.window))
    throw Exception("Cast from info.info.win.window to size_t failed");

  params["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(info.info.win.window));
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
  SDL_SysWMinfo info = window_->GetInfo();

  if (!reinterpret_cast<size_t>(info.info.x11.window))
    throw Exception("Cast from info.info.x11.window to size_t failed");

  params["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(info.info.x11.window));
#elif OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  params["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(native_));
  params["androidConfig"] = to_string(reinterpret_cast<size_t>(mAConfig));
  params["preserveContext"] = "true"; //Optionally preserve the gl context, prevents reloading all resources, this is false by default
#endif


  const char true_str[] = "true";
  const char false_str[] = "false";
  bool vsync_ = conf_->Get<bool>("graphics_vsync");
  bool gamma_ = conf_->Get<bool>("graphics_gamma");;
  int fsaa_ = conf_->Get<int>("graphics_fsaa");

  params["vsync"] = vsync_ ? true_str : false_str;
  params["gamma"] = gamma_ ? true_str : false_str;
  params["FSAA"] = to_string(fsaa_);

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  render_window_ = root_->createRenderWindow(window_->GetCaption(), window_->GetSize().first, \
                       window_->GetSize().second, window_->IsFullscreen(), &params);
#else
  render_window_ = Ogre::Root::getSingleton().createRenderWindow("MyDemo", 0, 0, false, &params);
#endif

#ifdef OGRE_BUILD_PLUGIN_OCTREE
  scene_ = root_->createSceneManager("OctreeSceneManager", "Default");
#else
  ogre_scene_ = root_->createSceneManager(Ogre::ST_GENERIC, "Default");
#endif

  //Camera block
  camera_ = scene_->createCamera("Default");
  auto *renderTarget = root_->getRenderTarget(render_window_->getName());
  viewport_ = renderTarget->addViewport(camera_.get());
  viewport_->setBackgroundColour(Ogre::ColourValue::Black);
  camera_->setAspectRatio(static_cast<float>(viewport_->getActualWidth()) / static_cast<float>(viewport_->getActualHeight()));
  camera_->setAutoAspectRatio(true);

  //Resource block
  Assets::InitGeneralResources("programs", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, "resources.list");
  Assets::InitGeneralResources("assets", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, "resources.list");

  //RTSS block
  xio::InitRtss();
  Assets::LoadResources();
  xio::CreateRtssShaders();
//  xio::InitInstansing();

  //Shadow block
  shadow_ = make_unique<ShadowSettings>();
  //Compositor block
  compositor_ = make_unique<Compositor>();

  compositor_->EnableEffect("ssao", conf_->Get<bool>("compositor_use_ssao"));
  compositor_->EnableEffect("bloom", conf_->Get<bool>("compositor_use_bloom"));
  compositor_->EnableEffect("hdr", conf_->Get<bool>("compositor_use_hdr"));
  compositor_->EnableEffect("motion", conf_->Get<bool>("compositor_use_motion"));
  compositor_->Init();
//  xio::InitPssm(shadow_->GetSplitPoints());
}

//----------------------------------------------------------------------------------------------------------------------
Renderer::~Renderer() {

}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::Cleanup() {

}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::Pause() {

}
//----------------------------------------------------------------------------------------------------------------------
void Renderer::Resume() {

}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::Update(float time) {
  compositor_->Update(time);
};

//----------------------------------------------------------------------------------------------------------------------
void Renderer::Refresh() {
  shadow_->UpdateParams();
  compositor_->Init();
  Ogre::MaterialManager::getSingleton().load("Gorilla2D", Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::UpdateShadow(bool enable, float far_distance, int tex_size, int tex_format) {
  shadow_->UpdateParams(enable, far_distance, tex_size, tex_format);
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::UpdateParams(Ogre::TextureFilterOptions filtering, int anisotropy) {
  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(Ogre::MIP_UNLIMITED);
  Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(filtering);
  Ogre::MaterialManager::MaterialManager::getSingleton().setDefaultAnisotropy(anisotropy);
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::Resize(int w, int h, bool f) {
//  if (f) {
//    window_->SetFullscreen(f);
//    render_window_->resize(window_->GetSize().first, window_->GetSize().second);
//    render_window_->setFullscreen(f, window_->GetSize().first, window_->GetSize().second);
//  } else {
//    window_->Resize(w, h);
//    render_window_->resize(w, h);
//  }
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::RenderOneFrame() {
  root_->renderOneFrame();
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  window_->SwapBuffers();
#endif
}

} //namespace
