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

#include "RenderSystem.h"

#include "AssetManager.h"
#include "Config.h"
#include "Exception.h"
#include "RtssUtils.h"
#include "pcheader.h"

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
#include <Overlay/OgreImGuiOverlay.h>
#endif

#include "Android.h"

using namespace std;

namespace xio {

//----------------------------------------------------------------------------------------------------------------------
RenderSystem::RenderSystem(int w, int h, bool f) {
  root_ = new Ogre::Root("");

  conf_->Get("render_system", render_system_);

  InitOgreRenderSystem_();
  InitOgrePlugins_();

  root_->initialise(false);

  window_ = make_unique<Window>(w, h, f);

  InitRenderWindow_();
  RestoreFullscreenAndroid_();

  // Camera block
  camera_ = scene_->createCamera("Default");
  auto *renderTarget = root_->getRenderTarget(render_window_->getName());
  viewport_ = renderTarget->addViewport(camera_.get());
  viewport_->setBackgroundColour(Ogre::ColourValue::Black);
  camera_->setAspectRatio(static_cast<float>(viewport_->getActualWidth()) /
                          static_cast<float>(viewport_->getActualHeight()));
  camera_->setAutoAspectRatio(true);

  InitResourceLocation_();

  // RTSS block
  xio::InitRtss();
  xio::CreateRtssShaders();

  // Overlay
  overlay_ = make_unique<Overlay>(render_window_);

  AssetManager::LoadResources();

  // Compositor block
  compositor_ = make_unique<Compositor>();

  overlay_->PrepareTexture("Roboto-Medium", Ogre::RGN_INTERNAL);

  InitTextureSettings_();

  InitShadowSettings_();
}

//----------------------------------------------------------------------------------------------------------------------
RenderSystem::~RenderSystem() {}

//----------------------------------------------------------------------------------------------------------------------
void RenderSystem::InitOgrePlugins_() {
#ifdef OGRE_BUILD_PLUGIN_OCTREE
  Ogre::Root::getSingleton().addSceneManagerFactory(new Ogre::OctreeSceneManagerFactory());
#endif  // OGRE_BUILD_PLUGIN_OCTREE
#ifdef OGRE_BUILD_PLUGIN_PFX
  Ogre::Root::getSingleton().installPlugin(new Ogre::ParticleFXPlugin());
#endif  // OGRE_BUILD_PLUGIN_PFX
#ifdef OGRE_BUILD_PLUGIN_STBI
  Ogre::Root::getSingleton().installPlugin(new Ogre::STBIPlugin());
#endif  // OGRE_BUILD_PLUGIN_STBI
#ifdef OGRE_BUILD_PLUGIN_FREEIMAGE
  Ogre::Root::getSingleton().installPlugin(new Ogre::FreeImagePlugin());
#endif
#ifdef OGRE_BUILD_PLUGIN_ASSIMP
#ifdef DEBUG
  Ogre::Root::getSingleton().installPlugin(new Ogre::AssimpPlugin());
#endif
#endif
#ifdef OGRE_BUILD_PLUGIN_OCTREE
  scene_ = root_->createSceneManager("OctreeSceneManager", "Default");
#else
  ogre_scene_ = root_->createSceneManager(Ogre::ST_GENERIC, "Default");
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void RenderSystem::InitOgreRenderSystem_() {
  if (render_system_ == "gl3") {
    InitOgreRenderSystem_GL3_();
  } else if (render_system_ == "gles2") {
    InitOgreRenderSystem_GLES2_();
  } else {
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID || OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
    InitOgreRenderSystem_GLES2_();
#else
    InitOgreRenderSystem_GL3_();
#endif
  }
}

//----------------------------------------------------------------------------------------------------------------------
void RenderSystem::InitOgreRenderSystem_GL3_() {
#ifdef OGRE_BUILD_RENDERSYSTEM_GL3PLUS
  auto *gl3plus_render_system = new Ogre::GL3PlusRenderSystem();
  gl3plus_render_system->setConfigOption("Separate Shader Objects", "Yes");

  Ogre::Root::getSingleton().setRenderSystem(gl3plus_render_system);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void RenderSystem::InitRenderWindow_() {
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

  jclass class_activity = env->FindClass("android/app/Activity");
  jclass class_resources = env->FindClass("android/content/res/Resources");
  jmethodID method_get_resources =
      env->GetMethodID(class_activity, "getResources", "()Landroid/content/res/Resources;");
  jmethodID method_get_assets =
      env->GetMethodID(class_resources, "getAssets", "()Landroid/content/res/AssetManager;");
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

  Ogre::ArchiveManager::getSingleton().addArchiveFactory(
      new Ogre::APKFileSystemArchiveFactory(asset_manager));
  Ogre::ArchiveManager::getSingleton().addArchiveFactory(
      new Ogre::APKZipArchiveFactory(asset_manager));
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
void RenderSystem::InitResourceLocation_() {
  const string internal_group = Ogre::RGN_INTERNAL;
  const string default_group = Ogre::RGN_DEFAULT;

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID

  AssetManager::AddLocation("programs/common", internal_group);
  AssetManager::AddLocation("programs/rtss", internal_group);
  AssetManager::AddLocation("programs/pbr", internal_group);
  AssetManager::AddLocation("programs/particles", internal_group);
  AssetManager::AddLocation("programs/compositor", internal_group);
  AssetManager::AddLocation("programs/overlay", internal_group);
  AssetManager::AddLocation("programs/gorilla", internal_group);

  AssetManager::AddLocationRecursive("assets", default_group, "resources.list");

#else

  Ogre::ResourceGroupManager &resGroupMan = Ogre::ResourceGroupManager::getSingleton();
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
void RenderSystem::InitShadowSettings_() {
  bool shadows_enable = true;
  float shadow_far = 400;
  int16_t tex_size = 256;
  string tex_format = "D16";

  conf_->Get("shadows_enable", shadows_enable);
  conf_->Get("shadow_far", shadow_far);
  conf_->Get("tex_format", tex_format);
  conf_->Get("tex_size", tex_size);

  if (!shadows_enable) {
    scene_->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
    return;
  }

  Ogre::PixelFormat texture_type = Ogre::PixelFormat::PF_DEPTH16;

  scene_->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
  scene_->setShadowFarDistance(shadow_far);

  if (tex_format == "F32") {
    texture_type = Ogre::PixelFormat::PF_FLOAT32_R;
  } else if (tex_format == "F16") {
    texture_type = Ogre::PixelFormat::PF_FLOAT16_R;
  }
  if (tex_format == "D32") {
    texture_type = Ogre::PixelFormat::PF_DEPTH32;
  } else if (tex_format == "D16") {
    texture_type = Ogre::PixelFormat::PF_DEPTH16;
  }

  scene_->setShadowTextureSize(tex_size);
  scene_->setShadowTexturePixelFormat(texture_type);
  scene_->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, 3);
  scene_->setShadowTextureCountPerLightType(Ogre::Light::LT_SPOTLIGHT, 1);
  scene_->setShadowTextureCountPerLightType(Ogre::Light::LT_POINT, 0);

  scene_->setShadowTextureSelfShadow(true);
  scene_->setShadowCasterRenderBackFaces(true);
  scene_->setShadowFarDistance(shadow_far);
  auto passCaterMaterial = Ogre::MaterialManager::getSingleton().getByName("PSSM/shadow_caster");
  scene_->setShadowTextureCasterMaterial(passCaterMaterial);

  pssm_ = make_shared<Ogre::PSSMShadowCameraSetup>();
  const float near_clip_distance = 0.001;
  pssm_->calculateSplitPoints(pssm_split_count_, near_clip_distance,
                              scene_->getShadowFarDistance());
  split_points_ = pssm_->getSplitPoints();
  //  pssm_->setSplitPadding(near_clip_distance);
  pssm_->setSplitPadding(0.1);

  for (int i = 0; i < pssm_split_count_; i++) {
    pssm_->setOptimalAdjustFactor(i, static_cast<float>(i));
  }

  scene_->setShadowCameraSetup(pssm_);
  scene_->setShadowColour(Ogre::ColourValue::Black);

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(Ogre::MIP_UNLIMITED);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void RenderSystem::InitTextureSettings_() {
  string filtration = conf_->Get<string>("filtration");
  unsigned int anisotropy = 4;

  conf_->Get("anisotropy", anisotropy);

  Ogre::TextureFilterOptions filtering = Ogre::TFO_ANISOTROPIC;

  if (filtration == "anisotropic") {
    filtering = Ogre::TFO_ANISOTROPIC;
  } else if (filtration == "bilinear") {
    filtering = Ogre::TFO_BILINEAR;
  } else if (filtration == "trilinear") {
    filtering = Ogre::TFO_TRILINEAR;
  } else if (filtration == "none") {
    filtering = Ogre::TFO_NONE;
  }

  Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(filtering);
  Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(anisotropy);
}

//----------------------------------------------------------------------------------------------------------------------
void RenderSystem::Cleanup() {}

//----------------------------------------------------------------------------------------------------------------------
void RenderSystem::Pause() {}

//----------------------------------------------------------------------------------------------------------------------
void RenderSystem::Resume() {}

//----------------------------------------------------------------------------------------------------------------------
void RenderSystem::Update(float time) {}

//----------------------------------------------------------------------------------------------------------------------
void RenderSystem::Refresh() {
  InitShadowSettings_();
  compositor_->Init();
}

//----------------------------------------------------------------------------------------------------------------------
void RenderSystem::Resize(int w, int h, bool f) {
  window_->SetFullscreen(f);
  window_->Resize(w, h);
  render_window_->resize(w, h);
}

//----------------------------------------------------------------------------------------------------------------------
void RenderSystem::RestoreFullscreenAndroid_() {
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  SDL_DisplayMode DM;
  SDL_GetDesktopDisplayMode(0, &DM);
  int screen_w = static_cast<int>(DM.w);
  int screen_h = static_cast<int>(DM.h);
  render_window_->resize(screen_w, screen_h);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void RenderSystem::RenderOneFrame() {
  root_->renderOneFrame();
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  window_->SwapBuffers();
#endif
}

//----------------------------------------------------------------------------------------------------------------------
Window &RenderSystem::GetWindow() { return *window_; }

//----------------------------------------------------------------------------------------------------------------------
Compositor &RenderSystem::GetCompositor() { return *compositor_; }

}  // namespace xio
