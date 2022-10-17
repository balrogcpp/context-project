/// created by Andrey Vasiliev

#include "pch.h"
#include "Engine.h"
#include "DotSceneLoaderB.h"
#include "Observer.h"
#include "PagedGeometry/PagedGeometryAll.h"
#include "SinbadCharacterController.h"
#include "SkyModel/ArHosekSkyModel.h"
#include "SkyModel/SkyModel.h"
#ifdef ANDROID
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
extern "C" {
#ifdef _MSC_VER
#define SDL_MAIN_HANDLED
#endif
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
}
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
#ifdef OGRE_BUILD_COMPONENT_BULLET
#include <Bullet/OgreBullet.h>
#endif
#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
#include <RTShaderSystem/OgreRTShaderSystem.h>
#include <RTShaderSystem/OgreShaderGenerator.h>
#endif
#ifdef OGRE_BUILD_COMPONENT_TERRAIN
#include "TerrainMaterialGeneratorB.h"
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include <Terrain/OgreTerrainMaterialGeneratorA.h>
#include <Terrain/OgreTerrainQuadTreeNode.h>
#endif
#ifdef OGRE_BUILD_COMPONENT_OVERLAY
#include <Overlay/OgreFontManager.h>
#include <Overlay/OgreImGuiOverlay.h>
#include <Overlay/OgreOverlayManager.h>
#endif
#if defined(DESKTOP) && defined(DEBUG)
#if __has_include(<filesystem>) && ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) \
    || (defined(__cplusplus) && __cplusplus >= 201703L && !defined(__APPLE__)) \
    || (!defined(__MAC_OS_X_VERSION_MIN_REQUIRED) || __MAC_OS_X_VERSION_MIN_REQUIRED >= 101500))
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;
#endif
#endif  // DESKTOP
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
extern "C" {
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

using namespace std;
using namespace Ogre;

int ErrorWindow(const char *WindowCaption, const char *MessageText) {
  using namespace Glue;
  static Engine *EnginePtr = Engine::GetInstancePtr();
  if (EnginePtr) EnginePtr->GrabCursor(false);
  SDL_Log("%s%s%s", WindowCaption, " : ", MessageText);
#ifdef _WIN32
  MessageBox(nullptr, MessageText, WindowCaption, MB_ICONERROR);
#else
  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, WindowCaption, MessageText, nullptr);
#endif
#ifdef EMSCRIPTEN
  emscripten_pause_main_loop();
#endif
  return -1;
}

namespace Glue {

void ShowMouseCursor(bool draw) {
  //  static ImGuiIO& io = ImGui::GetIO();
  //  io.MouseDrawCursor = draw;
}

Engine &GetEngine() {
  static auto &EnginePtr = Engine::GetInstance();
  return EnginePtr;
}

Physics &GetPhysics() { return GetComponent<Physics>(); }

Audio &GetAudio() { return GetComponent<Audio>(); }

#if defined(DESKTOP) && defined(DEBUG)
static inline bool IsHidden(const fs::path &Path) {
  string name = Path.filename().string();
  return name == ".." && name == "." && name[0] == '.';
}

static inline string FindPath(const string &path, int depth = 4) {
  string result = path;

  for (int i = 0; i < depth; i++) {
    if (fs::exists(result))
      return result;
    else if (fs::exists(result + ".zip"))
      return result.append(".zip");
    else
#ifndef WIN32
      result = string("../").append(result);
#else
      result = string("..\\").append(result);
#endif
  }

  return "";
}

static void AddLocation(const char *path, const string &groupName = RGN_DEFAULT) {
  const char *FILE_SYSTEM = "FileSystem";
  const char *ZIP = "Zip";

  vector<string> fileList;
  vector<string> dirList;
  vector<tuple<string, string, string>> resourceList;
  auto &RGM = ResourceGroupManager::getSingleton();

  string newPath = FindPath(path);

  if (fs::exists(newPath)) {
    if (fs::is_directory(newPath))
      resourceList.push_back(make_tuple(newPath, FILE_SYSTEM, groupName));
    else if (fs::is_regular_file(newPath) && fs::path(newPath).extension() == ".zip")
      RGM.addResourceLocation(newPath, ZIP, groupName);
  }

  for (const auto &it : resourceList) {
    RGM.addResourceLocation(get<0>(it), get<1>(it), get<2>(it));
    dirList.push_back(get<0>(it));

    for (fs::recursive_directory_iterator end, jt(get<0>(it)); jt != end; ++jt) {
      const string full_path = jt->path().string();
      const string fileName = jt->path().filename().string();
      const string extention = jt->path().filename().extension().string();

      if (jt->is_directory()) {
        if (IsHidden(jt->path())) {
          jt.disable_recursion_pending();
          continue;
        }

        dirList.push_back(fileName);

        RGM.addResourceLocation(full_path, FILE_SYSTEM, get<2>(it));
      } else if (jt->is_regular_file()) {
        if (IsHidden(jt->path())) continue;

        if (extention == ".zip") {
          RGM.addResourceLocation(full_path, ZIP, get<2>(it));
        } else {
          fileList.push_back(fileName);
        }
      }
    }
  }
}
#endif

#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
class ShaderResolver final : public MaterialManager::Listener {
 public:
  explicit ShaderResolver(RTShader::ShaderGenerator *shaderGenerator) : shaderGenerator(shaderGenerator) {}

  Technique *handleSchemeNotFound(unsigned short schemeIndex, const string &schemeName, Material *originalMaterial, unsigned short lodIndex,
                                  const Renderable *rend) override {
    if (!shaderGenerator->hasRenderState(schemeName)) {
      return nullptr;
    }
    // Case this is the default shader generator scheme.

    // Create shader generated technique for this material.
    bool techniqueCreated = shaderGenerator->createShaderBasedTechnique(*originalMaterial, Ogre::MaterialManager::DEFAULT_SCHEME_NAME, schemeName);

    if (!techniqueCreated) {
      return nullptr;
    }
    // Case technique registration succeeded.

    // Force creating the shaders for the generated technique.
    shaderGenerator->validateMaterial(schemeName, *originalMaterial);

    // Grab the generated technique.
    Ogre::Material::Techniques::const_iterator it;
    for (it = originalMaterial->getTechniques().begin(); it != originalMaterial->getTechniques().end(); ++it) {
      Ogre::Technique *curTech = *it;

      if (curTech->getSchemeName() == schemeName) {
        return curTech;
      }
    }

    return nullptr;
  }

  bool afterIlluminationPassesCreated(Technique *tech) override {
    if (shaderGenerator->hasRenderState(tech->getSchemeName())) {
      Ogre::Material *mat = tech->getParent();
      shaderGenerator->validateMaterialIlluminationPasses(tech->getSchemeName(), mat->getName(), mat->getGroup());
      return true;
    }
    return false;
  }

  bool beforeIlluminationPassesCleared(Technique *tech) override {
    if (shaderGenerator->hasRenderState(tech->getSchemeName())) {
      Ogre::Material *mat = tech->getParent();
      shaderGenerator->invalidateMaterialIlluminationPasses(tech->getSchemeName(), mat->getName(), mat->getGroup());
      return true;
    }
    return false;
  }

 protected:
  RTShader::ShaderGenerator *shaderGenerator = nullptr;
};
#endif  // OGRE_BUILD_COMPONENT_RTSHADERSYSTEM

Engine::Engine() {}

Engine::~Engine() { SDL_SetWindowFullscreen(sdlWindow, SDL_FALSE); }

ImFont *AddFont(const String &name, const char *group OGRE_RESOURCE_GROUP_INIT, const ImFontConfig *fontCfg = NULL,
                const ImWchar *glyphRanges = NULL) {
  typedef vector<ImWchar> CodePointRange;
  vector<CodePointRange> mCodePointRanges;

  FontPtr font = FontManager::getSingleton().getByName(name, group);
  OgreAssert(font, "font does not exist");
  OgreAssert(font->getType() == FT_TRUETYPE, "font must be of FT_TRUETYPE");
  DataStreamPtr dataStreamPtr = ResourceGroupManager::getSingleton().openResource(font->getSource(), font->getGroup());
  MemoryDataStream ttfchunk(dataStreamPtr, false);  // transfer ownership to imgui

  // convert codepoint ranges for imgui
  CodePointRange cprange;
  for (const auto &r : font->getCodePointRangeList()) {
    cprange.push_back(r.first);
    cprange.push_back(r.second);
  }

  ImGuiIO &io = ImGui::GetIO();
  const ImWchar *cprangePtr = io.Fonts->GetGlyphRangesDefault();
  if (!cprange.empty()) {
    cprange.push_back(0);  // terminate
    mCodePointRanges.push_back(cprange);
    // ptr must persist until createFontTexture
    cprangePtr = mCodePointRanges.back().data();
  }

  float vpScale = OverlayManager::getSingleton().getPixelRatio();

  ImFontConfig cfg;
  strncpy(cfg.Name, name.c_str(), IM_ARRAYSIZE(cfg.Name) - 1);
  return io.Fonts->AddFontFromMemoryTTF(ttfchunk.getPtr(), ttfchunk.size(), font->getTrueTypeSize() * vpScale, fontCfg, +glyphRanges);
}

void Engine::Init() {
  // SDL init
#ifndef EMSCRIPTEN
  OgreAssert(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER), "Failed to init SDL2");
#else
  OgreAssert(!SDL_Init(SDL_INIT_VIDEO), "Failed to init SDL2");
#endif
  SDL_DisplayMode DM;
  for (int i = 0; i < SDL_GetNumVideoDisplays(); i++) {
    if (SDL_GetCurrentDisplayMode(i, &DM) == 0) {
      SDL_Log("Display #%d: current display mode is %dx%dpx @ %dhz.", i, DM.w, DM.h, DM.refresh_rate);
      sdlMonitorList.push_back(DM);
      int ScreenDiag = sqrt(screenWidth * screenWidth + screenHeight * screenHeight);
      int TmpDiag = sqrt(DM.w * DM.w + DM.h * DM.h);
      if (TmpDiag > ScreenDiag) {
        screenWidth = DM.w;
        screenHeight = DM.h;
        currentDisplay = i;
      } else {
        SDL_Log("Could not get display mode for video display #%d: %s", i, SDL_GetError());
      }
    }
  }
  for (int i = 0; i < SDL_NumJoysticks(); i++) {
    if (SDL_IsGameController(i)) SDL_GameControllerOpen(i);
  }

  // SDL window
#if defined(DESKTOP)
  if (windowWidth == screenWidth && windowHeight == screenHeight) {
    sdlWindowFlags |= SDL_WINDOW_BORDERLESS;
  }
  if (windowFullScreen) {
    sdlWindowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    sdlWindowFlags |= SDL_WINDOW_BORDERLESS;
    windowWidth = screenWidth;
    windowHeight = screenHeight;
  }
  sdlWindowPositionFlag = SDL_WINDOWPOS_CENTERED_DISPLAY(currentDisplay);
  sdlWindow = SDL_CreateWindow(windowCaption.c_str(), sdlWindowPositionFlag, sdlWindowPositionFlag, windowWidth, windowHeight, sdlWindowFlags);
#else
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  sdlWindowFlags |= SDL_WINDOW_OPENGL;
#ifdef EMSCRIPTEN
  sdlWindowFlags |= SDL_WINDOW_RESIZABLE;
#endif
  sdlWindowFlags |= SDL_WINDOW_ALLOW_HIGHDPI;
#ifdef MOBILE
  sdlWindowFlags |= SDL_WINDOW_BORDERLESS;
  sdlWindowFlags |= SDL_WINDOW_FULLSCREEN;
#endif
  windowWidth = screenWidth;
  windowHeight = screenHeight;
  sdlWindowPositionFlag = SDL_WINDOWPOS_CENTERED;
  sdlWindow = SDL_CreateWindow(nullptr, sdlWindowPositionFlag, sdlWindowPositionFlag, screenWidth, screenHeight, sdlWindowFlags);
  SDL_GL_CreateContext(sdlWindow);
#endif

  // Init OGRE Root and plugins
  ogreRoot = new Root("", "", "");
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
  Root::getSingleton().addSceneManagerFactory(new OctreeSceneManagerFactory());
#endif
#ifdef OGRE_BUILD_PLUGIN_PFX
  Root::getSingleton().installPlugin(new ParticleFXPlugin());
#endif
#ifdef OGRE_BUILD_PLUGIN_STBI
  Root::getSingleton().installPlugin(new STBIPlugin());
#endif
#if defined(DEBUG) && defined(OGRE_BUILD_PLUGIN_FREEIMAGE) && !defined(OGRE_BUILD_PLUGIN_STBI)
  Root::getSingleton().installPlugin(new FreeImagePlugin());
#endif
#if defined(DEBUG) && defined(OGRE_BUILD_PLUGIN_ASSIMP)
  Root::getSingleton().installPlugin(new AssimpPlugin());
#endif
#ifdef OGRE_BUILD_PLUGIN_OCTREE
  sceneManager = ogreRoot->createSceneManager("OctreeSceneManager", "Default");
#else
  sceneManager = OgreRoot->createSceneManager(ST_GENERIC, "Default");
#endif
#ifdef OGRE_BUILD_PLUGIN_DOT_SCENE
  Root::getSingleton().installPlugin(new DotScenePluginB());
#else
  Root::getSingleton().installPlugin(new DotScenePluginB());
#endif
  ogreRoot->initialise(false);

  // OGRE window
  NameValuePairList renderParams;
  SDL_SysWMinfo info;
  SDL_VERSION(&info.version);
  SDL_GetWindowWMInfo(sdlWindow, &info);
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  renderParams["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(info.info.win.window));
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
  renderParams["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(info.info.x11.window));
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
  renderParams["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(info.info.cocoa.window));
#elif OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  renderParams["currentGLContext"] = "true";
  renderParams["externalGLControl"] = "true";
  renderParams["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(info.info.android.window));
  renderParams["preserveContext"] = "true";
  JNIEnv *env = static_cast<JNIEnv *> SDL_AndroidGetJNIEnv();
  jclass classActivity = env->FindClass("android/app/Activity");
  jclass classResources = env->FindClass("android/content/res/Resources");
  jmethodID methodGetResources = env->GetMethodID(classActivity, "getResources", "()Landroid/content/res/Resources;");
  jmethodID methodGetAssets = env->GetMethodID(classResources, "getAssets", "()Landroid/content/res/AssetManager;");
  jobject rawActivity = static_cast<jobject> SDL_AndroidGetActivity();
  jobject rawResources = env->CallObjectMethod(rawActivity, methodGetResources);
  jobject rawAssetManager = env->CallObjectMethod(rawResources, methodGetAssets);
  AAssetManager *assetManager = AAssetManager_fromJava(env, rawAssetManager);
  AConfiguration *aConfig = AConfiguration_new();
  AConfiguration_fromAssetManager(aConfig, assetManager);
  ArchiveManager::getSingleton().addArchiveFactory(new APKFileSystemArchiveFactory(assetManager));
  ArchiveManager::getSingleton().addArchiveFactory(new APKZipArchiveFactory(assetManager));
#endif
  const char *TRUE_STR = "true";
  const char *FALSE_STR = "false";
  windowVsync = true;
  renderParams["vsync"] = windowVsync ? TRUE_STR : FALSE_STR;
  ogreWindow = ogreRoot->createRenderWindow(windowCaption, windowWidth, windowHeight, windowFullScreen, &renderParams);
  RenderTarget *renderTarget = ogreRoot->getRenderTarget(ogreWindow->getName());
  camera = sceneManager->createCamera("Default");
  viewport = renderTarget->addViewport(camera);
  camera->setAspectRatio(static_cast<float>(viewport->getActualWidth()) / static_cast<float>(viewport->getActualHeight()));
  camera->setAutoAspectRatio(true);
#ifdef ANDROID
  SDL_GetDesktopDisplayMode(currentDisplay, &DM);
  ogreWindow->resize(static_cast<int>(DM.w), static_cast<int>(DM.h));
#endif

  // GPU capabilities
  const auto *RSC = Root::getSingleton().getRenderSystem()->getCapabilities();
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
#ifdef OGRE_BUILD_RENDERSYSTEM_GL3PLUS
    OgreAssert(CheckGL3Version(3, 3), "OpenGL 3.3 is not supported");
#endif
  } else if (RenderSystemIsGLES2()) {
#ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
    OgreAssert(CheckGLES2Version(3, 0), "OpenGLES 3.0 is not supported");
#endif
  }
// RTSS init
#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
  OgreAssert(RTShader::ShaderGenerator::initialize(), "OGRE RTSS init failed");
  auto *shaderGenerator = RTShader::ShaderGenerator::getSingletonPtr();
  viewport->setMaterialScheme(MSN_SHADERGEN);
  shaderGenerator->addSceneManager(sceneManager);
  shaderGenerator->setShaderCachePath("");
  auto resolver = make_unique<ShaderResolver>(shaderGenerator);
  MaterialManager::getSingleton().addListener(resolver.get());
#endif

  // overlay init
  auto *ogreOverlay = new OverlaySystem();
  imguiOverlay = new ImGuiOverlay();
  float vpScale = OverlayManager::getSingleton().getPixelRatio();
  ImGui::GetIO().FontGlobalScale = round(vpScale);  // default font does not work with fractional scaling
  ImGui::GetStyle().ScaleAllSizes(vpScale);
  imguiOverlay->setZOrder(300);
  OverlayManager::getSingleton().addOverlay(imguiOverlay);
  sceneManager->addRenderQueueListener(ogreOverlay);
  imguiListener = make_unique<ImGuiInputListener>();
  ImGuiIO &io = ImGui::GetIO();
  io.IniFilename = nullptr;
  io.LogFilename = nullptr;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
  io.MouseDrawCursor = false;
  io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;

  // texture init
  //  TextureManager::getSingleton().setDefaultNumMipmaps(MIP_UNLIMITED);
  //  if (Root::getSingleton().getRenderSystem()->getCapabilities()->hasCapability(RSC_ANISOTROPY)) {
  //    MaterialManager::getSingleton().setDefaultTextureFiltering(TFO_ANISOTROPIC);
  //    MaterialManager::getSingleton().setDefaultAnisotropy(8);
  //  } else {
  //    MaterialManager::getSingleton().setDefaultTextureFiltering(TFO_BILINEAR);
  //  }

  // scan resources
  auto &RGM = ResourceGroupManager::getSingleton();
#if defined(ANDROID)
  RGM.addResourceLocation("programs.zip", "APKZip", RGN_INTERNAL);
  RGM.addResourceLocation("assets.zip", "APKZip", RGN_DEFAULT);
#elif defined(DESKTOP) && defined(DEBUG)
  const char *PROGRAMS_DIR = "source/Programs";
  const char *ASSETS_DIR = "source/Example/Assets";
  AddLocation(PROGRAMS_DIR, RGN_INTERNAL);
  if (RenderSystemIsGLES2())
    AddLocation("source/GLSLES", RGN_INTERNAL);
  else
    AddLocation("source/GLSL", RGN_INTERNAL);
  AddLocation(ASSETS_DIR);
#else
  RGM.addResourceLocation("programs.zip", "Zip", RGN_INTERNAL);
  RGM.addResourceLocation("assets.zip", "Zip", RGN_DEFAULT);
#endif

  // load resources
  RGM.initialiseResourceGroup(RGN_INTERNAL);
  RGM.initialiseAllResourceGroups();

  // shadows init
  bool ShadowsEnabled = true;
#ifdef MOBILE
  ShadowsEnabled = false;
#endif
  if (!ShadowsEnabled) {
    sceneManager->setShadowTechnique(SHADOWTYPE_NONE);
    sceneManager->setShadowTextureCountPerLightType(Light::LT_DIRECTIONAL, 0);
    sceneManager->setShadowTextureCountPerLightType(Light::LT_SPOTLIGHT, 0);
    sceneManager->setShadowTextureCountPerLightType(Light::LT_POINT, 0);
  } else {
#ifdef DESKTOP
    PixelFormat ShadowTextureFormat = PixelFormat::PF_DEPTH16;
#else
    PixelFormat ShadowTextureFormat = PixelFormat::PF_FLOAT16_R;
#endif
    float shadowFarDistance = 400;
    int16_t shadowTexSize = 1024;
    sceneManager->setShadowTechnique(SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
    sceneManager->setShadowFarDistance(shadowFarDistance);
    sceneManager->setShadowTextureSize(shadowTexSize);
    sceneManager->setShadowTexturePixelFormat(ShadowTextureFormat);
    sceneManager->setShadowTextureCountPerLightType(Light::LT_DIRECTIONAL, 3);
    sceneManager->setShadowTextureCountPerLightType(Light::LT_SPOTLIGHT, 1);
    sceneManager->setShadowTextureCountPerLightType(Light::LT_POINT, 0);
    sceneManager->setShadowTextureSelfShadow(true);
    sceneManager->setShadowCasterRenderBackFaces(true);
    sceneManager->setShadowFarDistance(shadowFarDistance);
    auto passCaterMaterial = MaterialManager::getSingleton().getByName("PSSM/shadow_caster");
    sceneManager->setShadowTextureCasterMaterial(passCaterMaterial);
    pssmSetup = make_shared<PSSMShadowCameraSetup>();
    pssmSetup->calculateSplitPoints(PSSM_SPLITS, 0.001, sceneManager->getShadowFarDistance());
    pssmSplitPointList = pssmSetup->getSplitPoints();
    pssmSetup->setSplitPadding(0.0);
    for (int i = 0; i < PSSM_SPLITS; i++) pssmSetup->setOptimalAdjustFactor(i, static_cast<float>(0.5 * i));
    sceneManager->setShadowCameraSetup(pssmSetup);
    sceneManager->setShadowColour(ColourValue::Black);
#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
    auto *schemRenderState = shaderGenerator->getRenderState(MSN_SHADERGEN);
    auto subRenderState = shaderGenerator->createSubRenderState<RTShader::IntegratedPSSM3>();
    subRenderState->setSplitPoints(pssmSplitPointList);
    schemRenderState->addTemplateSubRenderState(subRenderState);
#endif  // OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
  }

  // components init
  physics = make_unique<Physics>();
  RegComponent(physics.get());

  audio = make_unique<Audio>();
  RegComponent(audio.get());

  InitCompositor();

  for (auto &it : componentList) it->OnSetUp();

  //  AddFont("NotoSans-Regular", RGN_INTERNAL, nullptr, io.Fonts->GetGlyphRangesCyrillic());
  //  ImFontConfig config;
  //  config.MergeMode = true;
  //  static const ImWchar icon_ranges[] = {ICON_MIN_MD, ICON_MAX_MD, 0};
  //  AddFont("KenneyIcon-Regular", RGN_INTERNAL, &config, icon_ranges);

  auto *TGO = TerrainGlobalOptions::getSingletonPtr();
  if (!TGO) TGO = new TerrainGlobalOptions();
  TGO->setUseVertexCompressionWhenAvailable(true);
  TGO->setCastsDynamicShadows(false);
  TGO->setCompositeMapDistance(300);
  TGO->setMaxPixelError(8);
  TGO->setUseRayBoxDistanceCalculation(true);
  TGO->setDefaultMaterialGenerator(make_shared<TerrainMaterialGeneratorB>());

  // cleanup
  imguiOverlay->show();
  ImGuiOverlay::NewFrame();
  ogreRoot->renderOneFrame();
#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
  MaterialManager::getSingleton().removeListener(resolver.get());
#endif
}

void Engine::Capture() {
  static auto &io = InputSequencer::GetInstance();
  io.Capture();
}

// constexpr ResolutionItem ResolutionList[] = {
//    {"360x800", 360, 800},
//    {"800x600", 800, 600},
//    {"810x1080", 810, 1080},
//    {"768x1024", 768, 1024},
//    {"834x1112", 834, 1112},
//    {"1024x768", 1024, 768},
//    {"1024x1366", 1024, 1366},
//    {"1280x720", 1280, 720},
//    {"1280x800", 1280, 800},
//    {"1280x1024", 1280, 1024},
//    {"1360x768", 1360, 768},
//    {"1366x768", 1366, 768},
//    {"1440x900", 1440, 900},
//    {"1536x864", 1536, 864},
//    {"1600x900", 1600, 900},
//    {"1680x1050", 1680, 1050},
//    {"1920x1080", 1920, 1080},
//    {"1920x1200", 1920, 1200},
//    {"2048x1152", 2048, 1152},
//    {"2560x1440", 2560, 1440},
//};

const static CompositorFX Bloom{"Bloom", false, "uBloomEnable", "Output", {"Bloom", "Downscale3x3"}};
const static CompositorFX SSAO{"SSAO", false, "uSSAOEnable", "Output", {"SSAO", "FilterX", "FilterY"}};
const static CompositorFX Blur{"Blur", false, "uBlurEnable", "Blur", {}};
const static CompositorFX FXAA{"FXAA", false, "uFXAAEnable", "FXAA", {}};
const static CompositorFX HDR{"HDR", false, "uHDREnable", "Output", {}};
const static string FX_SSAO = "SSAO";
const static string FX_BLOOM = "Bloom";
const static string FX_BLUR = "Blur";
const static string FX_FXAA = "FXAA";
const static string FX_HDR = "HDR";

static GpuProgramParametersSharedPtr GetFPparameters(const string &CompositorName) {
  static auto &MM = MaterialManager::getSingleton();
  return MM.getByName(CompositorName)->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
}

void Engine::EnableEffect(const string &FX, bool Enable) {
  compositorList[FX].Enabled = Enable;
  const string EnableStr = "uEnable";
  const float Flag = Enable ? 1.0f : 0.0f;
  const auto &FXPtr = compositorList[FX];
  GetFPparameters(FXPtr.OutputCompositor)->setNamedConstant(FXPtr.EnableParam, Flag);
  for (const auto &it : FXPtr.CompositorChain) GetFPparameters(it)->setNamedConstant(EnableStr, Flag);
}

void Engine::InitCompositor() {
  compositorManager = CompositorManager::getSingletonPtr();
  compositorChain = compositorManager->getCompositorChain(viewport);
  compositorList.insert(make_pair(FX_BLOOM, Bloom));
  compositorList.insert(make_pair(FX_SSAO, SSAO));
  compositorList.insert(make_pair(FX_BLUR, Blur));
  compositorList.insert(make_pair(FX_FXAA, FXAA));
  compositorList.insert(make_pair(FX_HDR, HDR));
  EnableEffect(FX_SSAO, false);
  EnableEffect(FX_BLOOM, false);
  EnableEffect(FX_BLUR, false);
  EnableEffect(FX_HDR, false);
  EnableEffect(FX_FXAA, false);

  const char *MRTCompositor = "MRT";
  int sizeY = viewport->getActualDimensions().height();
  int sizeX = viewport->getActualDimensions().width();
  OgreAssert(compositorManager->addCompositor(viewport, MRTCompositor, 0), "Failed to add MRT compositor");
  compositorManager->setCompositorEnabled(viewport, MRTCompositor, false);
  auto *MRTCompositorPtr = compositorChain->getCompositor(MRTCompositor);
  auto *MRTTexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("mrt");
  OgreAssert(MRTTexturePtr, "MRTCompositor texture not created");
#ifdef MOBILE
  MRTTexturePtr->width = 1024;
  MRTTexturePtr->height = 768;
#else
  MRTTexturePtr->width = sizeX;
  MRTTexturePtr->height = sizeY;
#endif
  auto *SSAOTexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("ssao");
  auto *FinalTexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("final");
  auto *RT1TexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("rt1");
  auto *RT2TexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("rt2");
  auto *RT3TexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("rt3");
  auto *RT4TexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("rt4");
  auto *RT5TexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("rt5");
  auto *RT6TexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("rt6");
  auto *RT7TexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("rt7");
  auto *RT8TexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("rt8");
  auto *RT9TexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("rt9");
  auto *RT10TexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("rt10");
  auto *RT11TexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("rt11");
  auto *RT12TexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("rt12");
  OgreAssert(SSAOTexturePtr, "SSAO texture not created");
  OgreAssert(FinalTexturePtr, "Final texture not created");
  OgreAssert(RT1TexturePtr, "RT1 texture not created");
  OgreAssert(RT2TexturePtr, "RT2 texture not created");
  OgreAssert(RT3TexturePtr, "RT3 texture not created");
  OgreAssert(RT4TexturePtr, "RT4 texture not created");
  OgreAssert(RT5TexturePtr, "RT5 texture not created");
  OgreAssert(RT6TexturePtr, "RT6 texture not created");
  OgreAssert(RT7TexturePtr, "RT7 texture not created");
  OgreAssert(RT8TexturePtr, "RT8 texture not created");
  OgreAssert(RT9TexturePtr, "RT9 texture not created");
  OgreAssert(RT10TexturePtr, "RT10 texture not created");
  OgreAssert(RT11TexturePtr, "RT11 texture not created");
  OgreAssert(RT12TexturePtr, "RT12 texture not created");
#ifdef MOBILE
  FinalTexturePtr->width = MRTTexturePtr->width;
  FinalTexturePtr->height = MRTTexturePtr->height;
#endif
  SSAOTexturePtr->width = MRTTexturePtr->width / 2;
  SSAOTexturePtr->height = MRTTexturePtr->height / 2;
  RT1TexturePtr->width = MRTTexturePtr->width / 2;
  RT1TexturePtr->height = MRTTexturePtr->height / 2;
  RT2TexturePtr->width = MRTTexturePtr->width / 4;
  RT2TexturePtr->height = MRTTexturePtr->height / 4;
  RT3TexturePtr->width = MRTTexturePtr->width / 8;
  RT3TexturePtr->height = MRTTexturePtr->height / 8;
  RT4TexturePtr->width = MRTTexturePtr->width / 16;
  RT4TexturePtr->height = MRTTexturePtr->height / 16;
  RT5TexturePtr->width = MRTTexturePtr->width / 32;
  RT5TexturePtr->height = MRTTexturePtr->height / 32;
  RT6TexturePtr->width = MRTTexturePtr->width / 64;
  RT6TexturePtr->height = MRTTexturePtr->height / 64;
  RT7TexturePtr->width = MRTTexturePtr->width / 128;
  RT7TexturePtr->height = MRTTexturePtr->height / 128;
  RT8TexturePtr->width = MRTTexturePtr->width / 256;
  RT8TexturePtr->height = MRTTexturePtr->height / 256;
  RT9TexturePtr->width = MRTTexturePtr->width / 512;
  RT9TexturePtr->height = MRTTexturePtr->height / 512;
  RT10TexturePtr->width = MRTTexturePtr->width / 1024;
  RT10TexturePtr->height = MRTTexturePtr->height / 1024;
  RT11TexturePtr->width = MRTTexturePtr->width / 2048;
  RT11TexturePtr->height = MRTTexturePtr->height / 2048;
  RT12TexturePtr->width = MRTTexturePtr->width / 4096;
  RT12TexturePtr->height = MRTTexturePtr->height / 4096;
  compositorManager->setCompositorEnabled(viewport, MRTCompositor, true);
}

static bool HasNoTangentsAndCanGenerate(VertexDeclaration *vertex_declaration) {
  bool hasTangents = false;
  bool hasUVs = false;
  auto &elementList = vertex_declaration->getElements();
  auto iter = elementList.begin();
  auto end = elementList.end();

  while (iter != end && !hasTangents) {
    const VertexElement &vertexElem = *iter;
    if (vertexElem.getSemantic() == VES_TANGENT) hasTangents = true;
    if (vertexElem.getSemantic() == VES_TEXTURE_COORDINATES) hasUVs = true;

    ++iter;
  }

  return !hasTangents && hasUVs;
}

static void EnsureHasTangents(const MeshPtr &MeshSPtr) {
  bool generateTangents = false;
  if (MeshSPtr->sharedVertexData) {
    VertexDeclaration *vertexDecl = MeshSPtr->sharedVertexData->vertexDeclaration;
    generateTangents |= HasNoTangentsAndCanGenerate(vertexDecl);
  }

  for (unsigned i = 0; i < MeshSPtr->getNumSubMeshes(); ++i) {
    SubMesh *subMesh = MeshSPtr->getSubMesh(i);
    if (subMesh->vertexData) {
      VertexDeclaration *vertexDecl = subMesh->vertexData->vertexDeclaration;
      generateTangents |= HasNoTangentsAndCanGenerate(vertexDecl);
    }
  }

  if (generateTangents) {
    MeshSPtr->buildTangentVectors();
  }
}

static void FixTransparentShadowCaster(const MaterialPtr &material) {
  auto *pass = material->getTechnique(0)->getPass(0);
  int alpha_rejection = static_cast<int>(pass->getAlphaRejectValue());
  bool transparency_casts_shadows = material->getTransparencyCastsShadows();
  int num_textures = pass->getNumTextureUnitStates();
  string MaterialName = material->getName();

  if (num_textures > 0 && alpha_rejection > 0 && transparency_casts_shadows) {
    auto caster_material = MaterialManager::getSingleton().getByName("PSSM/shadow_caster_alpha");
    string caster_name = "PSSM/shadow_caster_alpha/" + MaterialName;
    MaterialPtr new_caster = MaterialManager::getSingleton().getByName(caster_name);

    if (!new_caster) {
      new_caster = caster_material->clone(caster_name);

      auto texture_albedo = pass->getTextureUnitState("Albedo");

      if (texture_albedo) {
        string texture_name = pass->getTextureUnitState("Albedo")->getTextureName();

        auto *texPtr3 = new_caster->getTechnique(0)->getPass(0)->getTextureUnitState("BaseColor");

        if (texPtr3) {
          texPtr3->setContentType(TextureUnitState::CONTENT_NAMED);
          texPtr3->setTextureName(texture_name);
        }
      }
    }

    auto *new_pass = new_caster->getTechnique(0)->getPass(0);
    new_pass->setCullingMode(pass->getCullingMode());
    new_pass->setManualCullingMode(pass->getManualCullingMode());
    new_pass->setAlphaRejectValue(alpha_rejection);

    material->getTechnique(0)->setShadowCasterMaterial(new_caster);
  }
}

static void FixTransparentShadowCaster(const string &material) {
  auto material_ptr = MaterialManager::getSingleton().getByName(material);
  FixTransparentShadowCaster(material_ptr);
}

static void AddMaterial(const MaterialPtr &material) {
  FixTransparentShadowCaster(material);
  GetEngine().AddMaterial(material);
}

static void AddMaterial(const string &material) {
  auto MaterialSPtr = MaterialManager::getSingleton().getByName(material);
  if (MaterialSPtr) AddMaterial(MaterialSPtr);
}

static void AddMeshMaterial(MeshPtr MeshSPtr, const string &MaterialName) {
  try {
    EnsureHasTangents(MeshSPtr);

    for (auto &submesh : MeshSPtr->getSubMeshes()) {
      MaterialPtr material;

      if (!MaterialName.empty()) {
        submesh->setMaterialName(MaterialName);
      }

      material = submesh->getMaterial();
      if (material) AddMaterial(material);
    }
  } catch (Exception &e) {
    LogManager::getSingleton().logMessage(e.getFullDescription());
    LogManager::getSingleton().logMessage("[DotSceneLoader] Error loading an entity!");
  }
}

static void AddMeshMaterial(const string &MeshName, const string &MaterialName = "") {
  const auto &MeshSPtr = MeshManager::getSingleton().getByName(MeshName);
  AddMeshMaterial(MeshSPtr, MaterialName);
}

static void AddEntityMaterial(Entity *EntityPtr, const string &MaterialName = "") {
  try {
    if (!MaterialName.empty()) {
      auto material = MaterialManager::getSingleton().getByName(MaterialName);
      if (material) {
        AddMaterial(material);
        EntityPtr->setMaterial(material);
      }
    }

    AddMeshMaterial(EntityPtr->getMesh(), MaterialName);
  } catch (Exception &e) {
    LogManager::getSingleton().logMessage(e.getFullDescription());
    LogManager::getSingleton().logMessage("[DotSceneLoader] Error loading an entity!");
  }
}

Vector3 Engine::GetSunPosition() {
  auto *SunPtr = sceneManager->getLight("Sun");
  if (SunPtr)
    return -Vector3(SunPtr->getDerivedDirection().normalisedCopy());
  else
    return Vector3::ZERO;
}

float Engine::GetHeight(float x, float z) {
  if (terrainGroup)
    return terrainGroup->getHeightAtWorldPosition(x, 1000, z);
  else
    return 0.0f;
}

void Engine::AddEntity(Entity *EntityPtr) { AddEntityMaterial(EntityPtr); }

void Engine::AddMaterial(MaterialPtr material) {
  const auto *Pass = material->getTechnique(0)->getPass(0);
  if (Pass->hasVertexProgram()) gpuVpParams.push_back(Pass->getVertexProgramParameters());
  if (Pass->hasFragmentProgram()) gpuFpParams.push_back(Pass->getFragmentProgramParameters());
}

void Engine::AddMaterial(const string &MaterialName) {
  auto MaterialPtr = MaterialManager::getSingleton().getByName(MaterialName);
  if (MaterialPtr) AddMaterial(MaterialPtr);
}

void Engine::AddCamera(Camera *OgreCameraPtr) {}

void Engine::AddSinbad(Camera *OgreCameraPtr) {
  sinbad = make_unique<SinbadCharacterController>(OgreCameraPtr);
  InputSequencer::GetInstance().RegObserver(sinbad.get());
}

void Engine::AddForests(Forests::PagedGeometry *PGPtr, const string &MaterialName) {
  pgList.push_back(unique_ptr<Forests::PagedGeometry>(PGPtr));
  if (!MaterialName.empty()) AddMaterial(MaterialName);
}

void Engine::AddTerrain(TerrainGroup *TGP) { terrainGroup.reset(TGP); }

void Engine::AddSkyBox() {
  auto colorspace = sRGB;
  float sunSize = 0.27f;
  float turbidity = 4.0f;
  auto groundAlbedo = Vector3(1.0);
  auto sunColor = Vector3(20000);
  auto sunDir = GetSunPosition();

  SkyModel sky;
  sky.SetupSky(sunDir, sunSize, sunColor, groundAlbedo, turbidity, colorspace);

  const ArHosekSkyModelState *States[3] = {sky.StateX, sky.StateY, sky.StateZ};

  for (int i = 0; i < 9; i++)
    for (int j = 0; j < 3; j++) hosekParams[i][j] = States[j]->configs[j][i];
  hosekParams[9] = Vector3(sky.StateX->radiances[0], sky.StateY->radiances[1], sky.StateZ->radiances[2]);

  auto SkyMaterial = MaterialManager::getSingleton().getByName("SkyBox");
  if (!SkyMaterial) return;

  auto FpParams = SkyMaterial->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
  if (FpParams) skyBoxFpParams = FpParams;

  FpParams->setIgnoreMissingParams(true);
  for (int i = 0; i < 10; i++) FpParams->setNamedConstant(hosekParamList[i], hosekParams[i]);
}

void Engine::RegComponent(SystemI *ComponentPtr) {
  if (ComponentPtr) componentList.push_back(ComponentPtr);
}

void Engine::UnRegComponent(SystemI *ComponentPtr) {
  auto Iter = find(componentList.begin(), componentList.end(), ComponentPtr);
  if (Iter != componentList.end()) componentList.erase(Iter);
}

void Engine::OnPause() {
  for (auto &it : componentList) it->OnPause();
}

void Engine::OnResume() {
  for (auto &it : componentList) it->OnResume();
}

void Engine::OnMenuOn() {
  physics->OnPause();
  ShowMouseCursor(true);
}

void Engine::OnMenuOff() {
  physics->OnResume();
  ShowMouseCursor(false);
}

void Engine::OnCleanup() {
  for (auto &it : componentList) it->OnClean();
  InputSequencer::GetInstance().UnRegObserver(sinbad.get());
  sinbad.reset();
  pgList.clear();
  if (terrainGroup) terrainGroup->removeAllTerrains();
  terrainGroup.reset();
  delete TerrainGlobalOptions::getSingletonPtr();
  if (sceneManager) sceneManager->setShadowTechnique(SHADOWTYPE_NONE);
  if (sceneManager) sceneManager->clearScene();
  ResourceGroupManager::getSingleton().unloadResourceGroup(RGN_DEFAULT);
  gpuFpParams.clear();
  gpuFpParams.shrink_to_fit();
  gpuVpParams.clear();
  gpuVpParams.shrink_to_fit();
  skyBoxFpParams.reset();
}

void Engine::Update(float PassedTime) {
  if (paused) return;
  if (sinbad) sinbad->Update(PassedTime);
  for (auto &it : pgList) it->update();

  static Matrix4 MVP;
  static Matrix4 MVPprev;

  MVPprev = MVP;
  MVP = camera->getProjectionMatrixWithRSDepth() * camera->getViewMatrix();

  for (auto &it : gpuVpParams) it->setNamedConstant("uWorldViewProjPrev", MVPprev);

  if (skyNeedsUpdate && skyBoxFpParams)
    for (int i = 0; i < 10; i++) skyBoxFpParams->setNamedConstant(hosekParamList[i], hosekParams[i]);

  for (auto &it : componentList) it->OnUpdate(PassedTime);

  static ImGuiIO &io = ImGui::GetIO();
  ImGuiOverlay::NewFrame();
  ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Always);
  ImGui::SetNextWindowSize({0, 0}, ImGuiCond_Always);
  ImGui::SetNextWindowCollapsed(false, ImGuiCond_Always);
  ImGui::SetNextWindowBgAlpha(0.5);
  ImGui::Begin("FPS", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0 / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  ImGui::End();
}

void Engine::RenderFrame() {
  ogreRoot->renderOneFrame();
#if defined(WINDOWS) || defined(ANDROID)
  SDL_GL_SwapWindow(sdlWindow);
#endif
}

void Engine::ResizeWindow(int Width, int Height) {
  windowWidth = Width;
  windowHeight = Height;
  SDL_SetWindowPosition(sdlWindow, (screenWidth - windowWidth) / 2, (screenHeight - windowHeight) / 2);
  SDL_SetWindowSize(sdlWindow, windowWidth, windowHeight);
  ogreWindow->resize(Width, Height);
}

void Engine::SetFullscreen(bool Fullscreen) {
  if (Fullscreen) {
    windowFullScreen = true;
    ogreWindow->setFullscreen(windowFullScreen, windowWidth, windowHeight);
#ifdef DESKTOP
    SDL_SetWindowFullscreen(sdlWindow, sdlWindowFlags | SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN_DESKTOP);
#else
    SDL_SetWindowFullscreen(sdlWindow, sdlWindowFlags | SDL_WINDOW_FULLSCREEN);
#endif
  } else {
    windowFullScreen = false;
    ogreWindow->setFullscreen(windowFullScreen, windowWidth, windowHeight);
    SDL_SetWindowFullscreen(sdlWindow, sdlWindowFlags);
    SDL_SetWindowSize(sdlWindow, windowWidth, windowHeight);
  }
}

bool Engine::IsWindowFullscreen() { return windowFullScreen; }

void Engine::SetWindowCaption(const char *Caption) { SDL_SetWindowTitle(sdlWindow, Caption); }

void Engine::GrabCursor(bool Grab) {
#ifndef MOBILE  // This breaks input @Android >9.0
  if (sdlWindow) {
    SDL_ShowCursor(!Grab);
    SDL_SetWindowGrab(sdlWindow, static_cast<SDL_bool>(Grab));
    SDL_SetRelativeMouseMode(static_cast<SDL_bool>(Grab));
  }
#endif
}

void Engine::ShowCursor(bool Show) {
#ifndef MOBILE  // This breaks input @Android >9.0
  if (sdlWindow) SDL_ShowCursor(Show);
#endif
}

string Engine::GetWindowCaption() { return windowCaption; }

int Engine::GetWindowSizeX() { return windowWidth; }

int Engine::GetWindowSizeY() { return windowHeight; }

Vector3 SunDirection() {
  Vector3 Result = -OgreSceneManager()->getLight("Sun")->getParentSceneNode()->getPosition();
  return Result;
}

int WindowSizeX() { return GetEngine().GetWindowSizeX(); }

int WindowSizeY() { return GetEngine().GetWindowSizeY(); }

string WindowCaption() { return GetEngine().GetWindowCaption(); }

}  // namespace Glue
