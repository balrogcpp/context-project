// This source file is part of Glue Engine. Created by Andrey Vasiliev

#include "PCHeader.h"
#include "Engine.h"
#include "AssetHelpers.h"
#include "Components/DotSceneLoaderB.h"
#include "Config.h"
#include "Input/InputSequencer.h"
#include "Log.h"
#include "RTSS.h"
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
#include "Android.h"
#endif
#include "SDL2.hpp"
#include <OgrePlatformInformation.h>
#ifdef OGRE_BUILD_PLUGIN_OCTREE
#include <Plugins/OctreeSceneManager/OgreOctreeSceneManager.h>
#endif
#ifdef OGRE_BUILD_PLUGIN_PFX
#include <Plugins/ParticleFX/OgreParticleFXPlugin.h>
#endif
#ifdef OGRE_BUILD_PLUGIN_STBI
#include <Plugins/STBICodec/OgreSTBICodec.h>
#endif
#if defined(OGRE_BUILD_PLUGIN_FREEIMAGE) && !defined(OGRE_BUILD_PLUGIN_STBI)
#include <Plugins/FreeImageCodec/OgreFreeImageCodec.h>
#include <Plugins/FreeImageCodec/OgreFreeImageCodecExports.h>
#endif
#ifdef OGRE_BUILD_PLUGIN_ASSIMP
#include <Plugins/Assimp/OgreAssimpLoader.h>
#endif
#ifdef OGRE_BUILD_PLUGIN_DOT_SCENE
#include <Plugins/DotScene/OgreDotSceneLoader.h>
#endif

using namespace std;
using namespace Ogre;

namespace Glue {

Engine::Engine() {
  TestCPUCapabilities();
  ReadConfFile();
}

Engine::~Engine() { SDL_SetWindowFullscreen(SDLWindowPtr, SDL_FALSE); }

void Engine::TestCPUCapabilities() {
#if OGRE_CPU == OGRE_CPU_X86
  OgreAssert(Ogre::PlatformInformation::hasCpuFeature(Ogre::PlatformInformation::CPU_FEATURE_SSE), "SSE support required");
  OgreAssert(Ogre::PlatformInformation::hasCpuFeature(Ogre::PlatformInformation::CPU_FEATURE_SSE3), "SSE4 support required");
#elif OGRE_CPU == OGRE_CPU_ARM
  OgreAssert(Ogre::PlatformInformation::hasCpuFeature(Ogre::PlatformInformation::CPU_FEATURE_NEON), "ARM NEON support required");
#endif
}

void Engine::TestGPUCapabilities() {
  const auto *RSC = Ogre::Root::getSingleton().getRenderSystem()->getCapabilities();
  OgreAssert(RSC->hasCapability(RSC_HWRENDER_TO_TEXTURE), "Render to texture support required");
  OgreAssert(RSC->hasCapability(RSC_TEXTURE_FLOAT), "Float texture support required");
  OgreAssert(RSC->hasCapability(RSC_TEXTURE_COMPRESSION), "Texture compression support required");
#if defined(DESKTOP)
  OgreAssert(RSC->hasCapability(RSC_TEXTURE_COMPRESSION_DXT), "DXT compression support required");
#elif defined(ANDROID)
  OgreAssert(RSC->hasCapability(RSC_TEXTURE_COMPRESSION_ETC1), "ETC1 compression support required");
#elif defined(IOS)
  OgreAssert(RSC->hasCapability(RSC_TEXTURE_COMPRESSION_PVRTC), "PVRTC compression support required");
#endif
  if (RenderSystemIsGL3()) {
    OgreAssert(CheckRenderSystemVersion(3, 3), "OpenGL 3.3 is not supported");
  } else if (RenderSystemIsGLES2()) {
    OgreAssert(CheckRenderSystemVersion(3, 0), "OpenGL 3.0 is not supported");
  }
}

void Engine::InitComponents() {
  WindowWidth = ConfigPtr->GetInt("window_width", WindowWidth);
  WindowHeight = ConfigPtr->GetInt("window_high", WindowHeight);
  WindowFullScreen = ConfigPtr->GetBool("window_fullscreen", WindowFullScreen);
  RenderSystemName = ConfigPtr->Get("render_system", RenderSystemName);
  OgreRoot = new Root("", "", "");
  InitRenderSystem();
  InitSDLSubsystems();
  CreateSDLWindow();
  InitOgrePlugins();
  OgreRoot->initialise(false);
  CreateOgreRenderWindow();
  TestGPUCapabilities();
#ifdef ANDROID
  AndroidRestoreWindow();
#endif
  InitResourceLocation();
  InitRTSS();
  CreateRTSSRuntime();
  // InitRTSSInstansing();
  InitTextureSettings();
  InitOverlay();
  InitResources();
  InitShadowSettings();

  InitPhysics();
  InitSound();
  InitScene();
  InitCompositor();
  for (auto it : ComponentList) it->OnSetUp();
}

void Engine::InitSDLSubsystems() {
  OgreAssert(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER), "Failed to init SDL2");
  for (int i = 0; i < SDL_NumJoysticks(); ++i)
    if (SDL_IsGameController(i)) SDL_GameControllerOpen(i);
  SDL_DisplayMode DM;
  SDL_GetDesktopDisplayMode(0, &DM);
  ScreenWidth = static_cast<int>(DM.w);
  ScreenHeight = static_cast<int>(DM.h);
}

bool Engine::CheckRenderSystemVersion(int major, int minor) {
#ifdef OGRE_BUILD_RENDERSYSTEM_GL3PLUS
  if (RenderSystemIsGL3()) return CheckGL3Version(major, minor);
#endif
#ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
  if (RenderSystemIsGLES2()) return CheckGLES2Version(major, minor);
#endif
#ifdef OGRE_BUILD_RENDERSYSTEM_GL
  if (RenderSystemIsGL()) return CheckGLVersion(major, minor);
#endif
  return false;
}

void Engine::InitRenderSystem() {
#ifdef OGRE_STATIC_LIB
#ifdef DESKTOP
#if defined(OGRE_BUILD_RENDERSYSTEM_GL3PLUS)
  InitOgreRenderSystemGL3();
#elif defined(OGRE_BUILD_RENDERSYSTEM_GL)
  InitOgreRenderSystemGL();
#elif defined(OGRE_BUILD_RENDERSYSTEM_GLES2)
  InitOgreRenderSystemGLES2();
#endif // DESKTOP
#else // ! DESKTOP
  InitOgreRenderSystemGLES2();
#endif // DESKTOP
#endif  // OGRE_STATIC_LIB
}

void Engine::InitOgrePlugins() {
#ifdef OGRE_STATIC_LIB
#ifdef OGRE_BUILD_PLUGIN_OCTREE
  Root::getSingleton().addSceneManagerFactory(new OctreeSceneManagerFactory());
#endif
#ifdef OGRE_BUILD_PLUGIN_PFX
  Root::getSingleton().installPlugin(new ParticleFXPlugin());
#endif
#ifdef OGRE_BUILD_PLUGIN_STBI
  Root::getSingleton().installPlugin(new STBIPlugin());
#endif
#if defined(DEBUG) && defined(OGRE_BUILD_PLUGIN_FREEIMAGE) && !defined(OGRE_BUILD_PLUGIN_STBI) && defined(DESKTOP)
  Root::getSingleton().installPlugin(new FreeImagePlugin());
#endif
#if defined(DEBUG) && defined(OGRE_BUILD_PLUGIN_ASSIMP) && defined(DESKTOP)
  Root::getSingleton().installPlugin(new AssimpPlugin());
#endif
#ifdef OGRE_BUILD_PLUGIN_OCTREE
  OgreSceneManager = OgreRoot->createSceneManager("OctreeSceneManager", "Default");
#else
  OgreSceneManager = OgreRoot->createSceneManager(ST_GENERIC, "Default");
#endif
#ifdef OGRE_BUILD_PLUGIN_DOT_SCENE
  Root::getSingleton().installPlugin(new DotScenePluginB());
#else
  Root::getSingleton().installPlugin(new DotScenePluginB());
#endif
#endif  // OGRE_STATIC_LIB
}

void Engine::CreateSDLWindow() {
#if defined(DESKTOP)
  if (WindowWidth == ScreenWidth && WindowHeight == ScreenHeight) {
    SDLWindowFlags |= SDL_WINDOW_BORDERLESS;
  }
  if (WindowFullScreen) {
    SDLWindowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    SDLWindowFlags |= SDL_WINDOW_BORDERLESS;
    WindowWidth = ScreenWidth;
    WindowHeight = ScreenHeight;
  }
  WindowPositionFlag = SDL_WINDOWPOS_UNDEFINED_DISPLAY(0);
  SDLWindowPtr = SDL_CreateWindow(WindowCaption.c_str(), WindowPositionFlag, WindowPositionFlag, WindowWidth, WindowHeight, SDLWindowFlags);
#elif defined(ANDROID)
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDLWindowFlags |= SDL_WINDOW_BORDERLESS;
  SDLWindowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
  SDLWindowFlags |= SDL_WINDOW_OPENGL;
  WindowWidth = ScreenWidth;
  WindowHeight = ScreenHeight;
  WindowPositionFlag = SDL_WINDOWPOS_UNDEFINED;
  SDLWindowPtr = SDL_CreateWindow(nullptr, WindowPositionFlag, WindowPositionFlag, ScreenWidth, ScreenHeight, SDLWindowFlags);
  SDLGLContextPtr = SDL_GL_CreateContext(SDLWindowPtr);
#endif
}

void Engine::CreateOgreRenderWindow() {
  NameValuePairList OgreRenderParams;
  SDL_SysWMinfo info;
  SDL_VERSION(&info.version);
  SDL_GetWindowWMInfo(SDLWindowPtr, &info);
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  OgreRenderParams["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(info.info.win.window));
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
  OgreRenderParams["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(info.info.x11.window));
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
  OgreRenderParams["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(info.info.cocoa.window));
#elif OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  OgreRenderParams["currentGLContext"] = "true";
  OgreRenderParams["externalGLControl"] = "true";
  OgreRenderParams["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(info.info.android.window));
  OgreRenderParams["preserveContext"] = "true";
  JNIEnv *env = (JNIEnv *)SDL_AndroidGetJNIEnv();
  jclass class_activity = env->FindClass("android/app/Activity");
  jclass class_resources = env->FindClass("android/content/res/Resources");
  jmethodID method_get_resources = env->GetMethodID(class_activity, "getResources", "()Landroid/content/res/Resources;");
  jmethodID method_get_assets = env->GetMethodID(class_resources, "getAssets", "()Landroid/content/res/AssetManager;");
  jobject raw_activity = (jobject)SDL_AndroidGetActivity();
  jobject raw_resources = env->CallObjectMethod(raw_activity, method_get_resources);
  jobject raw_asset_manager = env->CallObjectMethod(raw_resources, method_get_assets);
  AAssetManager *asset_manager = AAssetManager_fromJava(env, raw_asset_manager);
  AConfiguration *mAConfig = AConfiguration_new();
  AConfiguration_fromAssetManager(mAConfig, asset_manager);
  ArchiveManager::getSingleton().addArchiveFactory(new APKFileSystemArchiveFactory(asset_manager));
  ArchiveManager::getSingleton().addArchiveFactory(new APKZipArchiveFactory(asset_manager));
#endif
  const char TrueStr[] = "true";
  const char FalseStr[] = "false";
  WindowVsync = ConfigPtr->GetBool("vsync", WindowVsync);
  WindowGammaCorrection = ConfigPtr->GetBool("gamma", WindowGammaCorrection);
  OgreRenderParams["vsync"] = WindowVsync ? TrueStr : FalseStr;
  OgreRenderParams["gamma"] = WindowGammaCorrection ? TrueStr : FalseStr;
  OgreRenderParams["FSAA"] = to_string(WindowFSAA);
  OgreRenderWindowPtr = OgreRoot->createRenderWindow(WindowCaption, WindowWidth, WindowHeight, WindowFullScreen, &OgreRenderParams);
  OgreRenderTarget = OgreRoot->getRenderTarget(OgreRenderWindowPtr->getName());
  OgreCamera = OgreSceneManager->createCamera("Default");
  OgreViewport = OgreRenderTarget->addViewport(OgreCamera);
  OgreViewport->setBackgroundColour(ColourValue::Black);
  OgreCamera->setAspectRatio(static_cast<float>(OgreViewport->getActualWidth()) / static_cast<float>(OgreViewport->getActualHeight()));
  OgreCamera->setAutoAspectRatio(true);
}

void Engine::InitShadowSettings() {
  bool ShadowsEnabled = true;
#ifdef MOBILE
  ShadowsEnabled = false;
#endif
  float ShadowFarDistance = 400;
  int16_t ShadowTexSize = 512;
  ShadowsEnabled = ConfigPtr->GetBool("shadows_enable", ShadowsEnabled);
  ShadowFarDistance = ConfigPtr->GetInt("shadow_far", ShadowFarDistance);
  ShadowTexSize = ConfigPtr->GetInt("tex_size", ShadowTexSize);
  if (!ShadowsEnabled) {
    OgreSceneManager->setShadowTechnique(SHADOWTYPE_NONE);
    OgreSceneManager->setShadowTextureCountPerLightType(Light::LT_DIRECTIONAL, 0);
    OgreSceneManager->setShadowTextureCountPerLightType(Light::LT_SPOTLIGHT, 0);
    OgreSceneManager->setShadowTextureCountPerLightType(Light::LT_POINT, 0);
    return;
  }
#if defined(DESKTOP)
  PixelFormat ShadowTextureFormat = PixelFormat::PF_DEPTH16;
#elif defined(MOBILE)
  PixelFormat ShadowTextureFormat = PixelFormat::PF_FLOAT16_R;
#endif
  OgreSceneManager->setShadowTechnique(SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
  OgreSceneManager->setShadowFarDistance(ShadowFarDistance);
  OgreSceneManager->setShadowTextureSize(ShadowTexSize);
  OgreSceneManager->setShadowTexturePixelFormat(ShadowTextureFormat);
  OgreSceneManager->setShadowTextureCountPerLightType(Light::LT_DIRECTIONAL, 3);
  OgreSceneManager->setShadowTextureCountPerLightType(Light::LT_SPOTLIGHT, 1);
  OgreSceneManager->setShadowTextureCountPerLightType(Light::LT_POINT, 0);
  OgreSceneManager->setShadowTextureSelfShadow(true);
  OgreSceneManager->setShadowCasterRenderBackFaces(true);
  OgreSceneManager->setShadowFarDistance(ShadowFarDistance);
  auto passCaterMaterial = MaterialManager::getSingleton().getByName("PSSM/shadow_caster");
  OgreSceneManager->setShadowTextureCasterMaterial(passCaterMaterial);
  PSSMSetupPtr = make_shared<PSSMShadowCameraSetup>();
  PSSMSetupPtr->calculateSplitPoints(PSSMSplitCount, 0.001, OgreSceneManager->getShadowFarDistance());
  PSSMSplitPointList = PSSMSetupPtr->getSplitPoints();
  PSSMSetupPtr->setSplitPadding(0.0);
  for (int i = 0; i < PSSMSplitCount; i++) PSSMSetupPtr->setOptimalAdjustFactor(i, static_cast<float>(0.5 * i));
  OgreSceneManager->setShadowCameraSetup(PSSMSetupPtr);
  OgreSceneManager->setShadowColour(ColourValue::Black);
#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
  InitRTSSPSSM(PSSMSplitPointList);
#endif
}

void Engine::InitTextureSettings() {
  const auto *RSC = Ogre::Root::getSingleton().getRenderSystem()->getCapabilities();
  auto &OTM = Ogre::TextureManager::getSingleton();
  auto &OMM = Ogre::MaterialManager::getSingleton();
  TextureFilterOptions TextureFiltering = TFO_BILINEAR;
  if (RSC->hasCapability(RSC_ANISOTROPY) && ConfigPtr->GetString("filtration").compare("anisotropic")) {
    TextureFiltering = TFO_ANISOTROPIC;
    OMM.setDefaultAnisotropy(ConfigPtr->GetInt("anisotropy", 1));
  }
  OMM.setDefaultTextureFiltering(TextureFiltering);
  OTM.setDefaultNumMipmaps(MIP_UNLIMITED);
}

void Engine::Capture() {
  static auto &io = InputSequencer::GetInstance();
  io.Capture();
}

void Engine::ReadConfFile() { ConfigPtr = Config::GetInstancePtr(); }

void Engine::InitSound() {
  SoundPtr = make_unique<Sound>();
  RegComponent(SoundPtr.get());
}

void Engine::InitResources() {
  auto &RGM = ResourceGroupManager::getSingleton();
  RGM.initialiseResourceGroup(RGN_INTERNAL);
  RGM.initialiseAllResourceGroups();
}

void Engine::InitOverlay() {
  OverlayPtr = make_unique<Overlay>(OgreRenderWindowPtr);
  RegComponent(OverlayPtr.get());
}

void Engine::InitCompositor() {
  CompositorUPtr = make_unique<Compositor>();
  RegComponent(CompositorUPtr.get());
}

void Engine::InitPhysics() {
  PhysicsPtr = make_unique<Physics>();
  RegComponent(PhysicsPtr.get());
}

void Engine::InitScene() {
  ScenePtr = make_unique<Scene>();
  RegComponent(ScenePtr.get());
}

void Engine::RegComponent(ComponentI *ComponentPtr) {
  if (ComponentPtr) ComponentList.push_back(ComponentPtr);
}

void Engine::UnRegComponent(ComponentI *ComponentPtr) {
  auto Iter = find(ComponentList.begin(), ComponentList.end(), ComponentPtr);
  if (Iter != ComponentList.end()) ComponentList.erase(Iter);
}

void Engine::OnPause() {
  for (auto &it : ComponentList) it->OnPause();
}

void Engine::OnResume() {
  for (auto &it : ComponentList) it->OnResume();
}

void Engine::OnMenuOn() {
  PhysicsPtr->OnPause();
  ScenePtr->OnPause();
}

void Engine::OnMenuOff() {
  PhysicsPtr->OnResume();
  ScenePtr->OnResume();
}

void Engine::OnCleanup() {
  for (auto &it : ComponentList) it->OnClean();
  InitShadowSettings();
}

void Engine::Update(float PassedTime) {
  for (auto &it : ComponentList) it->OnUpdate(PassedTime);
}

void Engine::RenderFrame() {
  OgreRoot->renderOneFrame();
#if defined(WINDOWS) || defined(ANDROID)
  SDL_GL_SwapWindow(SDLWindowPtr);
#endif
}

void Engine::ResizeWindow(int Width, int Height) {
  WindowWidth = Width;
  WindowHeight = Height;
  SDL_SetWindowPosition(SDLWindowPtr, (ScreenWidth - WindowWidth) / 2, (ScreenHeight - WindowHeight) / 2);
  SDL_SetWindowSize(SDLWindowPtr, WindowWidth, WindowHeight);
  OgreRenderWindowPtr->resize(Width, Height);
}

void Engine::SetFullscreen(bool Fullscreen) {
  if (Fullscreen) {
    WindowFullScreen = true;
    OgreRenderWindowPtr->setFullscreen(WindowFullScreen, WindowWidth, WindowHeight);
#ifdef DESKTOP
    SDL_SetWindowFullscreen(SDLWindowPtr, SDLWindowFlags | SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN_DESKTOP);
#else
    SDL_SetWindowFullscreen(SDLWindowPtr, SDLWindowFlags | SDL_WINDOW_FULLSCREEN);
#endif
  } else {
    WindowFullScreen = false;
    OgreRenderWindowPtr->setFullscreen(WindowFullScreen, WindowWidth, WindowHeight);
    SDL_SetWindowFullscreen(SDLWindowPtr, SDLWindowFlags);
    SDL_SetWindowSize(SDLWindowPtr, WindowWidth, WindowHeight);
  }
}

bool Engine::IsFullscreen() { return WindowFullScreen; }

void Engine::SetWindowCaption(const char *Caption) { SDL_SetWindowTitle(SDLWindowPtr, Caption); }

#ifdef MOBILE
void Engine::AndroidRestoreWindow() {
  SDL_DisplayMode DM;
  SDL_GetDesktopDisplayMode(0, &DM);
  int screen_w = static_cast<int>(DM.w);
  int screen_h = static_cast<int>(DM.h);
  OgreRenderWindowPtr->resize(screen_w, screen_h);
}
#endif

void Engine::GrabMouse(bool grab) {
#ifndef MOBILE  // This breaks input @Android >9.0
  if (SDLWindowPtr) {
    SDL_ShowCursor(!grab);
    SDL_SetWindowGrab(SDLWindowPtr, static_cast<SDL_bool>(grab));
    SDL_SetRelativeMouseMode(static_cast<SDL_bool>(grab));
  }
#endif
}

void Engine::InitResourceLocation() {
#if defined(DESKTOP)
#ifndef WINDOWS
  const char SEPARATOR = '/';
#else
  const char SEPARATOR = '\\';
#endif
  const string ProgramsDir = "Programs";
  const string AssetsDir = "Assets";
  AddLocation(ProgramsDir + SEPARATOR + "Core", RGN_INTERNAL);
  if (RenderSystemIsGLES2())
    AddLocation(ProgramsDir + SEPARATOR + "GLSLES", RGN_INTERNAL);
  else
    AddLocation(ProgramsDir + SEPARATOR + "GLSL", RGN_INTERNAL);
  AddLocation(ProgramsDir + SEPARATOR + "Other", RGN_INTERNAL);
  AddLocation(AssetsDir, RGN_DEFAULT);
#elif defined(ANDROID)
  auto &RGM = ResourceGroupManager::getSingleton();
  RGM.addResourceLocation("/Programs/Core.zip", "APKZip", RGN_INTERNAL);
  RGM.addResourceLocation("/Programs/GLSLES.zip", "APKZip", RGN_INTERNAL);
  RGM.addResourceLocation("/Programs/Other.zip", "APKZip", RGN_INTERNAL);
  RGM.addResourceLocation("/Assets.zip", "APKZip", RGN_DEFAULT);
#endif
}

Engine &GetEngine() {
  static auto &EnginePtr = Engine::GetInstance();
  return EnginePtr;
}

Physics &GetPhysics() { return GetComponent<Physics>(); }

Sound &GetAudio() { return GetComponent<Sound>(); }

Scene &GetScene() { return GetComponent<Scene>(); }

Overlay &GetOverlay() { return GetComponent<Overlay>(); }

}  // namespace Glue
