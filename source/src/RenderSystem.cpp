// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "RenderSystem.h"
#include "AssetLoader.h"
#include "Config.h"
#include "DotSceneLoaderB.h"
#include "Exception.h"
#include "RtssUtils.h"
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
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
#include "Android.h"
#endif

using namespace std;
using namespace Ogre;

namespace glue {

//----------------------------------------------------------------------------------------------------------------------
RenderSystem::RenderSystem(int w, int h, bool f) {
  ogre_root = new Root("");

  config->Get("render_system", render_system);

  InitOgreRenderSystem();
  InitOgrePlugins();

  ogre_root->initialise(false);

  window = make_unique<Window>(w, h, f);

  InitRenderWindow();
  RestoreFullscreenAndroid_();

  // Camera block
  camera = scene->createCamera("Default");
  auto *renderTarget = ogre_root->getRenderTarget(render_window->getName());
  viewport = renderTarget->addViewport(camera.get());
  viewport->setBackgroundColour(ColourValue::Black);
  camera->setAspectRatio(static_cast<float>(viewport->getActualWidth()) /
                          static_cast<float>(viewport->getActualHeight()));
  camera->setAutoAspectRatio(true);

  InitResourceLocation();

  // RTSS block
  glue::InitRtss();
  glue::CreateRtssShaders();

  // Overlay
  overlay = make_unique<Overlay>(render_window);

  AssetLoader::LoadResources();

  // Compositor block
  compositor = make_unique<Compositor>();

  overlay->PrepareTexture("Roboto-Medium", RGN_INTERNAL);

  InitTextureSettings();

  InitShadowSettings();
}

//----------------------------------------------------------------------------------------------------------------------
RenderSystem::~RenderSystem() {}

//----------------------------------------------------------------------------------------------------------------------
void RenderSystem::InitOgrePlugins() {
#ifdef OGRE_BUILD_PLUGIN_OCTREE
  Root::getSingleton().addSceneManagerFactory(new OctreeSceneManagerFactory());
#endif
#ifdef OGRE_BUILD_PLUGIN_PFX
  Root::getSingleton().installPlugin(new ParticleFXPlugin());
#endif
#ifdef OGRE_BUILD_PLUGIN_STBI
  Root::getSingleton().installPlugin(new STBIPlugin());
#endif
#ifdef OGRE_BUILD_PLUGIN_FREEIMAGE
  Root::getSingleton().installPlugin(new FreeImagePlugin());
#endif
#ifdef OGRE_BUILD_PLUGIN_ASSIMP
#ifdef DEBUG
  Root::getSingleton().installPlugin(new AssimpPlugin());
#endif
#endif
#ifdef OGRE_BUILD_PLUGIN_OCTREE
  scene = ogre_root->createSceneManager("OctreeSceneManager", "Default");
#else
  ogre_scene_ = root_->createSceneManager(ST_GENERIC, "Default");
#endif
  Root::getSingleton().installPlugin(new DotScenePluginB());
}

//----------------------------------------------------------------------------------------------------------------------
void RenderSystem::InitOgreRenderSystem() {
  if (render_system == "gl3") {
    InitOgreRenderSystemGL3();
  } else if (render_system == "gles2") {
    InitOgreRenderSystemGLES2();
  } else {
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID || OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
    InitOgreRenderSystem_GLES2_();
#else
    InitOgreRenderSystemGL3();
#endif
  }
}

//----------------------------------------------------------------------------------------------------------------------
void RenderSystem::InitOgreRenderSystemGL3() {
#ifdef OGRE_BUILD_RENDERSYSTEM_GL3PLUS
  auto *gl3plus_render_system = new GL3PlusRenderSystem();
  gl3plus_render_system->setConfigOption("Separate Shader Objects", "Yes");

  Root::getSingleton().setRenderSystem(gl3plus_render_system);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void RenderSystem::InitRenderWindow() {
  NameValuePairList params;

  SDL_SysWMinfo info = window->GetInfo();

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
  jmethodID method_get_assets = env->GetMethodID(class_resources, "getAssets", "()Landroid/content/res/AssetManager;");
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

  ArchiveManager::getSingleton().addArchiveFactory(new APKFileSystemArchiveFactory(asset_manager));
  ArchiveManager::getSingleton().addArchiveFactory(new APKZipArchiveFactory(asset_manager));
#endif

  const char true_str[] = "true";
  const char false_str[] = "false";

  bool vsync = true;
  bool gamma = false;
  int fsaa = 0;

  config->Get("vsync", vsync);
  config->Get("gamma", gamma);
  config->Get("fsaa", fsaa);

  params["vsync"] = vsync ? true_str : false_str;
  params["gamma"] = gamma ? true_str : false_str;
  params["FSAA"] = to_string(fsaa);

  render_window = Root::getSingleton().createRenderWindow("", 0, 0, false, &params);
}

//----------------------------------------------------------------------------------------------------------------------
void RenderSystem::InitResourceLocation() {
  const string internal_group = RGN_INTERNAL;
  const string default_group = RGN_DEFAULT;

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID

  AssetLoader::AddLocation("programs/main", internal_group);
  AssetLoader::AddLocation("programs/rtss", internal_group);
  AssetLoader::AddLocation("programs/pbr", internal_group);
  AssetLoader::AddLocation("programs/particles", internal_group);
  AssetLoader::AddLocation("programs/compositor", internal_group);
  AssetLoader::AddLocation("programs/overlay", internal_group);
  AssetLoader::AddLocation("programs/gorilla", internal_group);

  AssetLoader::AddLocationRecursive("assets", default_group, "resources.list");

#else

  ResourceGroupManager &resGroupMan = ResourceGroupManager::getSingleton();
  String defResGroup = RGN_DEFAULT;

  const std::string file_system = "APKFileSystem";
  const std::string zip = "APKZip";

  resGroupMan.addResourceLocation("/programs/main.zip", zip, internal_group);
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
void RenderSystem::InitShadowSettings() {
  bool shadows_enable = true;
  float shadow_far = 400;
  int16_t tex_size = 256;
  string tex_format = "D16";

  config->Get("shadows_enable", shadows_enable);
  config->Get("shadow_far", shadow_far);
  config->Get("tex_format", tex_format);
  config->Get("tex_size", tex_size);

  if (!shadows_enable) {
    scene->setShadowTechnique(SHADOWTYPE_NONE);
    return;
  }

  PixelFormat texture_type = PixelFormat::PF_DEPTH16;

  scene->setShadowTechnique(SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
  scene->setShadowFarDistance(shadow_far);

  if (tex_format == "F32") {
    texture_type = PixelFormat::PF_FLOAT32_R;
  } else if (tex_format == "F16") {
    texture_type = PixelFormat::PF_FLOAT16_R;
  }
  if (tex_format == "D32") {
    texture_type = PixelFormat::PF_DEPTH32;
  } else if (tex_format == "D16") {
    texture_type = PixelFormat::PF_DEPTH16;
  }

  scene->setShadowTextureSize(tex_size);
  scene->setShadowTexturePixelFormat(texture_type);
  scene->setShadowTextureCountPerLightType(Light::LT_DIRECTIONAL, 3);
  scene->setShadowTextureCountPerLightType(Light::LT_SPOTLIGHT, 1);
  scene->setShadowTextureCountPerLightType(Light::LT_POINT, 0);

  scene->setShadowTextureSelfShadow(true);
  scene->setShadowCasterRenderBackFaces(true);
  scene->setShadowFarDistance(shadow_far);
  auto passCaterMaterial = MaterialManager::getSingleton().getByName("PSSM/shadow_caster");
  scene->setShadowTextureCasterMaterial(passCaterMaterial);

  pssm = make_shared<PSSMShadowCameraSetup>();
  const float near_clip_distance = 0.001;
  pssm->calculateSplitPoints(pssm_split_count, near_clip_distance, scene->getShadowFarDistance());
  split_points = pssm->getSplitPoints();
  //  pssm_->setSplitPadding(near_clip_distance);
  pssm->setSplitPadding(0.1);

  for (size_t i = 0; i < pssm_split_count; i++) {
    pssm->setOptimalAdjustFactor(i, static_cast<float>(i));
  }

  scene->setShadowCameraSetup(pssm);
  scene->setShadowColour(ColourValue::Black);

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  TextureManager::getSingleton().setDefaultNumMipmaps(MIP_UNLIMITED);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void RenderSystem::InitTextureSettings() {
  string filtration = config->Get<string>("filtration");
  unsigned int anisotropy = 4;

  config->Get("anisotropy", anisotropy);

  TextureFilterOptions filtering = TFO_ANISOTROPIC;

  if (filtration == "anisotropic") {
    filtering = TFO_ANISOTROPIC;
  } else if (filtration == "bilinear") {
    filtering = TFO_BILINEAR;
  } else if (filtration == "trilinear") {
    filtering = TFO_TRILINEAR;
  } else if (filtration == "none") {
    filtering = TFO_NONE;
  }

  MaterialManager::getSingleton().setDefaultTextureFiltering(filtering);
  MaterialManager::getSingleton().setDefaultAnisotropy(anisotropy);
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
  InitShadowSettings();
  compositor->SetUp();
}

//----------------------------------------------------------------------------------------------------------------------
void RenderSystem::Resize(int w, int h, bool f) {
  window->SetFullscreen(f);
  window->Resize(w, h);
  render_window->resize(w, h);
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
  ogre_root->renderOneFrame();
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  window_->SwapBuffers();
#endif
}

//----------------------------------------------------------------------------------------------------------------------
Window &RenderSystem::GetWindow() { return *window; }

//----------------------------------------------------------------------------------------------------------------------
Compositor &RenderSystem::GetCompositor() { return *compositor; }

}  // namespace glue
