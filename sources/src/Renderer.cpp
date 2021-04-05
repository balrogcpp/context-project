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


#ifdef OGRE_BUILD_RENDERSYSTEM_GL3PLUS
#include <RenderSystems/GL3Plus/OgreGL3PlusRenderSystem.h>
#elif defined OGRE_BUILD_RENDERSYSTEM_GLES2
#include <RenderSystems/GLES2/OgreGLES2RenderSystem.h>
#endif
#ifdef OGRE_BUILD_PLUGIN_OCTREE
#include <Plugins/OctreeSceneManager/OgreOctreeSceneManager.h>
#endif
#ifdef OGRE_BUILD_PLUGIN_PFX
#include <Plugins/ParticleFX/OgreParticleFXPlugin.h>
#endif
#ifdef OGRE_BUILD_PLUGIN_STBI
#include <Plugins/STBICodec/OgreSTBICodec.h>
#endif
#ifdef OGRE_BUILD_PLUGIN_FREEIMAGE
#include <Plugins/FreeImageCodec/OgreFreeImageCodec.h>
#include <Plugins/FreeImageCodec/OgreFreeImageCodecExports.h>
#endif
#ifdef OGRE_BUILD_PLUGIN_ASSIMP
#include <Plugins/Assimp/OgreAssimpLoader.h>
#endif
//#ifdef OGRE_BUILD_COMPONENT_VOLUME
//#include <Volume/OgreVolumeChunk.h>
//#include <Volume/OgreVolumeCSGSource.h>
//#include <Volume/OgreVolumeCacheSource.h>
//#include <Volume/OgreVolumeTextureSource.h>
//#endif
#ifdef OGRE_BUILD_COMPONENT_OVERLAY
#include <Overlay/OgreOverlayManager.h>
#include <Overlay/OgreImGuiOverlay.h>
#endif


#include "Android.h"

using namespace std;
namespace fs = filesystem;

namespace xio {
Renderer::Renderer(int w, int h, bool f) {
  root_ = new Ogre::Root("");

  InitOgrePlugins_();

  root_->initialise(false);

  window_ = make_unique<Window>(w, h, f);

  InitRenderWindow_();

  //Camera block
  camera_ = scene_->createCamera("Default");
  auto *renderTarget = root_->getRenderTarget(render_window_->getName());
  viewport_ = renderTarget->addViewport(camera_.get());
  viewport_->setBackgroundColour(Ogre::ColourValue::Black);
  camera_->setAspectRatio(static_cast<float>(viewport_->getActualWidth()) / static_cast<float>(viewport_->getActualHeight()));
  camera_->setAutoAspectRatio(true);

  InitResourceLocation_();


  //RTSS block
  xio::InitRtss();
  xio::CreateRtssShaders();

  // Overlay
  overlay_ = make_unique<Overlay>(render_window_);


  Assets::LoadResources();

  //Shadow block
  shadow_settings_ = make_unique<ShadowSettings>();
  //Compositor block
  compositor_ = make_unique<Compositor>();


  compositor_->EnableEffect("ssao", conf_->Get<bool>("compositor_use_ssao"));
  compositor_->EnableEffect("bloom", conf_->Get<bool>("compositor_use_bloom"));
  compositor_->EnableEffect("hdr", conf_->Get<bool>("compositor_use_hdr"));
  compositor_->EnableEffect("motion", conf_->Get<bool>("compositor_use_motion"));
  compositor_->Init();

  overlay_->PrepareTexture("Roboto-Medium", Ogre::RGN_INTERNAL);
}

//----------------------------------------------------------------------------------------------------------------------
Renderer::~Renderer() {

}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::InitOgrePlugins_() {
#ifdef OGRE_BUILD_RENDERSYSTEM_GL3PLUS
  auto *gl3plus_render_system = new Ogre::GL3PlusRenderSystem();
  gl3plus_render_system->setConfigOption("Separate Shader Objects", "Yes");

  Ogre::Root::getSingleton().setRenderSystem(gl3plus_render_system);
#elif defined OGRE_BUILD_RENDERSYSTEM_GLES2
  Ogre::Root::getSingleton().setRenderSystem(new Ogre::GLES2RenderSystem());
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
#ifdef OGRE_BUILD_PLUGIN_OCTREE
  scene_ = root_->createSceneManager("OctreeSceneManager", "Default");
#else
  ogre_scene_ = root_->createSceneManager(Ogre::ST_GENERIC, "Default");
#endif

}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::InitRenderWindow_() {
  Ogre::NameValuePairList params;

  SDL_SysWMinfo info = window_->GetInfo();

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  params["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(info.info.win.window));
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
  params["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(info.info.x11.window));
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
  params["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(info.info.cocoa.window));
#elif OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();

  jclass class_activity       = env->FindClass("android/app/Activity");
  jclass class_resources      = env->FindClass("android/content/res/Resources");
  jmethodID method_get_resources      = env->GetMethodID(class_activity, "getResources", "()Landroid/content/res/Resources;");
  jmethodID method_get_assets         = env->GetMethodID(class_resources, "getAssets", "()Landroid/content/res/AssetManager;");
  jobject raw_activity = (jobject)SDL_AndroidGetActivity();
  jobject raw_resources = env->CallObjectMethod(raw_activity, method_get_resources);
  jobject raw_asset_manager = env->CallObjectMethod(raw_resources, method_get_assets);

  params["currentGLContext"] = "true";
  params["externalGLControl"] = "true";
  params["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(info.info.android.window));
  params["preserveContext"] = "true";

  AAssetManager* asset_manager = AAssetManager_fromJava(env, raw_asset_manager);
  AConfiguration* mAConfig = AConfiguration_new();
  AConfiguration_fromAssetManager(mAConfig, asset_manager);

  Ogre::ArchiveManager::getSingleton().addArchiveFactory(new Ogre::APKFileSystemArchiveFactory(asset_manager));
  Ogre::ArchiveManager::getSingleton().addArchiveFactory(new Ogre::APKZipArchiveFactory(asset_manager));
#endif

  const char true_str[] = "true";
  const char false_str[] = "false";

  bool vsync_ = conf_->Get<bool>("graphics_vsync");
  bool gamma_ = conf_->Get<bool>("graphics_gamma");
  int fsaa_ = conf_->Get<int>("graphics_fsaa");

  params["vsync"] = vsync_ ? true_str : false_str;
  params["gamma"] = gamma_ ? true_str : false_str;
  params["FSAA"] = to_string(fsaa_);


  render_window_ = Ogre::Root::getSingleton().createRenderWindow("", 0, 0, false, &params);

}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::InitResourceLocation_() {
  const string internal_group = Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME;
  const string default_group = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID

  Assets::AddLocation("programs/common", internal_group);
  Assets::AddLocation("programs/rtss", internal_group);
  Assets::AddLocation("programs/pbr", Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME);
  Assets::AddLocation("programs/particles", internal_group);
  Assets::AddLocation("programs/compositor", internal_group);
  Assets::AddLocation("programs/overlay", internal_group);
  Assets::AddLocation("programs/gorilla", internal_group);

  Assets::AddLocationRecursive("assets", default_group, "resources.list");

#else
  Ogre::ResourceGroupManager& resGroupMan = Ogre::ResourceGroupManager::getSingleton();
  Ogre::String defResGroup = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;

  const std::string file_system = "APKFileSystem";
  const std::string zip = "APKZip";

  resGroupMan.addResourceLocation("/programs/common.zip", zip, internal_group);
  resGroupMan.addResourceLocation("/programs/rtss.zip", zip, internal_group);
  resGroupMan.addResourceLocation("/programs/pbr.zip", zip, internal_group);
  resGroupMan.addResourceLocation("/programs/particles.zip", zip, internal_group);
  resGroupMan.addResourceLocation("/programs/compositor.zip", zip, internal_group);
  resGroupMan.addResourceLocation("/programs/overlay.zip", zip, internal_group);
  resGroupMan.addResourceLocation("/programs/gorilla.zip", zip, internal_group);

  resGroupMan.addResourceLocation("/assets/material.zip", zip, default_group);
  resGroupMan.addResourceLocation("/assets/models.zip", zip, default_group);
  resGroupMan.addResourceLocation("/assets/music.zip", zip, default_group);
  resGroupMan.addResourceLocation("/assets/scenes.zip", zip, default_group);
  resGroupMan.addResourceLocation("/assets/sounds.zip", zip, default_group);
  resGroupMan.addResourceLocation("/assets/terrain.zip", zip, default_group);
  resGroupMan.addResourceLocation("/assets/vegetation.zip", zip, default_group);


#endif

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
  //compositor_->Update(time);
  //overlay_->Update(time);
};

//----------------------------------------------------------------------------------------------------------------------
void Renderer::Refresh() {
  if (shadow_settings_) shadow_settings_->UpdateParams();
  compositor_->Init();
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::UpdateShadow(bool enable, float far_distance, int tex_size, int tex_format) {
  if (shadow_settings_) shadow_settings_->UpdateParams(enable, far_distance, tex_size, tex_format);
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::UpdateParams(Ogre::TextureFilterOptions filtering, int anisotropy) {
  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(Ogre::MIP_UNLIMITED);
  Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(filtering);
  Ogre::MaterialManager::MaterialManager::getSingleton().setDefaultAnisotropy(anisotropy);
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::Resize(int w, int h, bool f) {
  render_window_->resize(w, h);
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::RestoreFullscreenAndroid() {
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  SDL_DisplayMode DM;
  SDL_GetCurrentDisplayMode(0, &DM);
  int screen_w = static_cast<int>(DM.w);
  int screen_h = static_cast<int>(DM.h);
  Resize(screen_w, screen_h, true);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::RenderOneFrame() {
  root_->renderOneFrame();
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  window_->SwapBuffers();
#endif
}

//----------------------------------------------------------------------------------------------------------------------
Window &Renderer::GetWindow() {
  return *window_;
}

//----------------------------------------------------------------------------------------------------------------------
ShadowSettings &Renderer::GetShadowSettings() {
  return *shadow_settings_;
}

//----------------------------------------------------------------------------------------------------------------------
Compositor &Renderer::GetCompositor() {
  return *compositor_;
}

} //namespace
