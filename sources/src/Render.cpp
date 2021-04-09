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
#include "Render.h"
#include "Assets.h"
#include "RtssUtils.h"
#include "Exception.h"


#ifdef OGRE_BUILD_RENDERSYSTEM_GL3PLUS
#include <RenderSystems/GL3Plus/OgreGL3PlusRenderSystem.h>
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
#ifdef OGRE_BUILD_COMPONENT_OVERLAY
#include <Overlay/OgreOverlayManager.h>
#include <Overlay/OgreImGuiOverlay.h>
#endif


#include "Android.h"

using namespace std;
namespace fs = filesystem;

namespace xio {
Render::Render(int w, int h, bool f) {
  root_ = new Ogre::Root("");

  conf_->Get("render_system", render_system_);

  InitOgreRenderSystem_();
  InitOgrePlugins_();

  root_->initialise(false);

  window_ = make_unique<Window>(w, h, f);

  InitRenderWindow_();
  RestoreFullscreenAndroid_();

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
  compositor_->EnableEffect("motion", conf_->Get<bool>("compositor_use_motion"));
  compositor_->Init();

  overlay_->PrepareTexture("Roboto-Medium", Ogre::RGN_INTERNAL);

  // Shadows param
  bool shadow_enable = conf_->Get<bool>("shadows_enable");
  float shadow_far = conf_->Get<float>("shadows_far_distance");
  int16_t tex_size = conf_->Get<int>("shadows_texture_resolution");
  int tex_format = conf_->Get<int>("shadows_texture_format");

  shadow_settings_->UpdateParams(shadow_enable, shadow_far, tex_size, tex_format);

}

//----------------------------------------------------------------------------------------------------------------------
Render::~Render() {

}

//----------------------------------------------------------------------------------------------------------------------
void Render::InitOgrePlugins_() {

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

}//----------------------------------------------------------------------------------------------------------------------
void Render::InitOgreRenderSystem_() {
  if (render_system_ == "gl3")
  	InitOgreRenderSystem_GL3_();
  else if (render_system_ == "gles2")
  	InitOgreRenderSystem_GLES2_();
  else {
#if OGRE_PLATFORM==OGRE_PLATFORM_ANDROID || OGRE_PLATFORM==OGRE_PLATFORM_APPLE_IOS
	InitOgreRenderSystem_GLES2_();
#else
	InitOgreRenderSystem_GL3_();
#endif
  }

}

//----------------------------------------------------------------------------------------------------------------------
void Render::InitOgreRenderSystem_GL3_() {
#ifdef OGRE_BUILD_RENDERSYSTEM_GL3PLUS
  auto *gl3plus_render_system = new Ogre::GL3PlusRenderSystem();
  gl3plus_render_system->setConfigOption("Separate Shader Objects", "Yes");

  Ogre::Root::getSingleton().setRenderSystem(gl3plus_render_system);
#endif

}

//----------------------------------------------------------------------------------------------------------------------
void Render::InitRenderWindow_() {
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

  bool vsync = true;
  bool gamma = false;
  int fsaa = 0;

  conf_->Get("vsync", vsync);
  conf_->Get("gamma", gamma);
  conf_->Get("fsaa", fsaa);

  params["vsync"] = vsync ? true_str : false_str;
  params["gamma"] = gamma ? true_str : false_str;
  params["FSAA"] = to_string(fsaa);


  render_window_ = Ogre::Root::getSingleton().createRenderWindow("", 0, 0, false, &params);

}

//----------------------------------------------------------------------------------------------------------------------
void Render::InitResourceLocation_() {
  const string internal_group = Ogre::RGN_INTERNAL;
  const string default_group = Ogre::RGN_DEFAULT;

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID

  Assets::AddLocation("programs/common", internal_group);
  Assets::AddLocation("programs/rtss", internal_group);
  Assets::AddLocation("programs/pbr", internal_group);
  Assets::AddLocation("programs/particles", internal_group);
  Assets::AddLocation("programs/compositor", internal_group);
  Assets::AddLocation("programs/overlay", internal_group);
  Assets::AddLocation("programs/gorilla", internal_group);

  Assets::AddLocationRecursive("assets", default_group, "resources.list");

#else
  Ogre::ResourceGroupManager& resGroupMan = Ogre::ResourceGroupManager::getSingleton();
  Ogre::String defResGroup = Ogre::RGN_DEFAULT;

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
void Render::Cleanup() {

}

//----------------------------------------------------------------------------------------------------------------------
void Render::Pause() {

}
//----------------------------------------------------------------------------------------------------------------------
void Render::Resume() {

}

//----------------------------------------------------------------------------------------------------------------------
void Render::Update(float time) {

};

//----------------------------------------------------------------------------------------------------------------------
void Render::Refresh() {
  if (shadow_settings_) shadow_settings_->UpdateParams();
  compositor_->Init();
}

//----------------------------------------------------------------------------------------------------------------------
void Render::UpdateShadow(bool enable, float far_distance, int tex_size, int tex_format) {
  if (shadow_settings_) shadow_settings_->UpdateParams(enable, far_distance, tex_size, tex_format);
}

//----------------------------------------------------------------------------------------------------------------------
void Render::UpdateParams(Ogre::TextureFilterOptions filtering, int anisotropy) {
  //Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(Ogre::MIP_UNLIMITED);
  Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(filtering);
  Ogre::MaterialManager::MaterialManager::getSingleton().setDefaultAnisotropy(anisotropy);
}

//----------------------------------------------------------------------------------------------------------------------
void Render::Resize(int w, int h, bool f) {
  window_->SetFullscreen(f);
  window_->Resize(w, h);
  render_window_->resize(w, h);
}

//----------------------------------------------------------------------------------------------------------------------
void Render::RestoreFullscreenAndroid_() {
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  SDL_DisplayMode DM;
  SDL_GetDesktopDisplayMode(0, &DM);
  int screen_w = static_cast<int>(DM.w);
  int screen_h = static_cast<int>(DM.h);
//  Resize(screen_w, screen_h, true);
  render_window_->resize(screen_w, screen_h);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void Render::RenderOneFrame() {
  root_->renderOneFrame();
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  window_->SwapBuffers();
#endif
}

//----------------------------------------------------------------------------------------------------------------------
Window &Render::GetWindow() {
  return *window_;
}

//----------------------------------------------------------------------------------------------------------------------
Compositor &Render::GetCompositor() {
  return *compositor_;
}

} //namespace
