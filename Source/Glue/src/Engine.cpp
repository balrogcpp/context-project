// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Engine.h"
#include "AssetLoader.h"
#include "Components/Audio.h"
#include "Components/Component.h"
#include "Components/Compositor.h"
#include "Components/Overlay.h"
#include "Components/Physics.h"
#include "Components/DotSceneLoaderB.h"
#include "PhysicalInput/InputSequencer.h"
#include "Config.h"
#include "RTSSUtils.h"
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
extern "C" {
#include <SDL2/SDL_syswm.h>
}

/// MinGW has macro CreateWindow that shadows function
#undef CreateWindow

using namespace std;
using namespace Ogre;

namespace Glue {

Engine::Engine() {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  ConfPtr = make_unique<Config>("config.json");
#else
  ConfPtr = make_unique<Config>("");
  ConfPtr->AddMember("window_fullscreen", true);
  ConfPtr->AddMember("compositor_use_bloom", false);
  ConfPtr->AddMember("compositor_use_ssao", false);
  ConfPtr->AddMember("compositor_use_motion", false);
  ConfPtr->AddMember("target_fps", 30);
  ConfPtr->AddMember("lock_fps", true);
  ConfPtr->AddMember("vsync", false);
  ConfPtr->AddMember("shadows_enable", false);
  ConfPtr->AddMember("fsaa", 0);
  ConfPtr->AddMember("filtration", "bilinear");
  ConfPtr->AddMember("anisotropy_level", 8);
  ConfPtr->AddMember("shadows_texture_resolution", 512);
  ConfPtr->AddMember("shadows_far_distance", 400);
  ConfPtr->AddMember("shadows_texture_format", 16);
#endif

  Component::SetConfig(ConfPtr.get());
  ComponentList.reserve(16);
}

Engine::~Engine() {
  SDL_SetWindowFullscreen(SDLWindowPtr, SDL_FALSE);
  SDL_DestroyWindow(SDLWindowPtr);
}

void Engine::InitSystems() {
  int window_width = ConfPtr->Get<int>("window_width");
  int window_high = ConfPtr->Get<int>("window_high");
  bool window_fullscreen = ConfPtr->Get<bool>("window_fullscreen");

  // RS = make_unique<Render>(window_width, window_high, window_fullscreen);
  OgreRoot = new Ogre::Root("");

  ConfPtr->Get("render_system", RenderSystemName);

  InitDefaultRenderSystem();
  InitOgrePlugins();

  OgreRoot->initialise(false);

  CreateWindow();

  InitRenderWindow();
  WindowRestoreFullscreenAndroid();

  // Camera block
  OgreCamera = OgreSceneManager->createCamera("Default");
  auto* renderTarget = OgreRoot->getRenderTarget(OgreRenderWindowPtr->getName());
  OgreViewport = renderTarget->addViewport(OgreCamera);
  OgreViewport->setBackgroundColour(ColourValue::Black);
  OgreCamera->setAspectRatio(static_cast<float>(OgreViewport->getActualWidth()) /
                             static_cast<float>(OgreViewport->getActualHeight()));
  OgreCamera->setAutoAspectRatio(true);

  InitResourceLocation();

  // RTSS block
  Glue::InitRtss();
  Glue::CreateRtssShaders();

  InitTextureSettings();

  InitShadowSettings();

  // Overlay
  OverlayPtr = make_unique<Overlay>(OgreRenderWindowPtr);

  AssetLoader::LoadResources();

  // Compositor block
  CompositorUPtr = make_unique<Compositor>();

  OverlayPtr->PrepareTexture("Roboto-Medium", Ogre::RGN_INTERNAL);

  ps = make_unique<Physics>();
  as = make_unique<Audio>(8, 8);

  loader = make_unique<DotSceneLoaderB>();

  CompositorUPtr->SetUp();
}

void Engine::Capture() {
  static auto& io = InputSequencer::GetInstance();
  io.Capture();
}

void Engine::RegSystem(Component* system) { ComponentList.push_back(system); }

void Engine::Pause() {
  for (auto& it : ComponentList) it->Pause();
}

void Engine::InMenu() {
  ps->Pause();
  loader->Pause();
}

void Engine::OffMenu() {
  ps->Resume();
  loader->Resume();
}

void Engine::Resume() {
  for (auto& it : ComponentList) it->Resume();
}

void Engine::Cleanup() {
  for (auto& it : ComponentList) it->Cleanup();
  Refresh();
}

void Engine::Update(float PassedTime) {
  for (auto& it : ComponentList) it->Update(PassedTime);
}

void Engine::RenderOneFrame() {
  OgreRoot->renderOneFrame();
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  SDL_GL_SwapWindow(SDLWindowPtr);
#endif
}

void Engine::Refresh() {
  InitShadowSettings();
  CompositorUPtr->SetUp();
}

void Engine::InitOgrePlugins() {
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
  OgreSceneManager = OgreRoot->createSceneManager("OctreeSceneManager", "Default");
#else
  OgreSceneManager = OgreRoot->createSceneManager(ST_GENERIC, "Default");
#endif
  Root::getSingleton().installPlugin(new DotScenePluginB());
}

void Engine::InitDefaultRenderSystem() {
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID || OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
  InitOgreRenderSystemGLES2();
#else
  InitOgreRenderSystemGL3();
#endif
}

void Engine::InitRenderWindow() {
  NameValuePairList params;

  SDL_SysWMinfo info;
  SDL_VERSION(&info.version);
  SDL_GetWindowWMInfo(SDLWindowPtr, &info);

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

  const char true_str[] = "True";
  const char false_str[] = "False";

  bool vsync = true;
  bool gamma = false;
  int fsaa = 0;

  ConfPtr->Get("vsync", vsync);
  ConfPtr->Get("gamma", gamma);
  ConfPtr->Get("fsaa", fsaa);

  params["vsync"] = vsync ? true_str : false_str;
  params["gamma"] = gamma ? true_str : false_str;
  params["FSAA"] = to_string(fsaa);

  OgreRenderWindowPtr = Root::getSingleton().createRenderWindow("MainWindow", 0, 0, false, &params);
}

void Engine::InitResourceLocation() {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  AssetLoader::AddLocation("Programs/Main", RGN_INTERNAL);
  AssetLoader::AddLocation("Programs/RTSS", RGN_INTERNAL);
  AssetLoader::AddLocation("Programs/PBR", RGN_INTERNAL);
  AssetLoader::AddLocation("Programs/Particles", RGN_INTERNAL);
  AssetLoader::AddLocation("Programs/Compositor", RGN_INTERNAL);
  AssetLoader::AddLocation("Programs/Overlay", RGN_INTERNAL);
  // AssetLoader::AddLocation("Programs/filament", RGN_INTERNAL);

  AssetLoader::AddLocationRecursive("Assets", RGN_DEFAULT, "Resources.list");
#else
  ResourceGroupManager& resGroupMan = ResourceGroupManager::getSingleton();

  resGroupMan.addResourceLocation("/Programs/Main.zip", "Zip", RGN_INTERNAL);
  resGroupMan.addResourceLocation("/Programs/RTSS.zip", "Zip", RGN_INTERNAL);
  resGroupMan.addResourceLocation("/Programs/PBR.zip", "Zip", RGN_INTERNAL);
  resGroupMan.addResourceLocation("/Programs/Particles.zip", "Zip", RGN_INTERNAL);
  resGroupMan.addResourceLocation("/Programs/Compositor.zip", "Zip", RGN_INTERNAL);
  resGroupMan.addResourceLocation("/Programs/Overlay.zip", "Zip", RGN_INTERNAL);
  // resGroupMan.addResourceLocation("/Programs/filament.zip", "Zip", RGN_INTERNAL);

  resGroupMan.addResourceLocation("/Assets/Materials.zip", "Zip", RGN_DEFAULT);
  resGroupMan.addResourceLocation("/Assets/Models.zip", "Zip", RGN_DEFAULT);
  resGroupMan.addResourceLocation("/Assets/Scenes.zip", "Zip", RGN_DEFAULT);
  resGroupMan.addResourceLocation("/Assets/Sounds.zip", "Zip", RGN_DEFAULT);
#endif
}

void Engine::InitShadowSettings() {
  bool shadows_enable = true;
  float shadow_far = 400;
  int16_t tex_size = 256;
  string tex_format = "D16";

  ConfPtr->Get("shadows_enable", shadows_enable);
  ConfPtr->Get("shadow_far", shadow_far);
  ConfPtr->Get("tex_format", tex_format);
  ConfPtr->Get("tex_size", tex_size);

  if (!shadows_enable) {
    OgreSceneManager->setShadowTechnique(SHADOWTYPE_NONE);
    return;
  }

  PixelFormat texture_type = PixelFormat::PF_DEPTH16;

  OgreSceneManager->setShadowTechnique(SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
  OgreSceneManager->setShadowFarDistance(shadow_far);

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

  OgreSceneManager->setShadowTextureSize(tex_size);
  OgreSceneManager->setShadowTexturePixelFormat(texture_type);
  OgreSceneManager->setShadowTextureCountPerLightType(Light::LT_DIRECTIONAL, 3);
  OgreSceneManager->setShadowTextureCountPerLightType(Light::LT_SPOTLIGHT, 1);
  OgreSceneManager->setShadowTextureCountPerLightType(Light::LT_POINT, 0);

  OgreSceneManager->setShadowTextureSelfShadow(true);
  OgreSceneManager->setShadowCasterRenderBackFaces(true);
  OgreSceneManager->setShadowFarDistance(shadow_far);
  auto passCaterMaterial = MaterialManager::getSingleton().getByName("PSSM/shadow_caster");
  OgreSceneManager->setShadowTextureCasterMaterial(passCaterMaterial);

  PSSMSetupPtr = make_shared<PSSMShadowCameraSetup>();
  const float near_clip_distance = 0.001;
  PSSMSetupPtr->calculateSplitPoints(PSSMSplitCount, near_clip_distance, OgreSceneManager->getShadowFarDistance());
  PSSMSplitPointList = PSSMSetupPtr->getSplitPoints();
  //  pssm_->setSplitPadding(near_clip_distance);
  PSSMSetupPtr->setSplitPadding(0.1);

  for (size_t i = 0; i < PSSMSplitCount; i++) {
    PSSMSetupPtr->setOptimalAdjustFactor(i, static_cast<float>(i));
  }

  OgreSceneManager->setShadowCameraSetup(PSSMSetupPtr);
  OgreSceneManager->setShadowColour(ColourValue::Black);

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  TextureManager::getSingleton().setDefaultNumMipmaps(MIP_UNLIMITED);
#endif
}

void Engine::InitTextureSettings() {
  string filtration = ConfPtr->Get<string>("filtration");
  unsigned int anisotropy = 4;

  ConfPtr->Get("anisotropy", anisotropy);

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

void Engine::ResizeWindow(int Width, int Height) {
  WindowWidth = Width;
  WindowHeight = Height;
  SDL_SetWindowPosition(SDLWindowPtr, (ScreenWidth - WindowWidth) / 2, (ScreenHeight - WindowHeight) / 2);
  SDL_SetWindowSize(SDLWindowPtr, WindowWidth, WindowHeight);

  OgreRenderWindowPtr->resize(Width, Height);
}

void Engine::CreateWindow() {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
    throw Exception("Failed to init SDL2");
  }

  for (int i = 0; i < SDL_NumJoysticks(); ++i) {
    if (SDL_IsGameController(i)) {
      SDL_GameControllerOpen(i);
    }
  }

  SDL_DisplayMode DM;
  SDL_GetDesktopDisplayMode(0, &DM);
  ScreenWidth = static_cast<int>(DM.w);
  ScreenHeight = static_cast<int>(DM.h);

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID

  if (WindowWidth == ScreenWidth && WindowHeight == ScreenHeight) {
    SDLWindowFlags |= SDL_WINDOW_BORDERLESS;
  }

  if (FullScreen) {
    SDLWindowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    SDLWindowFlags |= SDL_WINDOW_BORDERLESS;
    WindowWidth = ScreenWidth;
    WindowHeight = ScreenHeight;
  }

  SDLWindowPtr = SDL_CreateWindow(caption_.c_str(), SDL_WINDOWPOS_UNDEFINED_DISPLAY(0),
                                  SDL_WINDOWPOS_UNDEFINED_DISPLAY(0), WindowWidth, WindowHeight, SDLWindowFlags);

#else

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

  SDLWindowFlags |= SDL_WINDOW_BORDERLESS;
  SDLWindowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
  SDLWindowFlags |= SDL_WINDOW_OPENGL;

  WindowWidth = ScreenWidth;
  WindowHeight = ScreenHeight;

  SDLWindowPtr = SDL_CreateWindow(nullptr, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ScreenWidth, ScreenHeight,
                                  SDLWindowFlags);
  SDLGLContextPtr = SDL_GL_CreateContext(SDLWindowPtr);

#endif
}

void Engine::SetFullscreen() {
  FullScreen = true;

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  SDL_SetWindowFullscreen(SDLWindowPtr, SDLWindowFlags | SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN_DESKTOP);
#else
  SDL_SetWindowFullscreen(SDLWindowPtr, SDLWindowFlags | SDL_WINDOW_FULLSCREEN);
#endif
}

void Engine::SetWindowed() {
  FullScreen = false;

  SDL_SetWindowSize(SDLWindowPtr, WindowWidth, WindowHeight);
}

void Engine::SetWindowCaption(const char* Caption) { SDL_SetWindowTitle(SDLWindowPtr, Caption); }

void Engine::WindowRestoreFullscreenAndroid() {
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  SDL_DisplayMode DM;
  SDL_GetDesktopDisplayMode(0, &DM);
  int screen_w = static_cast<int>(DM.w);
  int screen_h = static_cast<int>(DM.h);
  OgreRenderWindowPtr->resize(screen_w, screen_h);
#endif
}

std::pair<int, int> Engine::GetSize() const { return make_pair(WindowWidth, WindowHeight); }

void Engine::Grab(bool grab) {
  // This breaks input on > Android 9.0
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  SDL_ShowCursor(!grab);
  SDL_SetWindowGrab(SDLWindowPtr, static_cast<SDL_bool>(grab));
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE
  // osx workaround: mouse motion events are gone otherwise
  SDL_SetRelativeMouseMode(static_cast<SDL_bool>(grab));
#endif
#endif
}

}  // namespace Glue
