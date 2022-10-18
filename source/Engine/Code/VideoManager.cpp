/// created by Andrey Vasiliev

#include "pch.h"
#include "VideoManager.h"
#include "DotSceneLoaderB.h"
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
#include <Ogre.h>
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
#endif  // defined(DESKTOP) && defined(DEBUG)

using namespace std;
using namespace Ogre;

namespace Glue {

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

VideoManager::VideoManager() {}
VideoManager::~VideoManager() { SDL_SetWindowFullscreen(sdlWindow, SDL_FALSE); }

void VideoManager::OnUpdate(float time) {
  static Matrix4 MVP;
  static Matrix4 MVPprev;
  MVPprev = MVP;
  MVP = camera->getProjectionMatrixWithRSDepth() * camera->getViewMatrix();
  for (auto &it : gpuVpParams) it->setNamedConstant("uWorldViewProjPrev", MVPprev);
  if (skyNeedsUpdate && skyBoxFpParams)
    for (int i = 0; i < 10; i++) skyBoxFpParams->setNamedConstant(hosekParamList[i], hosekParams[i]);
}

void VideoManager::resizeWindow(int width, int height) {
  windowWidth = width;
  windowHeight = height;
  SDL_SetWindowPosition(sdlWindow, (screenWidth - windowWidth) / 2, (screenHeight - windowHeight) / 2);
  SDL_SetWindowSize(sdlWindow, windowWidth, windowHeight);
  ogreWindow->resize(width, height);
}

void VideoManager::grabCursor(bool grab) {
#ifndef MOBILE  // This breaks input @Android >9.0
  if (sdlWindow) {
    SDL_ShowCursor(!grab);
    SDL_SetWindowGrab(sdlWindow, static_cast<SDL_bool>(grab));
    SDL_SetRelativeMouseMode(static_cast<SDL_bool>(grab));
  }
#endif
}

void VideoManager::showCursor(bool show) {
#ifndef MOBILE  // This breaks input @Android >9.0
  if (sdlWindow) SDL_ShowCursor(show);
#endif
}

void VideoManager::setWindowCaption(const char *caption) {
  windowCaption = caption;
  SDL_SetWindowTitle(sdlWindow, caption);
}

void VideoManager::setFullscreen(bool fullscreen) {
  if (fullscreen) {
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

void VideoManager::renderFrame() {
  ogreRoot->renderOneFrame();
#if defined(WINDOWS) || defined(ANDROID)
  SDL_GL_SwapWindow(sdlWindow);
#endif
}

Vector3 VideoManager::GetSunPosition() {
  auto *SunPtr = sceneManager->getLight("Sun");
  if (SunPtr)
    return -Vector3(SunPtr->getDerivedDirection().normalisedCopy());
  else
    return Vector3::ZERO;
}

void VideoManager::InitSky() {
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

void VideoManager::OnSetUp() {
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

  // cleanup
  imguiOverlay->show();
  ImGuiOverlay::NewFrame();
  ogreRoot->renderOneFrame();
#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
  MaterialManager::getSingleton().removeListener(resolver.get());
#endif

//  InitSky();
}

void VideoManager::OnClean() {
  ResourceGroupManager::getSingleton().unloadResourceGroup(RGN_DEFAULT);
  if (sceneManager) sceneManager->setShadowTechnique(SHADOWTYPE_NONE);
  if (sceneManager) sceneManager->clearScene();
  skyBoxFpParams.reset();
  gpuFpParams.clear();
  gpuFpParams.shrink_to_fit();
  gpuVpParams.clear();
  gpuVpParams.shrink_to_fit();
}

void VideoManager::OnPause() {}

void VideoManager::OnResume() {}

}  // namespace Glue
