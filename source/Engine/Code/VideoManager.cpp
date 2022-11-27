/// created by Andrey Vasiliev

#include "pch.h"
#include "VideoManager.h"
#include "DotSceneLoaderB/DotSceneLoaderB.h"
#include "Platform.h"
#include "imgui_impl_sdl.h"
#ifdef OGRE_OPENGL
#include <OgreGLRenderSystemCommon.h>
#endif
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
#include "TerrainMaterialGeneratorB.h"
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
#include <ghc/filesystem.hpp>
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
inline bool RenderSystemIsGL() { return Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL Rendering Subsystem"; };
inline bool RenderSystemIsGL3() { return Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL 3+ Rendering Subsystem"; };
inline bool RenderSystemIsGLES2() { return Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL ES 2.x Rendering Subsystem"; };
inline void ParseSDLError(bool result, const char *message = "") {
  if (!result) LogError(message, SDL_GetError());
}

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
  const char *FILE_SYSTEM = "FileSystem";
  const char *ZIP = "Zip";
  std::vector<std::string> resourceList;

  if (!fs::is_directory(path)) {
    LogError("[ScanLocation]", "path is not directory");
    return;
  }
  auto &ogreResourceManager = Ogre::ResourceGroupManager::getSingleton();
  ogreResourceManager.addResourceLocation(path, FILE_SYSTEM, groupName);

  for (fs::recursive_directory_iterator end, it(path); it != end; ++it) {
    const std::string fullPath = it->path().string();
    const std::string fileExtention = it->path().filename().extension().string();

    if (it->is_directory())
      ogreResourceManager.addResourceLocation(fullPath, FILE_SYSTEM, groupName);
    else if (it->is_regular_file() && fileExtention == ".bin")
      ogreResourceManager.addResourceLocation(fullPath, ZIP, groupName);
  }
}
#endif  // DESKTOP
}  // namespace

namespace Glue {
#ifdef OGRE_BUILD_RENDERSYSTEM_GL3PLUS
void InitOgreRenderSystemGL3();
#endif
#ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
void InitOgreRenderSystemGLES2();
#endif
#ifdef OGRE_BUILD_RENDERSYSTEM_GL
void InitOgreRenderSystemGL();
#endif
void InitEmbeddedResources();

void VideoManager::LoadResources() {
#ifdef ANDROID
  JNIEnv *env = static_cast<JNIEnv *>(SDL_AndroidGetJNIEnv());
  jclass classActivity = env->FindClass("android/app/Activity");
  jclass classResources = env->FindClass("android/content/res/Resources");
  jmethodID methodGetResources = env->GetMethodID(classActivity, "getResources", "()Landroid/content/res/Resources;");
  jmethodID methodGetAssets = env->GetMethodID(classResources, "getAssets", "()Landroid/content/res/AssetManager;");
  jobject rawActivity = static_cast<jobject>(SDL_AndroidGetActivity());
  jobject rawResources = env->CallObjectMethod(rawActivity, methodGetResources);
  jobject rawAssetManager = env->CallObjectMethod(rawResources, methodGetAssets);
  AAssetManager *assetManager = AAssetManager_fromJava(env, rawAssetManager);
  AConfiguration *aConfig = AConfiguration_new();
  AConfiguration_fromAssetManager(aConfig, assetManager);
  auto &archiveManager = Ogre::ArchiveManager::getSingleton();
  archiveManager.addArchiveFactory(new Ogre::APKFileSystemArchiveFactory(assetManager));
  archiveManager.addArchiveFactory(new Ogre::APKZipArchiveFactory(assetManager));
#endif

  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(Ogre::MIP_UNLIMITED);
  Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_BILINEAR);
  auto &ogreResourceManager = Ogre::ResourceGroupManager::getSingleton();

  const char *FILE_SYSTEM = "FileSystem";
  const char *ZIP = "Zip";
  const char *APKZIP = "APKZip";
  const char *EMZIP = "EmbeddedZip";
  const char *PROGRAMS_ZIP = "programs.bin";
  const char *ASSETS_ZIP = "assets.bin";
  const int SCAN_DEPTH = 4;
  const char *PROGRAMS_DIR = "source/Programs";
  const char *GLSL_DIR = "source/GLSL";
  const char *GLSLES_DIR = "source/GLSLES";
  const char *ASSETS_DIR = "source/Example/Assets";

#if defined(DESKTOP)
  if (!FindPath(PROGRAMS_ZIP).empty()) {
    ogreResourceManager.addResourceLocation(FindPath(PROGRAMS_ZIP), ZIP, Ogre::RGN_INTERNAL);
  } else if (!FindPath("programs").empty()) {
    ScanLocation(FindPath("programs"), Ogre::RGN_INTERNAL);
  } else if (!FindPath(PROGRAMS_DIR, SCAN_DEPTH).empty()) {
    ScanLocation(FindPath(PROGRAMS_DIR, SCAN_DEPTH), Ogre::RGN_INTERNAL);
    if (RenderSystemIsGLES2())
      ScanLocation(FindPath(GLSLES_DIR, SCAN_DEPTH), Ogre::RGN_INTERNAL);
    else
      ScanLocation(FindPath(GLSL_DIR, SCAN_DEPTH), Ogre::RGN_INTERNAL);
  } else {
    // InitEmbeddedResources();
  }

  if (!FindPath(ASSETS_ZIP).empty()) {
    ogreResourceManager.addResourceLocation(FindPath(ASSETS_ZIP), ZIP, Ogre::RGN_DEFAULT);
  } else if (!FindPath("assets").empty()) {
    ScanLocation(FindPath("assets"), Ogre::RGN_DEFAULT);
  } else {
    ScanLocation(FindPath(ASSETS_DIR, SCAN_DEPTH), Ogre::RGN_DEFAULT);
  }

#elif defined(ANDROID)
  ogreResourceManager.addResourceLocation(PROGRAMS_ZIP, APKZIP, Ogre::RGN_INTERNAL);
  ogreResourceManager.addResourceLocation(ASSETS_ZIP, APKZIP, Ogre::RGN_DEFAULT);
#elif defined(IOS) || defined(EMSCRIPTEN)
  ogreResourceManager.addResourceLocation(PROGRAMS_ZIP, ZIP, Ogre::RGN_INTERNAL);
  ogreResourceManager.addResourceLocation(ASSETS_ZIP, ZIP, Ogre::RGN_DEFAULT);
#endif

  ogreResourceManager.initialiseResourceGroup(Ogre::RGN_INTERNAL);
  ogreResourceManager.initialiseAllResourceGroups();
}

VideoManager::VideoManager()
    : ogreMinLogLevel(Ogre::LML_NORMAL),
      ogreLogFile("Ogre.log"),
      shadowEnabled(true),
      shadowTechnique(Ogre::SHADOWTYPE_NONE),
      pssmSplitCount(3),
      shadowFarDistance(400.0),
      shadowTexSize(512),
      gamepadSupport(false) {}

VideoManager::~VideoManager() {
  if (imguiOverlay) {
    ImGui_ImplSDL2_Shutdown();
  }

  SDL_Quit();
}

void VideoManager::OnUpdate(float time) {
  if (imguiOverlay && gamepadSupport) {
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
  ogreSceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
  ogreSceneManager->clearScene();
  Ogre::ResourceGroupManager::getSingleton().unloadResourceGroup(Ogre::RGN_DEFAULT);
}

void VideoManager::RenderFrame() {
  ogreRoot->renderOneFrame();

  for (const auto &it : windowList) {
    it.RenderFrame();
  }
}

Window &VideoManager::GetWindow(int number) { return windowList[0]; }

Window &VideoManager::GetMainWindow() { return *mainWindow; }

void VideoManager::ShowWindow(bool show, int index) { windowList[index].Show(show); }

void VideoManager::CheckGPU() {
  const auto *ogreRenderSystem = ogreRoot->getRenderSystem();
  const auto *ogreRenderCapabilities = ogreRenderSystem->getCapabilities();
  const auto *ogreRenderSystemCommon = static_cast<const Ogre::GLRenderSystemCommon *>(ogreRenderSystem);
  ASSERTION(ogreRenderCapabilities->hasCapability(Ogre::RSC_HWRENDER_TO_TEXTURE), "Render to texture support required");
  ASSERTION(ogreRenderCapabilities->hasCapability(Ogre::RSC_TEXTURE_FLOAT), "Float texture support required");
  ASSERTION(ogreRenderCapabilities->hasCapability(Ogre::RSC_TEXTURE_COMPRESSION), "Texture compression support required");
  ASSERTION(ogreRenderCapabilities->hasCapability(Ogre::RSC_VERTEX_BUFFER_INSTANCE_DATA), "Instancing support required");
#if defined(DESKTOP)
  ASSERTION(ogreRenderCapabilities->hasCapability(Ogre::RSC_TEXTURE_COMPRESSION_DXT), "DXT compression support required");
#elif defined(ANDROID)
  ASSERTION(ogreRenderCapabilities->hasCapability(Ogre::RSC_TEXTURE_COMPRESSION_ETC1), "ETC1 compression support required");
#elif defined(IOS)
  ASSERTION(ogreRenderCapabilities->hasCapabilityOgre::(RSC_TEXTURE_COMPRESSION_PVRTC), "PVRTC compression support required");
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
  ParseSDLError(!result, "SDL_Init failed");
  ASSERTION(!result, "Failed to init SDL");
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
      of << "[" << chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count() << "] ";
      of << message << '\n';
      of.flush();
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
#endif  // DESKTOP
#else   // !DESKTOP
  InitOgreRenderSystemGLES2();
#endif  // DESKTOP

#ifdef OGRE_BUILD_PLUGIN_OCTREE
  Ogre::Root::getSingleton().addSceneManagerFactory(new Ogre::OctreeSceneManagerFactory());
  ogreSceneManager = ogreRoot->createSceneManager("OctreeSceneManager", "Default");
#else
  ogreSceneManager = OgreRoot->createSceneManager(Ogre::ST_GENERIC, "Default");
#endif
#ifdef OGRE_BUILD_PLUGIN_PFX
  Ogre::Root::getSingleton().installPlugin(new Ogre::ParticleFXPlugin());
#endif
#ifdef OGRE_BUILD_PLUGIN_STBI
  Ogre::Root::getSingleton().installPlugin(new Ogre::STBIPlugin());
#endif
#ifdef OGRE_BUILD_PLUGIN_FREEIMAGE
  Ogre::Root::getSingleton().installPlugin(new Ogre::FreeImagePlugin());
#endif
#if defined(OGRE_BUILD_PLUGIN_ASSIMP)
  Ogre::Root::getSingleton().installPlugin(new Ogre::AssimpPlugin());
#endif
#ifdef OGRE_BUILD_PLUGIN_DOT_SCENE
  Ogre::Root::getSingleton().installPlugin(new Ogre::DotScenePlugin());
#else
  Ogre::Root::getSingleton().installPlugin(new Ogre::DotScenePluginB());
#endif
#ifdef OGRE_BUILD_COMPONENT_TERRAIN
  auto *terrainGlobalOptions = Ogre::TerrainGlobalOptions::getSingletonPtr();
  if (!terrainGlobalOptions) terrainGlobalOptions = new Ogre::TerrainGlobalOptions();
  terrainGlobalOptions->setDefaultMaterialGenerator(make_shared<Ogre::TerrainMaterialGeneratorB>());
  terrainGlobalOptions->setUseRayBoxDistanceCalculation(true);
  terrainGlobalOptions->setCompositeMapDistance(300);
  terrainGlobalOptions->setCastsDynamicShadows(false);
  terrainGlobalOptions->setUseVertexCompressionWhenAvailable(true);
#endif
  ogreRoot->initialise(false);
}

void VideoManager::MakeWindow() {
  windowList.emplace_back();
  mainWindow = &windowList[0];
  ogreCamera = ogreSceneManager->createCamera("Default");
  mainWindow->Create("Example0", ogreCamera, 0, 1270, 720, 0);
  ogreViewport = mainWindow->ogreViewport;
  InputSequencer::GetInstance().RegWindowListener(mainWindow);
}

void VideoManager::InitOgreOverlay() {
  auto *ogreOverlay = new Ogre::OverlaySystem();
  imguiOverlay = new Ogre::ImGuiOverlay();
  ImGui_ImplSDL2_InitForOpenGL(windowList[0].sdlWindow, windowList[0].sdlGlContext);

  float vpScale = Ogre::OverlayManager::getSingleton().getPixelRatio();
  ImGui::GetIO().FontGlobalScale = round(vpScale);
  ImGui::GetStyle().ScaleAllSizes(vpScale);
  imguiOverlay->setZOrder(300);
  Ogre::OverlayManager::getSingleton().addOverlay(imguiOverlay);
  ogreSceneManager->addRenderQueueListener(ogreOverlay);

  ImGuiIO &io = ImGui::GetIO();
  io.IniFilename = nullptr;
  io.LogFilename = nullptr;
  io.WantSaveIniSettings = false;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
#ifdef MOBILE
  io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;
#endif
}

void VideoManager::InitOgreScene() {
#ifdef DESKTOP
  shadowEnabled = true;
#else
  shadowEnabled = false;
#endif
  if (shadowEnabled) {
#ifdef DESKTOP
    Ogre::PixelFormat ShadowTextureFormat = Ogre::PixelFormat::PF_DEPTH16;
#else
    Ogre::PixelFormat ShadowTextureFormat = Ogre::PixelFormat::PF_FLOAT16_R;
#endif
    shadowFarDistance = 400;
#ifdef DESKTOP
    shadowTexSize = 1024;
#else
    shadowTexSize = 512;
#endif

    ogreSceneManager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
    ogreSceneManager->setShadowFarDistance(shadowFarDistance);
    ogreSceneManager->setShadowTextureSize(shadowTexSize);
    ogreSceneManager->setShadowTexturePixelFormat(ShadowTextureFormat);
    ogreSceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, 3);
    ogreSceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_SPOTLIGHT, 1);
    ogreSceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_POINT, 0);
    auto casterMaterial = Ogre::MaterialManager::getSingleton().getByName("PSSM/shadow_caster_alpha");
    ogreSceneManager->setShadowTextureCasterMaterial(casterMaterial);

    // pssm stuff
    pssmSetup = make_shared<Ogre::PSSMShadowCameraSetup>();
    pssmSetup->calculateSplitPoints(pssmSplitCount, 0.001, ogreSceneManager->getShadowFarDistance());
    pssmSplitPointList = pssmSetup->getSplitPoints();
    pssmSetup->setSplitPadding(0.0);
    for (int i = 0; i < pssmSplitCount; i++) {
      pssmSetup->setOptimalAdjustFactor(i, static_cast<Ogre::Real>(0.5 * i));
    }
    ogreSceneManager->setShadowCameraSetup(pssmSetup);
    auto *schemRenderState = Ogre::RTShader::ShaderGenerator::getSingleton().getRenderState(Ogre::MSN_SHADERGEN);
    auto *subRenderState = static_cast<Ogre::RTShader::IntegratedPSSM3 *>(schemRenderState->getSubRenderState(Ogre::RTShader::SRS_INTEGRATED_PSSM3));
    subRenderState->setSplitPoints(pssmSplitPointList);

  } else {
    ogreSceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
    ogreSceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, 0);
    ogreSceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_SPOTLIGHT, 0);
    ogreSceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_POINT, 0);
  }
}

void VideoManager::ClearScene() {
  ogreSceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
  ogreSceneManager->clearScene();
}

void VideoManager::UnloadResources() {
  ogreSceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
  ogreSceneManager->clearScene();
  Ogre::ResourceGroupManager::getSingleton().unloadResourceGroup(Ogre::RGN_DEFAULT);
}

class VideoManager::ShaderResolver final : public Ogre::MaterialManager::Listener {
 public:
  explicit ShaderResolver(Ogre::RTShader::ShaderGenerator *shaderGenerator) : shaderGen(shaderGenerator) {}

  static bool ResolveMaterial(const std::string &name, const std::string &group = Ogre::RGN_AUTODETECT) {
    static auto &shaderGen = Ogre::RTShader::ShaderGenerator::getSingleton();
    static auto &ogreMaterialManager = Ogre::MaterialManager::getSingleton();
    auto material = ogreMaterialManager.getByName(name);
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

ImFont *VideoManager::AddFont(const std::string &name, const std::string &group, const ImFontConfig *cfg, const ImWchar *ranges) {
  typedef std::vector<ImWchar> CodePointRange;
  std::vector<CodePointRange> mCodePointRanges;
  static ImGuiIO &io = ImGui::GetIO();

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

  return io.Fonts->AddFontFromMemoryTTF(ttfchunk.getPtr(), ttfchunk.size(), font->getTrueTypeSize() * vpScale, cfg, ranges);
}

void VideoManager::GamepadEnable(bool enable) { gamepadSupport = enable; }

void VideoManager::ShowOverlay(bool show) {
  if (show) {
    imguiOverlay->show();
    ShaderResolver::ResolveMaterial("ImGui/material", Ogre::RGN_INTERNAL);
  } else {
    imguiOverlay->hide();
  }
}

void VideoManager::InitOgreRTSS() {
  bool result = Ogre::RTShader::ShaderGenerator::initialize();
  ASSERTION(result, "[VideoManager] OGRE RTSS init failed");
  auto &shaderGen = Ogre::RTShader::ShaderGenerator::getSingleton();
  shaderGen.addSceneManager(ogreSceneManager);
  shaderGen.setShaderCachePath("");
  shaderResolver = make_unique<ShaderResolver>(&shaderGen);

  auto *schemRenderState = shaderGen.getRenderState(Ogre::MSN_SHADERGEN);
  auto *subRenderState = shaderGen.createSubRenderState(Ogre::RTShader::SRS_INTEGRATED_PSSM3);
  schemRenderState->addTemplateSubRenderState(subRenderState);

  // Add the hardware skinning to the shader generator default render state
  subRenderState = shaderGen.createSubRenderState(Ogre::RTShader::SRS_HARDWARE_SKINNING);
  schemRenderState->addTemplateSubRenderState(subRenderState);

  // increase max bone count for higher efficiency
  Ogre::RTShader::HardwareSkinningFactory::getSingleton().setMaxCalculableBoneCount(80);

  // re-generate shaders to include new SRSs
  shaderGen.invalidateScheme(Ogre::MSN_SHADERGEN);
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
  InitOgreScene();
}
}  // namespace Glue
