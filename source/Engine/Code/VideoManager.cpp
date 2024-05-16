/// created by Andrey Vasiliev

#include "pch.h"
#include "VideoManager.h"
#include "CompositorManager.h"
#include "DotSceneLoaderB/DotSceneLoaderB.h"
#include "Platform.h"
#include "imgui_impl_sdl2.h"
#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
#include <RTShaderSystem/OgreRTShaderSystem.h>
#endif
#ifdef OGRE_BUILD_PLUGIN_OCTREE
#include <Plugins/OctreeSceneManager/OgreOctreeSceneManager.h>
#endif
#ifdef OGRE_BUILD_PLUGIN_STBI
#include <Plugins/STBICodec/OgreSTBICodec.h>
#endif
#ifdef OGRE_BUILD_PLUGIN_FREEIMAGE
#include <Plugins/FreeImageCodec/OgreFreeImageCodec.h>
#endif
#ifdef OGRE_BUILD_PLUGIN_ASSIMP
#include <Plugins/Assimp/OgreAssimpLoader.h>
#endif
#ifdef OGRE_BUILD_PLUGIN_DOT_SCENE
#include <Plugins/DotScene/OgreDotSceneLoader.h>
#endif
#ifdef OGRE_BUILD_PLUGIN_PFX
#include <Plugins/ParticleFX/OgreParticleFXPlugin.h>
#endif
#ifdef OGRE_BUILD_COMPONENT_TERRAIN
#include <Terrain/OgreTerrainGroup.h>
#ifdef OGRE_BUILD_COMPONENT_PAGING
#include <Terrain/OgreTerrainPaging.h>
#endif
#endif
#ifdef OGRE_BUILD_COMPONENT_PAGING
#include <Paging/OgrePaging.h>
#endif
#ifdef OGRE_BUILD_COMPONENT_OVERLAY
#include <Overlay/OgreFontManager.h>
#include <Overlay/OgreImGuiOverlay.h>
#include <Overlay/OgreOverlayManager.h>
#include <Overlay/OgreOverlaySystem.h>
#endif
#if defined(OGRE_BUILD_RENDERSYSTEM_GL) || defined(OGRE_BUILD_RENDERSYSTEM_GL3PLUS) || defined(OGRE_BUILD_RENDERSYSTEM_GLES2)
#include <OgreGLRenderSystemCommon.h>
#endif
#include <Ogre.h>
#include <SDL2/SDL.h>
#include <chrono>
#include <iomanip>
#include <iostream>
#ifdef DESKTOP
#if __has_include(<filesystem>) && ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) \
    || (defined(__cplusplus) && __cplusplus >= 201703L && !defined(__APPLE__)) \
    || (!defined(__MAC_OS_X_VERSION_MIN_REQUIRED) || __MAC_OS_X_VERSION_MIN_REQUIRED >= 101500))
#include <filesystem>
namespace fs = std::filesystem;
#else
#include "ghc/filesystem.hpp"
namespace fs = ghc::filesystem;
#endif  // <filesystem>
#endif  // DESKTOP
#ifdef APPLE
#include <mach-o/dyld.h>
#endif
#ifdef LINUX
#include <unistd.h>
#endif
#ifdef WINDOWS
#include <windows.h>
#endif
#ifdef __ANDROID__
#include <OgreArchiveFactory.h>
#include <OgreFileSystem.h>
#include <OgreZip.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/configuration.h>
#include <android/input.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/sensor.h>
#include <jni.h>
#endif

using namespace std;

namespace {

#ifdef DESKTOP
// based on tensorflow GetBinaryDir
// https://github.com/tensorflow/tensorflow/blob/e895d5ca395c2362df4f5c8f08b68501b41f8a98/tensorflow/stream_executor/cuda/cuda_gpu_executor.cc#L202
std::string GetBinaryDir() {
#ifndef MAX_PATH
#define MAX_PATH 1024
#endif
  char buffer[MAX_PATH] = {0};
#ifdef APPLE
  uint32_t bufferSize = 0;
  _NSGetExecutablePath(nullptr, &bufferSize);
  char unresolvedPath[bufferSize];
  _NSGetExecutablePath(unresolvedPath, &bufferSize);
  realpath(unresolvedPath, buffer);
#elif defined(WINDOWS)
  GetModuleFileNameA(NULL, buffer, MAX_PATH);
#elif defined(LINUX)
  readlink("/proc/self/exe", buffer, MAX_PATH);
#endif
  buffer[MAX_PATH - 1] = 0;
  auto pos = std::string(buffer).find_last_of("\\/");
  return std::string(buffer).substr(0, pos + 1);
}

std::string FindPath(const std::string &path, int depth = 0) {
  std::string result = path;
  std::string buffer = GetBinaryDir();

  if (fs::exists(buffer.append(path))) {
    return buffer;
  }

  for (int i = 0; i < depth; i++) {
    if (fs::exists(result))
      return result;
    else
      result = std::string("../").append(result);
  }

  return "";
}

void ScanLocation(const string &path, const string &groupName) {
  std::vector<std::string> resourceList;

  if (!fs::is_directory(path)) {
    LogError("[ScanLocation]", "path is not directory");
    return;
  }
  auto &ogreResourceManager = Ogre::ResourceGroupManager::getSingleton();
  ogreResourceManager.addResourceLocation(path, "FileSystem", groupName);

  for (fs::recursive_directory_iterator end, it(path); it != end; ++it) {
    const std::string fullPath = it->path().string();
    const std::string fileExtention = it->path().filename().extension().string();

    if (it->is_directory())
      ogreResourceManager.addResourceLocation(fullPath, "FileSystem", groupName);
    else if (it->is_regular_file() && (fileExtention == ".bin" || fileExtention == ".zip"))
      ogreResourceManager.addResourceLocation(fullPath, "Zip", groupName);
  }
}
#endif  // DESKTOP
}  // namespace

namespace gge {

#ifdef OGRE_BUILD_RENDERSYSTEM_GL3PLUS
void InitOgreRenderSystemGL3();
#endif
#ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
void InitOgreRenderSystemGLES2();
#endif
#ifdef OGRE_BUILD_RENDERSYSTEM_GL
void InitOgreRenderSystemGL();
#endif
#ifdef OGRE_BUILD_RENDERSYSTEM_D3D11
void InitOgreRenderSystemD3D11();
#endif
#ifdef OGRE_BUILD_RENDERSYSTEM_D3D9
void InitOgreRenderSystemD3D9();
#endif
void InitEmbeddedResources();

void VideoManager::LoadResources() {
#ifdef ANDROID
  JNIEnv *env = (JNIEnv *)SDL_AndroidGetJNIEnv();
  jclass classActivity = env->FindClass("android/app/Activity");
  jclass classResources = env->FindClass("android/content/res/Resources");
  jmethodID methodGetResources = env->GetMethodID(classActivity, "getResources", "()Landroid/content/res/Resources;");
  jmethodID methodGetAssets = env->GetMethodID(classResources, "getAssets", "()Landroid/content/res/AssetManager;");
  jobject rawActivity = (jobject)SDL_AndroidGetActivity();
  jobject rawResources = env->CallObjectMethod(rawActivity, methodGetResources);
  jobject rawAssetManager = env->CallObjectMethod(rawResources, methodGetAssets);
  AAssetManager *assetManager = AAssetManager_fromJava(env, rawAssetManager);
  AConfiguration *aConfig = AConfiguration_new();
  AConfiguration_fromAssetManager(aConfig, assetManager);
  auto &archiveManager = Ogre::ArchiveManager::getSingleton();
  archiveManager.addArchiveFactory(new Ogre::APKFileSystemArchiveFactory(assetManager));
  archiveManager.addArchiveFactory(new Ogre::APKZipArchiveFactory(assetManager));
#endif

  if (!RenderSystemIsGLES2()) {
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(Ogre::MIP_UNLIMITED);
    Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
    Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(1);
  } else {
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(0);
    Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_BILINEAR);
  }

  auto &ogreResourceManager = Ogre::ResourceGroupManager::getSingleton();
  const char *FILE_SYSTEM = "FileSystem";
  const char *ZIP = "Zip";
  const char *APKZIP = "APKZip";
  const char *EMZIP = "EmbeddedZip";
  const char *PROGRAMS_ZIP = "programs.bin";
  const char *ASSETS_ZIP = "assets.bin";
  const int SCAN_DEPTH = 4;
  const char *PROGRAMS_DIR = "source/Programs";
  const char *ASSETS_DIR = "source/Tests/Assets";

#if defined(DESKTOP)
  if (!FindPath("programs.bin").empty()) {
    ogreResourceManager.addResourceLocation(FindPath("programs.bin"), ZIP, Ogre::RGN_INTERNAL);
  } else if (!FindPath("programs.zip").empty()) {
    ogreResourceManager.addResourceLocation(FindPath("programs.zip"), ZIP, Ogre::RGN_INTERNAL);
  } else if (!FindPath("programs").empty()) {
    ScanLocation(FindPath("programs"), Ogre::RGN_INTERNAL);
  } else if (!FindPath(PROGRAMS_DIR, SCAN_DEPTH).empty()) {
    ScanLocation(FindPath(PROGRAMS_DIR, SCAN_DEPTH), Ogre::RGN_INTERNAL);
  } else {
    InitEmbeddedResources();
  }

  if (!FindPath("assets.bin").empty()) {
    ogreResourceManager.addResourceLocation(FindPath("assets.bin"), ZIP, Ogre::RGN_DEFAULT);
  } else if (!FindPath("assets.zip").empty()) {
    ogreResourceManager.addResourceLocation(FindPath("assets.zip"), ZIP, Ogre::RGN_DEFAULT);
  } else if (!FindPath("assets").empty()) {
    ScanLocation(FindPath("assets"), Ogre::RGN_DEFAULT);
  } else if (!FindPath(ASSETS_DIR, SCAN_DEPTH).empty()) {
    ScanLocation(FindPath(ASSETS_DIR, SCAN_DEPTH), Ogre::RGN_DEFAULT);
  }

#elif defined(ANDROID)
#ifdef NDEBUG
  InitEmbeddedResources();
#else
  ogreResourceManager.addResourceLocation(PROGRAMS_ZIP, APKZIP, Ogre::RGN_INTERNAL);
#endif
  ogreResourceManager.addResourceLocation(ASSETS_ZIP, APKZIP, Ogre::RGN_DEFAULT);
#elif defined(IOS) || defined(EMSCRIPTEN)
#ifdef NDEBUG
  InitEmbeddedResources();
#else
  ogreResourceManager.addResourceLocation(PROGRAMS_ZIP, ZIP, Ogre::RGN_INTERNAL);
#endif
  ogreResourceManager.addResourceLocation(ASSETS_ZIP, ZIP, Ogre::RGN_DEFAULT);
#endif

  ogreResourceManager.initialiseResourceGroup(Ogre::RGN_INTERNAL);
  ogreResourceManager.initialiseAllResourceGroups();
}

VideoManager::VideoManager()
    : ogreMinLogLevel(Ogre::LML_NORMAL),
      ogreLogFile("Ogre.log"),
      shadowEnabled(true),
      shadowTexCount(8),
      pssmSplitCount(2),
      pssmSplitPadding(1.0),
      shadowNearDistance(1.0),
      shadowFarDistance(30.0),
      shadowTexSize(1024) {
#if OGRE_CPU == OGRE_CPU_X86 && OGRE_ARCH_TYPE == OGRE_ARCHITECTURE_32
  OgreAssert(Ogre::PlatformInformation::hasCpuFeature(Ogre::PlatformInformation::CPU_FEATURE_SSE2), "SSE2 support required");
#endif
#if OGRE_CPU == OGRE_CPU_X86
  // OgreAssert(can_use_intel_core_4th_gen_features(), "AVX2 support required");
#endif
}

VideoManager::~VideoManager() {
  if (imguiOverlay) {
    ImGui_ImplSDL2_Shutdown();
  }

  SDL_Quit();
}

void VideoManager::OnUpdate(float time) {
  if (imguiOverlay) {
    ImGui_ImplSDL2_NewFrame();
  }
}

void VideoManager::OnEvent(const SDL_Event &event) {
  if (imguiOverlay) {
    ImGui_ImplSDL2_ProcessEvent(&event);
  }
}

void VideoManager::OnClean() {
  InputSequencer::GetInstance().UnregWindowListener(this);
  sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
  sceneManager->clearScene();
  Ogre::ResourceGroupManager::getSingleton().unloadResourceGroup(Ogre::RGN_DEFAULT);
}

Window &VideoManager::GetWindow(int number) { return windowList[0]; }

Window &VideoManager::GetMainWindow() { return *mainWindow; }

void VideoManager::ShowWindow(bool show, int index) { windowList[index].Show(show); }

void VideoManager::CheckGPU() {
  const auto *ogreRenderSystem = ogreRoot->getRenderSystem();
  const auto *ogreRenderCapabilities = ogreRenderSystem->getCapabilities();
  const auto *ogreRenderSystemCommon = dynamic_cast<const Ogre::GLRenderSystemCommon *>(ogreRenderSystem);
  ASSERTION(ogreRenderCapabilities->hasCapability(Ogre::RSC_VERTEX_PROGRAM), "Vertex shader support required");
  ASSERTION(ogreRenderCapabilities->hasCapability(Ogre::RSC_FRAGMENT_PROGRAM), "Fragment shader support required");
  ASSERTION(ogreRenderCapabilities->hasCapability(Ogre::RSC_TEXTURE_3D), "3D texture support required");
  ASSERTION(ogreRenderCapabilities->hasCapability(Ogre::RSC_HWRENDER_TO_TEXTURE), "Render to texture support required");
  ASSERTION(ogreRenderCapabilities->hasCapability(Ogre::RSC_TEXTURE_FLOAT), "Float texture support required");
  ASSERTION(ogreRenderCapabilities->hasCapability(Ogre::RSC_TEXTURE_COMPRESSION), "Texture compression support required");
  ASSERTION(ogreRenderCapabilities->hasCapability(Ogre::RSC_VERTEX_BUFFER_INSTANCE_DATA), "Instancing support required");
#if defined(DESKTOP)
  if (RenderSystemIsGL3()) {
    ASSERTION(ogreRenderCapabilities->hasCapability(Ogre::RSC_AUTOMIPMAP_COMPRESSED), "DXT compression support required");
    ASSERTION(ogreRenderCapabilities->hasCapability(Ogre::RSC_TEXTURE_COMPRESSION_DXT), "DXT compression support required");
    ASSERTION(ogreRenderCapabilities->hasCapability(Ogre::RSC_ANISOTROPY), "DXT compression support required");
  }
#elif defined(ANDROID)
  if (RenderSystemIsGLES2()) {
    ASSERTION(ogreRenderCapabilities->hasCapability(Ogre::RSC_TEXTURE_COMPRESSION_ETC1), "ETC1 compression support required");
  }
#elif defined(IOS)
  if (RenderSystemIsGLES2()) {
    ASSERTION(ogreRenderCapabilities->hasCapabilityOgre::(RSC_TEXTURE_COMPRESSION_PVRTC), "PVRTC compression support required");
  }
#endif
  if (RenderSystemIsGL3()) {
#ifdef OGRE_BUILD_RENDERSYSTEM_GL3PLUS
    ASSERTION(ogreRenderSystemCommon->hasMinGLVersion(3, 3), "OpenGL 3.3 is not supported");
#endif
  } else if (RenderSystemIsGLES2()) {
#ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
    ASSERTION(ogreRenderSystemCommon->hasMinGLVersion(3, 0), "OpenGLES 3.0 is not supported");
#endif
  }
}

void VideoManager::InitSDL() {
#ifdef _MSC_VER
  SDL_SetMainReady();
#endif

  int result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);
  if (!result) LogError("SDL_Init failed", SDL_GetError());
  ASSERTION(!result, "Failed to init SDL");

#ifdef DESKTOP
  string path = FindPath("gamecontrollerdb.txt", 1);
  if (!path.empty()) {
    result = SDL_GameControllerAddMappingsFromFile(path.c_str());
    if (result == -1) LogError("gamecontrollerdb.txt not found", SDL_GetError());
    ASSERTION(result != -1, "gamecontrollerdb.txt not found");
  }
#endif
}

class MutedLogListener final : public Ogre::LogListener {
 public:
  void messageLogged(const Ogre::String &message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String &logName,
                     bool &skipThisMessage) override {}
};

#ifdef DESKTOP
class DefaultLogListener final : public Ogre::LogListener {
 public:
  void messageLogged(const Ogre::String &message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String &logName,
                     bool &skipThisMessage) override {
    static std::ofstream of(GetBinaryDir() + "/" + logName);
    static bool isOpen = of.is_open();

    if (isOpen) {
      std::stringstream ss;
      ss << "[" << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count() << "] ";
      ss << message << '\n';
      of << ss.str();
    }

#ifdef _DEBUG
    printf("%s\n", message.c_str());
#endif
  }
};
#endif

void VideoManager::InitOgreRoot() {
  ogreLogFile = "runtime.log";

#if defined(NDEBUG) && defined(ANDROID)
  ogreMinLogLevel = Ogre::LML_CRITICAL;
#else
  ogreMinLogLevel = Ogre::LML_TRIVIAL;
#endif

#ifdef DESKTOP
  ogreLogManager = std::make_unique<Ogre::LogManager>();
  ogreLogManager->createLog(ogreLogFile, false, false, true);
  ogreLogManager->getDefaultLog()->addListener(new DefaultLogListener());

  auto tc = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  std::stringstream ss;
  ss << std::put_time(std::localtime(&tc), "%F %T %Z");
  ogreLogManager->getDefaultLog()->logMessage(ss.str(), Ogre::LML_NORMAL);
#endif

  ogreRoot = new Ogre::Root("", "", "");

  Ogre::LogManager::getSingleton().setMinLogLevel(static_cast<Ogre::LogMessageLevel>(ogreMinLogLevel));

#ifdef DESKTOP
#if defined(OGRE_BUILD_RENDERSYSTEM_GL3PLUS)
  InitOgreRenderSystemGL3();
#elif defined(OGRE_BUILD_RENDERSYSTEM_GLES2)
  InitOgreRenderSystemGLES2();
#elif defined(OGRE_BUILD_RENDERSYSTEM_GL)
  InitOgreRenderSystemGL();
#elif defined(OGRE_BUILD_RENDERSYSTEM_D3D11)
  InitOgreRenderSystemD3D11();
#elif defined(OGRE_BUILD_RENDERSYSTEM_D3D9)
  InitOgreRenderSystemD3D();
#endif  // DESKTOP
#else   // !DESKTOP
  InitOgreRenderSystemGLES2();
#endif  // DESKTOP

#ifdef OGRE_BUILD_PLUGIN_OCTREE
  Ogre::Root::getSingleton().addSceneManagerFactory(new Ogre::OctreeSceneManagerFactory());
  sceneManager = ogreRoot->createSceneManager("OctreeSceneManager", "Default");
#else
  sceneManager = OgreRoot->createSceneManager(Ogre::SMT_DEFAULT, "Default");
#endif
#ifdef OGRE_BUILD_PLUGIN_PFX
  Ogre::Root::getSingleton().installPlugin(new Ogre::ParticleFXPlugin());
#endif
#ifdef OGRE_BUILD_PLUGIN_STBI
  Ogre::STBIImageCodec::startup();
#endif
#ifdef OGRE_BUILD_PLUGIN_FREEIMAGE
  Ogre::FreeImageCodec::startup();
#endif
#ifdef OGRE_BUILD_PLUGIN_ASSIMP
#ifdef USE_ASSIMP_PLUGIN
  Ogre::Root::getSingleton().installPlugin(new Ogre::AssimpPlugin());
#endif
#endif
#ifdef OGRE_BUILD_PLUGIN_DOT_SCENE
  Ogre::Root::getSingleton().installPlugin(new Ogre::DotScenePlugin());
#else
  Ogre::Root::getSingleton().installPlugin(new Ogre::DotScenePluginB());
#endif
#ifdef OGRE_BUILD_COMPONENT_TERRAIN
  auto *terrainGlobalOptions = Ogre::TerrainGlobalOptions::getSingletonPtr();
  if (!terrainGlobalOptions) terrainGlobalOptions = new Ogre::TerrainGlobalOptions();
#endif
  ogreRoot->initialise(false);
}

void VideoManager::MakeWindow() {
  windowList.emplace_back();
  mainWindow = &windowList[0];
  camera = sceneManager->createCamera("Camera");
  mainWindow->Create("Demo0", camera, 0, 1270, 720, 0);
  camera->setNearClipDistance(0.1);
  camera->setFarClipDistance(1000.0);
  InputSequencer::GetInstance().RegWindowListener(mainWindow);
}

void VideoManager::InitOgreOverlay() {
  auto *ogreOverlay = new Ogre::OverlaySystem();
  imguiOverlay = new Ogre::ImGuiOverlay();
  ImGui_ImplSDL2_InitForOpenGL(windowList[0].sdlWindow, windowList[0].glContext);

  float vpScale = Ogre::OverlayManager::getSingleton().getPixelRatio();
  ImGui::GetIO().FontGlobalScale = round(vpScale);
  ImGui::GetStyle().ScaleAllSizes(vpScale);
  imguiOverlay->setZOrder(300);
  Ogre::OverlayManager::getSingleton().addOverlay(imguiOverlay);
  sceneManager->addRenderQueueListener(ogreOverlay);

  ImGuiIO &io = ImGui::GetIO();
  io.IniFilename = nullptr;
  io.LogFilename = nullptr;
  io.WantSaveIniSettings = false;
#ifdef MOBILE
  io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;
#endif
}

class DPSMCameraSetup : public Ogre::PSSMShadowCameraSetup {
 public:
  DPSMCameraSetup() {}
  virtual ~DPSMCameraSetup() {}

  static Ogre::ShadowCameraSetupPtr create() { return make_shared<DPSMCameraSetup>(); }

  /// Default shadow camera setup
  void getShadowCamera(const Ogre::SceneManager *sm, const Ogre::Camera *cam, const Ogre::Viewport *vp, const Ogre::Light *light,
                       Ogre::Camera *texCam, size_t iteration) const override {
    auto lType = light->getType();

    if (lType == Ogre::Light::LT_POINT) {
      Ogre::LiSPSMShadowCameraSetup::getShadowCamera(sm, cam, vp, light, texCam, iteration);

    } else if (lType == Ogre::Light::LT_DIRECTIONAL) {
      Ogre::PSSMShadowCameraSetup::getShadowCamera(sm, cam, vp, light, texCam, iteration);

    } else if (lType == Ogre::Light::LT_SPOTLIGHT) {
      Ogre::LiSPSMShadowCameraSetup::getShadowCamera(sm, cam, vp, light, texCam, iteration);

    } else if (lType == Ogre::Light::LT_RECTLIGHT) {
      Ogre::LiSPSMShadowCameraSetup::getShadowCamera(sm, cam, vp, light, texCam, iteration);

    } else {
      Ogre::DefaultShadowCameraSetup::getShadowCamera(sm, cam, vp, light, texCam, iteration);
    }
  }
};

void VideoManager::InitOgreSceneManager() {
  sceneManager->setFog(Ogre::FOG_EXP, Ogre::ColourValue(0.5, 0.6, 0.7), 0.003);
  sceneManager->setSkyBox(true, "SkyBox", 500, false);
  sceneManager->setAmbientLight(Ogre::ColourValue::White * 0.32);
  shadowEnabled = !RenderSystemIsGLES2();
  if (shadowEnabled) {
    Ogre::PixelFormat ShadowTextureFormat = Ogre::PixelFormat::PF_FLOAT16_R;
    pssmSetup = make_shared<DPSMCameraSetup>();
    sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
    sceneManager->setShadowColour(Ogre::ColourValue::Black);
    sceneManager->setShadowFarDistance(shadowFarDistance);
    sceneManager->setShadowDirectionalLightExtrusionDistance(shadowFarDistance);
    sceneManager->setShadowTextureSize(shadowTexSize);
    sceneManager->setShadowTextureSelfShadow(true);
    sceneManager->setShadowTexturePixelFormat(ShadowTextureFormat);
    sceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, pssmSplitCount);
    sceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_POINT, 1);
    sceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_SPOTLIGHT, 1);
    sceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_RECTLIGHT, 1);
    auto casterMaterial = Ogre::MaterialManager::getSingleton().getByName("PSSM/shadow_caster");
    sceneManager->setShadowTextureCasterMaterial(casterMaterial);
    sceneManager->setShadowTextureCount(shadowTexCount);
    for (int i = 0; i < shadowTexCount; i++) {
      Ogre::ShadowTextureConfig texConfig = sceneManager->getShadowTextureConfigList().at(i);
      texConfig.depthBufferPoolId = 2;
      texConfig.height = shadowTexSize / (floor(i / 3) + 1);
      texConfig.width = texConfig.height;
      if (RenderSystemIsGLES2()) {
        texConfig.height /= 2;
        texConfig.width /= 2;
      }
      sceneManager->setShadowTextureConfig(i, texConfig);
    }

    // pssm stuff
    shadowNearDistance = camera->getNearClipDistance();
    pssmSetup->setSplitPadding(pssmSplitPadding);
    for (int i = 0; i < pssmSplitCount; i++) {
      pssmSetup->setOptimalAdjustFactor(i, static_cast<Ogre::Real>(0.5 * i));
    }
    pssmSetup->calculateSplitPoints(pssmSplitCount, shadowNearDistance, sceneManager->getShadowFarDistance());
    pssmSplitPointList = pssmSetup->getSplitPoints();
    sceneManager->setShadowCameraSetup(pssmSetup);

  } else {
    sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
    sceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, 0);
    sceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_SPOTLIGHT, 0);
    sceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_RECTLIGHT, 0);
    sceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_POINT, 0);
    sceneManager->setShadowTextureCount(0);
  }
}

void VideoManager::EnableShadows(bool enable) {
  shadowEnabled = enable;
  if (shadowEnabled) {
    sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
    sceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, pssmSplitCount);
    sceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_SPOTLIGHT, 1);
    sceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_RECTLIGHT, 1);
    sceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_POINT, 1);
    sceneManager->setShadowTextureCount(shadowTexCount);
    InitOgreSceneManager();
  } else {
    sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
    sceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, 0);
    sceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_SPOTLIGHT, 0);
    sceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_RECTLIGHT, 0);
    sceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_POINT, 0);
    sceneManager->setShadowTextureCount(0);
  }
}

bool VideoManager::IsShadowEnabled() { return sceneManager->getShadowTechnique() != Ogre::SHADOWTYPE_NONE; }
void VideoManager::SetShadowTexSize(unsigned short size) { sceneManager->setShadowTextureSize(size); }
unsigned short VideoManager::GetShadowTexSize() { return IsShadowEnabled() ? sceneManager->getShadowTextureConfigList()[0].height : 0; }

void VideoManager::SetTexFiltering(unsigned int type, int anisotropy) {
  Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(static_cast<Ogre::TextureFilterOptions>(type));
  Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(anisotropy);
}

std::vector<float> VideoManager::GetPSSMSplitPoints() { return pssmSplitPointList; }

int VideoManager::GetDisplaySizeX(int index) {
  SDL_DisplayMode displayMode;
  return !SDL_GetCurrentDisplayMode(index, &displayMode) ? displayMode.w : -1;
}

int VideoManager::GetDisplaySizeY(int index) {
  SDL_DisplayMode displayMode;
  return !SDL_GetCurrentDisplayMode(index, &displayMode) ? displayMode.h : -1;
}

float VideoManager::GetDisplayDPI(int index) {
  SDL_DisplayMode displayMode;
  float ddpi = 0.0, hdpi = 0.0, vdpi = 0.0;
  int res = SDL_GetDisplayDPI(index, &ddpi, &hdpi, &vdpi);
  return !res ? ddpi : 1.0;
}

float VideoManager::GetDisplayHDPI(int index) {
  SDL_DisplayMode displayMode;
  float ddpi = 0.0, hdpi = 0.0, vdpi = 0.0;
  int res = SDL_GetDisplayDPI(index, &ddpi, &hdpi, &vdpi);
  return !res ? hdpi : 1.0;
}

float VideoManager::GetDisplayVDPI(int index) {
  SDL_DisplayMode displayMode;
  float ddpi = 0.0, hdpi = 0.0, vdpi = 0.0;
  int res = SDL_GetDisplayDPI(index, &ddpi, &hdpi, &vdpi);
  return !res ? vdpi : 1.0;
}

void VideoManager::ClearScene() {
  sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
  sceneManager->clearScene();
}

void VideoManager::UnloadResources() {
  sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
  sceneManager->clearScene();
  Ogre::ResourceGroupManager::getSingleton().unloadResourceGroup(Ogre::RGN_DEFAULT);
}

class VideoManager::ShaderResolver final : public Ogre::MaterialManager::Listener {
 public:
  explicit ShaderResolver(Ogre::RTShader::ShaderGenerator *shaderGenerator) : shaderGen(shaderGenerator) {}

  static bool ResolveMaterial(const std::string &name, const std::string &group = Ogre::RGN_AUTODETECT) {
    static auto &shaderGen = Ogre::RTShader::ShaderGenerator::getSingleton();
    static auto &ogreMaterialManager = Ogre::MaterialManager::getSingleton();
    auto material = ogreMaterialManager.getByName(name, group);
    const std::string scheme = Ogre::MSN_SHADERGEN;

    if (!material) {
      return false;
    }

    // SetUp shader generated technique for this material.
    bool tech = shaderGen.createShaderBasedTechnique(*material, Ogre::MaterialManager::DEFAULT_SCHEME_NAME, scheme);

    if (!tech) {
      return false;
    }

    // Force creating the shaders for the generated technique.
    shaderGen.validateMaterial(scheme, *material);

    Ogre::Material::Techniques::const_iterator it;
    for (it = material->getTechniques().begin(); it != material->getTechniques().end(); ++it) {
      Ogre::Technique *curTech = *it;

      if (curTech->getSchemeName() == scheme) {
        return curTech;
      }
    }

    return true;
  }

  Ogre::Technique *handleSchemeNotFound(unsigned short schemeIndex, const string &scheme, Ogre::Material *material, unsigned short lod,
                                        const Ogre::Renderable *rend) override {
    if (!shaderGen->hasRenderState(scheme)) {
      return nullptr;
    }
    // Case this is the default shader generator scheme.

    // Create shader generated technique for this material.
    bool tech = shaderGen->createShaderBasedTechnique(*material, Ogre::MaterialManager::DEFAULT_SCHEME_NAME, scheme);

    if (!tech) {
      return nullptr;
    }
    // Case technique registration succeeded.

    // Force creating the shaders for the generated technique.
    shaderGen->validateMaterial(scheme, *material);

    // Grab the generated technique.
    Ogre::Material::Techniques::const_iterator it;
    for (it = material->getTechniques().begin(); it != material->getTechniques().end(); ++it) {
      Ogre::Technique *curTech = *it;

      if (curTech->getSchemeName() == scheme) {
        return curTech;
      }
    }

    return nullptr;
  }

  bool afterIlluminationPassesCreated(Ogre::Technique *tech) override {
    if (shaderGen->hasRenderState(tech->getSchemeName())) {
      auto *mat = tech->getParent();
      shaderGen->validateMaterialIlluminationPasses(tech->getSchemeName(), mat->getName(), mat->getGroup());
      return true;
    }
    return false;
  }

  bool beforeIlluminationPassesCleared(Ogre::Technique *tech) override {
    if (shaderGen->hasRenderState(tech->getSchemeName())) {
      auto *mat = tech->getParent();
      shaderGen->invalidateMaterialIlluminationPasses(tech->getSchemeName(), mat->getName(), mat->getGroup());
      return true;
    }
    return false;
  }

 protected:
  Ogre::RTShader::ShaderGenerator *shaderGen = nullptr;
};

void VideoManager::RenderFrame() {
  ogreRoot->renderOneFrame();

  for (const auto &it : windowList) {
    it.RenderFrame();
  }
}

void VideoManager::ShowOverlay(bool show) {
  if (show) {
    imguiOverlay->show();
    ShaderResolver::ResolveMaterial("ImGui/material", Ogre::RGN_INTERNAL);
  } else {
    imguiOverlay->hide();
  }
}

void VideoManager::ShowOgreProfiler(bool show) {
#if OGRE_PROFILING == 1
  if (show) {
    Ogre::FontManager::getSingleton().load("SdkTrays/Value", Ogre::RGN_INTERNAL);
    Ogre::MaterialManager::getSingleton().load("Fonts/SdkTrays/Value", Ogre::RGN_INTERNAL);
    ShaderResolver::ResolveMaterial("Fonts/SdkTrays/Value", Ogre::RGN_INTERNAL);
  }
  Ogre::Profiler::getSingleton().setEnabled(show);
#endif
}

void VideoManager::RebuildOverlayFontAtlas() {
  ImGuiIO &io = ImGui::GetIO();
  unsigned char *pixels;
  int width, height;

  if (auto mat = Ogre::MaterialManager::getSingleton().getByName("ImGui/material", Ogre::RGN_INTERNAL)) {
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
// doesn't work at windows
#ifdef WIN32
    Ogre::TextureManager::getSingleton().unload("ImGui/FontTex", Ogre::RGN_INTERNAL);
    Ogre::TextureManager::getSingleton().remove("ImGui/FontTex", Ogre::RGN_INTERNAL);
    auto tex = Ogre::TextureManager::getSingleton().createManual("ImGui/FontTex", Ogre::RGN_INTERNAL, Ogre::TEX_TYPE_2D, width, height, 1, 1,
                                                                 Ogre::PF_BYTE_RGBA);
#else
    auto tex = Ogre::TextureManager::getSingleton().getByName("ImGui/FontTex");
#endif

    tex->getBuffer()->blitFromMemory(Ogre::PixelBox(Ogre::Box(0, 0, width, height), Ogre::PF_BYTE_RGBA, pixels));
  }
}

ImFont *VideoManager::AddOverlayFont(const std::string &name, const int size, const std::string &group, const ImFontConfig *cfg,
                                     const ImWchar *ranges) {
  typedef std::vector<ImWchar> CodePointRange;
  std::vector<CodePointRange> mCodePointRanges;
  ImGuiIO &io = ImGui::GetIO();

  Ogre::FontPtr font = Ogre::FontManager::getSingleton().getByName(name, group);
  ASSERTION(font, "font does not exist");
  ASSERTION(font->getType() == Ogre::FT_TRUETYPE, "font must be of FT_TRUETYPE");
  Ogre::DataStreamPtr dataStreamPtr = Ogre::ResourceGroupManager::getSingleton().openResource(font->getSource(), font->getGroup());
  Ogre::MemoryDataStream ttfchunk(dataStreamPtr, false);  // transfer ownership to imgui

  // convert codepoint ranges for imgui
  CodePointRange cprange;
  for (const auto &r : font->getCodePointRangeList()) {
    cprange.push_back(r.first);
    cprange.push_back(r.second);
  }

  const ImWchar *cprangePtr = io.Fonts->GetGlyphRangesDefault();
  if (!cprange.empty()) {
    cprange.push_back(0);  // terminate
    mCodePointRanges.push_back(cprange);
    // ptr must persist until createFontTexture
    cprangePtr = mCodePointRanges.back().data();
  }

  float vpScale = Ogre::OverlayManager::getSingleton().getPixelRatio();
  int size_ = size > 0 ? size : font->getTrueTypeSize();

  ImFont *res = io.Fonts->AddFontFromMemoryTTF(ttfchunk.getPtr(), ttfchunk.size(), size_ * vpScale, cfg, ranges);

  return res;
}

void VideoManager::EnableGamepadNav(bool enable) {
  ImGuiIO &io = ImGui::GetIO();
  enable ? io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad : io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableGamepad;
}

void VideoManager::EnableKeyboardNav(bool enable) {
  ImGuiIO &io = ImGui::GetIO();
  enable ? io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard : io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;
}

void VideoManager::InitOgreRTSS() {
  bool result = Ogre::RTShader::ShaderGenerator::initialize();
  ASSERTION(result, "[VideoManager] OGRE RTSS init failed");
  auto &shaderGen = Ogre::RTShader::ShaderGenerator::getSingleton();
  shaderGen.addSceneManager(sceneManager);
  shaderGen.setShaderCachePath("");
  shaderResolver = make_unique<ShaderResolver>(&shaderGen);
  auto *schemRenderState = shaderGen.getRenderState(Ogre::MSN_SHADERGEN);

  // Add the hardware skinning to the shader generator default render state
  auto *subRenderState = shaderGen.createSubRenderState(Ogre::RTShader::SRS_HARDWARE_SKINNING);
  schemRenderState->addTemplateSubRenderState(subRenderState);

  // increase max bone count for higher efficiency
  Ogre::RTShader::HardwareSkinningFactory::getSingleton().setMaxCalculableBoneCount(80);

  // re-generate shaders to include new SRSs
  shaderGen.invalidateScheme(Ogre::MSN_SHADERGEN);

  // Add listener
  // Ogre::MaterialManager::getSingleton().addListener(shaderResolver.get());
}

void VideoManager::OnSetUp() {
  // reg as input listener
  InputSequencer::GetInstance().RegWindowListener(this);

  // init
  InitOgreRoot();
  InitSDL();
  MakeWindow();
  CheckGPU();
  InitOgreRTSS();
  InitOgreOverlay();
  LoadResources();
  InitOgreSceneManager();
}

}  // namespace gge
