/// created by Andrey Vasiliev

#include "pch.h"
#include "VideoManager.h"
#include "Android.h"
#include "DotSceneLoaderB.h"
#include "FSHelpers.h"
#include "ImguiHelpers.h"
#include "SkyModel/ArHosekSkyModel.h"
#include "SkyModel/SkyModel.h"
#include <Ogre.h>
#ifdef OGRE_OPENGL
#include <OgreGLRenderSystemCommon.h>
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
#endif
#ifdef OGRE_BUILD_PLUGIN_ASSIMP
#include <Plugins/Assimp/OgreAssimpLoader.h>
#endif
#ifdef OGRE_BUILD_PLUGIN_DOT_SCENE
#include <Plugins/DotScene/OgreDotSceneLoader.h>
#endif
#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
#include <RTShaderSystem/OgreRTShaderSystem.h>
#endif
#ifdef OGRE_BUILD_COMPONENT_TERRAIN
#include "TerrainMaterialGeneratorB.h"
#include <Terrain/OgreTerrainGroup.h>
#endif
#ifdef OGRE_BUILD_COMPONENT_OVERLAY
#include <Overlay/OgreFontManager.h>
#include <Overlay/OgreImGuiOverlay.h>
#include <Overlay/OgreOverlayManager.h>
#endif
extern "C" {
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
}
#ifdef _WIN32
#undef CreateWindow
#endif

using namespace std;

namespace Glue {

inline bool RenderSystemIsGL() { return Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL Rendering Subsystem"; };
inline bool RenderSystemIsGL3() { return Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL 3+ Rendering Subsystem"; };
inline bool RenderSystemIsGLES2() { return Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL ES 2.x Rendering Subsystem"; };
inline Ogre::SceneManager *OgreSceneManager() { return Ogre::Root::getSingleton().getSceneManager("Default"); }
inline Ogre::SceneNode *OgreRootNode() { return OgreSceneManager()->getRootSceneNode(); }
inline Ogre::Camera *OgreCamera() { return OgreSceneManager()->getCamera("Default"); }
inline Ogre::SceneNode *OgreCameraNode() { return OgreCamera()->getParentSceneNode(); }
inline Ogre::Vector3 SunDirection() { return -OgreSceneManager()->getLight("Sun")->getParentSceneNode()->getPosition(); }
#ifdef OGRE_BUILD_RENDERSYSTEM_GL3PLUS
void InitOgreRenderSystemGL3();
#endif
#ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
void InitOgreRenderSystemGLES2();
#endif
#ifdef OGRE_BUILD_RENDERSYSTEM_GL
void InitOgreRenderSystemGL();
#endif

Window::Window(const std::string &caption) : sdlFlags(0), caption(caption), vsync(true), width(1270), height(720), fullscreen(false) {
  if (caption.empty()) this->caption = "Example0";
}

Window::~Window() { SDL_SetWindowFullscreen(sdlWindow, SDL_FALSE); }

void Window::Create(Ogre::Camera *ogreCamera, int monitor, bool fullscreen, int width, int height) {
  this->width = width;
  this->height = height;
  this->fullscreen = fullscreen;
  this->ogreCamera = ogreCamera;

  // select biggest display
  SDL_DisplayMode displayMode;
  int screenWidth = 0, screenHeight = 0, currentDisplay = 0;
  for (int i = 0; i < SDL_GetNumVideoDisplays(); i++) {
    if (SDL_GetCurrentDisplayMode(i, &displayMode) == 0) {
      SDL_Log("Display #%d: current display mode is %dx%dpx @ %dhz.", i, displayMode.w, displayMode.h, displayMode.refresh_rate);
      int screenDiag = sqrt(screenWidth * screenWidth + screenHeight * screenHeight);
      int screenDiagI = sqrt(displayMode.w * displayMode.w + displayMode.h * displayMode.h);
      if (screenDiagI > screenDiag) {
        screenWidth = displayMode.w;
        screenHeight = displayMode.h;
        currentDisplay = i;
      } else {
        SDL_Log("Could not get display mode for video display #%d: %s", i, SDL_GetError());
      }
    }
  }

#if defined(DESKTOP)
  if (width == screenWidth && height == screenHeight) {
    sdlFlags |= SDL_WINDOW_BORDERLESS;
  }
  if (fullscreen) {
    sdlFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    sdlFlags |= SDL_WINDOW_BORDERLESS;
    width = screenWidth;
    height = screenHeight;
  }
  int32_t sdlPositionFlags = SDL_WINDOWPOS_CENTERED_DISPLAY(currentDisplay);
  sdlWindow = SDL_CreateWindow(caption.c_str(), sdlPositionFlags, sdlPositionFlags, width, height, sdlFlags);
  OgreAssert(sdlWindow, "SDL_CreateWindow failed");
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
  sdlFlags |= SDL_WINDOW_OPENGL;
#ifdef EMSCRIPTEN
  sdlFlags |= SDL_WINDOW_RESIZABLE;
#endif
  sdlFlags |= SDL_WINDOW_ALLOW_HIGHDPI;
#ifdef MOBILE
  sdlFlags |= SDL_WINDOW_BORDERLESS;
  sdlFlags |= SDL_WINDOW_FULLSCREEN;
#endif
  width = screenWidth;
  height = screenHeight;
  int32_t sdlPositionFlags = SDL_WINDOWPOS_CENTERED;
  sdlWindow = SDL_CreateWindow(nullptr, sdlPositionFlags, sdlPositionFlags, screenWidth, screenHeight, sdlFlags);
  SDL_GL_CreateContext(sdlWindow);
#endif

  Ogre::NameValuePairList renderParams;
  SDL_SysWMinfo info;
  SDL_VERSION(&info.version);
  SDL_GetWindowWMInfo(sdlWindow, &info);
  const char *TRUE_STR = "true";
  const char *FALSE_STR = "false";
  renderParams["vsync"] = vsync ? TRUE_STR : FALSE_STR;
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  renderParams["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(info.info.win.window));
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
  renderParams["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(info.info.x11.window));
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
  renderParams["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(info.info.cocoa.window));
#elif OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  renderParams["currentGLContext"] = TRUE_STR;
  renderParams["externalGLControl"] = TRUE_STR;
  renderParams["preserveContext"] = TRUE_STR;
  renderParams["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(info.info.android.window));
#endif
  auto &ogreRoot = Ogre::Root::getSingleton();
  ogreWindow = ogreRoot.createRenderWindow(caption, width, height, fullscreen, &renderParams);
  renderTarget = ogreRoot.getRenderTarget(ogreWindow->getName());
  ogreViewport = renderTarget->addViewport(ogreCamera);
  ogreCamera->setAspectRatio(static_cast<float>(ogreViewport->getActualWidth()) / static_cast<float>(ogreViewport->getActualHeight()));
  ogreCamera->setAutoAspectRatio(true);
#ifdef ANDROID
  SDL_GetDesktopDisplayMode(currentDisplay, &displayMode);
  ogreWindow->resize(static_cast<int>(displayMode.w), static_cast<int>(displayMode.h));
#endif
}

void Window::Resize(int width, int height) {
  width = width;
  height = height;
  SDL_SetWindowSize(sdlWindow, width, height);
  ogreWindow->resize(width, height);
}

void Window::GrabCursor(bool grab) {
#ifndef MOBILE  // This breaks input @Android >9.0
  if (sdlWindow) {
    SDL_ShowCursor(!grab);
    SDL_SetWindowGrab(sdlWindow, static_cast<SDL_bool>(grab));
    SDL_SetRelativeMouseMode(static_cast<SDL_bool>(grab));
    ShowCursor(!grab);
  }
#endif
}

void Window::ShowCursor(bool show) {
#ifndef MOBILE  // This breaks input @Android >9.0
  if (sdlWindow) SDL_ShowCursor(show);
#endif
}

void Window::SetWindowCaption(const char *caption) {
  this->caption = caption;
  SDL_SetWindowTitle(sdlWindow, caption);
}

void Window::SetFullscreen(bool fullscreen) {
  if (fullscreen) {
    fullscreen = true;
    ogreWindow->setFullscreen(fullscreen, width, height);
#ifdef DESKTOP
    SDL_SetWindowFullscreen(sdlWindow, sdlFlags | SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN_DESKTOP);
#else
    SDL_SetWindowFullscreen(sdlWindow, sdlFlags | SDL_WINDOW_FULLSCREEN);
#endif
  } else {
    fullscreen = false;
    ogreWindow->setFullscreen(fullscreen, width, height);
    SDL_SetWindowFullscreen(sdlWindow, sdlFlags);
    SDL_SetWindowSize(sdlWindow, width, height);
  }
}

void Window::Delete() {}

class VideoManager::ShaderResolver final : public Ogre::MaterialManager::Listener {
 public:
  explicit ShaderResolver(Ogre::RTShader::ShaderGenerator *shaderGenerator) : shaderGenerator(shaderGenerator) {}

  Ogre::Technique *handleSchemeNotFound(unsigned short schemeIndex, const string &schemeName, Ogre::Material *originalMaterial,
                                        unsigned short lodIndex, const Ogre::Renderable *rend) override {
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

  bool afterIlluminationPassesCreated(Ogre::Technique *tech) override {
    if (shaderGenerator->hasRenderState(tech->getSchemeName())) {
      Ogre::Material *mat = tech->getParent();
      shaderGenerator->validateMaterialIlluminationPasses(tech->getSchemeName(), mat->getName(), mat->getGroup());
      return true;
    }
    return false;
  }

  bool beforeIlluminationPassesCleared(Ogre::Technique *tech) override {
    if (shaderGenerator->hasRenderState(tech->getSchemeName())) {
      Ogre::Material *mat = tech->getParent();
      shaderGenerator->invalidateMaterialIlluminationPasses(tech->getSchemeName(), mat->getName(), mat->getGroup());
      return true;
    }
    return false;
  }

 protected:
  Ogre::RTShader::ShaderGenerator *shaderGenerator = nullptr;
};

VideoManager::VideoManager() : ogreMinLogLevel(Ogre::LML_TRIVIAL), ogreLogFile("Ogre.log"), shadowTechnique(Ogre::SHADOWTYPE_NONE), pssmSplitCount(3) {}
VideoManager::~VideoManager() { SDL_Quit(); }

void VideoManager::OnSetUp() {
  // init
  InitOgreRoot();
  InitSDL();
  CreateWindow();
  CheckGPU();
  InitOgreRTSS();
  InitOgreOverlay();
  LoadResources();
  InitOgreScene();

  // cleanup
  imguiOverlay->show();
  Ogre::ImGuiOverlay::NewFrame();
  ogreRoot->renderOneFrame();
  Ogre::MaterialManager::getSingleton().removeListener(shaderResolver);
  delete shaderResolver;
}

void VideoManager::OnUpdate(float time) {
  static Ogre::Matrix4 MVP;
  static Ogre::Matrix4 MVPprev;
  MVPprev = MVP;
  MVP = ogreCamera->getProjectionMatrixWithRSDepth() * ogreCamera->getViewMatrix();
  for (auto &it : gpuVpParams) it->setNamedConstant("uWorldViewProjPrev", MVPprev);
  if (skyNeedsUpdate && skyBoxFpParams)
    for (int i = 0; i < 10; i++) skyBoxFpParams->setNamedConstant(hosekParamList[i], hosekParams[i]);
}

void VideoManager::OnClean() {
  Ogre::ResourceGroupManager::getSingleton().unloadResourceGroup(Ogre::RGN_DEFAULT);
  if (sceneManager) sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
  if (sceneManager) sceneManager->clearScene();
  skyBoxFpParams.reset();
  gpuFpParams.clear();
  gpuFpParams.shrink_to_fit();
  gpuVpParams.clear();
  gpuVpParams.shrink_to_fit();
}

void VideoManager::OnPause() {}

void VideoManager::OnResume() {}

void VideoManager::RenderFrame() {
  ogreRoot->renderOneFrame();
#if defined(WINDOWS) || defined(ANDROID)
  SDL_GL_SwapWindow(windowList[0].sdlWindow);
#endif
}

Ogre::Vector3 VideoManager::GetSunPosition() {
  auto *SunPtr = sceneManager->getLight("Sun");
  if (SunPtr)
    return -Ogre::Vector3(SunPtr->getDerivedDirection().normalisedCopy());
  else
    return Ogre::Vector3::ZERO;
}

bool CheckGLVersion(int major, int minor) {
  return dynamic_cast<Ogre::GLRenderSystemCommon *>(Ogre::Root::getSingleton().getRenderSystem())->hasMinGLVersion(major, minor);
}

void VideoManager::CheckGPU() {
  const auto *RSC = Ogre::Root::getSingleton().getRenderSystem()->getCapabilities();
  OgreAssert(RSC->hasCapability(Ogre::RSC_HWRENDER_TO_TEXTURE), "Render to texture support required");
  OgreAssert(RSC->hasCapability(Ogre::RSC_TEXTURE_FLOAT), "Float texture support required");
  OgreAssert(RSC->hasCapability(Ogre::RSC_TEXTURE_COMPRESSION), "Texture compression support required");
#if defined(DESKTOP)
  OgreAssert(RSC->hasCapability(Ogre::RSC_TEXTURE_COMPRESSION_DXT), "DXT compression support required");
#elif defined(ANDROID)
  OgreAssert(RSC->hasCapability(Ogre::RSC_TEXTURE_COMPRESSION_ETC1), "ETC1 compression support required");
#elif defined(IOS)
  OgreAssert(RSC->hasCapabilityOgre::(RSC_TEXTURE_COMPRESSION_PVRTC), "PVRTC compression support required");
#endif
  if (RenderSystemIsGL3()) {
#ifdef OGRE_BUILD_RENDERSYSTEM_GL3PLUS
    OgreAssert(CheckGLVersion(3, 3), "OpenGL 3.3 is not supported");
#endif
  } else if (RenderSystemIsGLES2()) {
#ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
    OgreAssert(CheckGLVersion(3, 0), "OpenGLES 3.0 is not supported");
#endif
  }
}

void VideoManager::InitSky() {
  auto colorspace = ColorSpace::sRGB;
  float sunSize = 0.27f;
  float turbidity = 4.0f;
  auto groundAlbedo = Ogre::Vector3(1.0);
  auto sunColor = Ogre::Vector3(20000);
  auto sunDir = GetSunPosition();

  SkyModel sky;
  sky.SetupSky(sunDir, sunSize, sunColor, groundAlbedo, turbidity, colorspace);

  const ArHosekSkyModelState *States[3] = {sky.StateX, sky.StateY, sky.StateZ};

  for (int i = 0; i < 9; i++)
    for (int j = 0; j < 3; j++) hosekParams[i][j] = States[j]->configs[j][i];
  hosekParams[9] = Ogre::Vector3(sky.StateX->radiances[0], sky.StateY->radiances[1], sky.StateZ->radiances[2]);

  auto SkyMaterial = Ogre::MaterialManager::getSingleton().getByName("SkyBox");
  if (!SkyMaterial) return;

  auto FpParams = SkyMaterial->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
  if (FpParams) skyBoxFpParams = FpParams;

  FpParams->setIgnoreMissingParams(true);
  for (int i = 0; i < hosekParamList.size(); i++) FpParams->setNamedConstant(hosekParamList[i], hosekParams[i]);
}

void VideoManager::InitSDL() {
#ifdef _MSC_VER
  SDL_SetMainReady();
#endif
#ifndef EMSCRIPTEN
  OgreAssert(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER), "Failed to init SDL");
  for (int i = 0; i < SDL_NumJoysticks(); i++) {
    if (SDL_IsGameController(i)) SDL_GameControllerOpen(i);
  }
#else
  OgreAssert(!SDL_Init(SDL_INIT_VIDEO), "Failed to init SDL");
#endif
}

void VideoManager::InitOgreRoot() {
  ogreLogFile = "runtime.log";
#ifdef DEBUG
  ogreMinLogLevel = Ogre::LML_TRIVIAL;
#else
  ogreMinLogLevel = Ogre::LML_WARNING;
#endif
  ogreRoot = new Ogre::Root("", "", ogreLogFile.c_str());
  Ogre::LogManager::getSingleton().setMinLogLevel(static_cast<Ogre::LogMessageLevel>(ogreMinLogLevel));
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
  Ogre::ArchiveManager::getSingleton().addArchiveFactory(new Ogre::APKFileSystemArchiveFactory(assetManager));
  Ogre::ArchiveManager::getSingleton().addArchiveFactory(new Ogre::APKZipArchiveFactory(assetManager));
#endif
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
  sceneManager = ogreRoot->createSceneManager("OctreeSceneManager", "Default");
#else
  sceneManager = OgreRoot->createSceneManager(Ogre::ST_GENERIC, "Default");
#endif
#ifdef OGRE_BUILD_PLUGIN_PFX
  Ogre::Root::getSingleton().installPlugin(new Ogre::ParticleFXPlugin());
#endif
#ifdef OGRE_BUILD_PLUGIN_STBI
  Ogre::Root::getSingleton().installPlugin(new Ogre::STBIPlugin());
#endif
#if defined(OGRE_BUILD_PLUGIN_FREEIMAGE)
  Root::getSingleton().installPlugin(new Ogre::FreeImagePlugin());
#endif
#if defined(OGRE_BUILD_PLUGIN_ASSIMP)
  Root::getSingleton().installPlugin(new Ogre::AssimpPlugin());
#endif
#ifdef OGRE_BUILD_PLUGIN_DOT_SCENE
  Root::getSingleton().installPlugin(new Ogre::DotScenePluginB());
#else
  Ogre::Root::getSingleton().installPlugin(new Ogre::DotScenePluginB());
#endif
#ifdef OGRE_BUILD_COMPONENT_TERRAIN
  auto *terrainGlobalOption = Ogre::TerrainGlobalOptions::getSingletonPtr();
  if (!terrainGlobalOption) terrainGlobalOption = new Ogre::TerrainGlobalOptions();
  terrainGlobalOption->setDefaultMaterialGenerator(make_shared<Ogre::TerrainMaterialGeneratorB>());
  terrainGlobalOption->setUseRayBoxDistanceCalculation(true);
#endif
  ogreRoot->initialise(false);
}

void VideoManager::CreateWindow() {
  windowList.emplace_back("Example0");
  ogreCamera = sceneManager->createCamera("Default");
  windowList[0].Create(ogreCamera, -1, false, 1270, 720);
  ogreViewport = windowList[0].ogreViewport;
}

void VideoManager::InitOgreRTSS() {
  OgreAssert(Ogre::RTShader::ShaderGenerator::initialize(), "OGRE RTSS init failed");
  auto *shaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
  ogreViewport->setMaterialScheme(Ogre::MSN_SHADERGEN);
  shaderGenerator->addSceneManager(sceneManager);
  shaderGenerator->setShaderCachePath("");
  shaderResolver = new ShaderResolver(shaderGenerator);
  Ogre::MaterialManager::getSingleton().addListener(shaderResolver);
}

void VideoManager::InitOgreOverlay() {
  auto *ogreOverlay = new Ogre::OverlaySystem();
  imguiOverlay = new Ogre::ImGuiOverlay();
  float vpScale = Ogre::OverlayManager::getSingleton().getPixelRatio();
  ImGui::GetIO().FontGlobalScale = round(vpScale);
  ImGui::GetStyle().ScaleAllSizes(vpScale);
  imguiOverlay->setZOrder(300);
  Ogre::OverlayManager::getSingleton().addOverlay(imguiOverlay);
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
}

void VideoManager::LoadResources() {
#ifdef DESKTOP
  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(Ogre::MIP_UNLIMITED);
  if (Ogre::Root::getSingleton().getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_ANISOTROPY)) {
    Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
    Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(8);
  } else {
    Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_BILINEAR);
  }
#endif
  auto &RGM = Ogre::ResourceGroupManager::getSingleton();
#if defined(ANDROID)
  RGM.addResourceLocation("programs.zip", "APKZip", Ogre::RGN_INTERNAL);
  RGM.addResourceLocation("assets.zip", "APKZip", Ogre::RGN_DEFAULT);
#elif defined(DESKTOP) && defined(DEBUG)
  const char *PROGRAMS_DIR = "source/Programs";
  const char *ASSETS_DIR = "source/Example/Assets";
  ScanLocation(PROGRAMS_DIR, Ogre::RGN_INTERNAL);
  if (RenderSystemIsGLES2())
    ScanLocation("source/GLSLES", Ogre::RGN_INTERNAL);
  else
    ScanLocation("source/GLSL", Ogre::RGN_INTERNAL);
  ScanLocation(ASSETS_DIR);
#else
  RGM.addResourceLocation("programs.zip", "Zip", Ogre::RGN_INTERNAL);
  RGM.addResourceLocation("assets.zip", "Zip", Ogre::RGN_DEFAULT);
#endif
  RGM.initialiseResourceGroup(Ogre::RGN_INTERNAL);
  RGM.initialiseAllResourceGroups();
}

void VideoManager::InitOgreScene() {
#ifdef DESKTOP
  bool ShadowsEnabled = true;
#else
  bool ShadowsEnabled = false;
#endif
  if (!ShadowsEnabled) {
    sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
    sceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, 0);
    sceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_SPOTLIGHT, 0);
    sceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_POINT, 0);
  } else {
#ifdef DESKTOP
    Ogre::PixelFormat ShadowTextureFormat = Ogre::PixelFormat::PF_DEPTH16;
#else
    Ogre::PixelFormat ShadowTextureFormat = Ogre::PixelFormat::PF_FLOAT16_R;
#endif
    shadowFarDistance = 400;
    shadowTexSize = 1024;
    sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
    sceneManager->setShadowFarDistance(shadowFarDistance);
    sceneManager->setShadowTextureSize(shadowTexSize);
    sceneManager->setShadowTexturePixelFormat(ShadowTextureFormat);
    sceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, 3);
    sceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_SPOTLIGHT, 1);
    sceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_POINT, 0);
    sceneManager->setShadowTextureSelfShadow(true);
    sceneManager->setShadowCasterRenderBackFaces(true);
    sceneManager->setShadowFarDistance(shadowFarDistance);
    auto passCaterMaterial = Ogre::MaterialManager::getSingleton().getByName("PSSM/shadow_caster");
    sceneManager->setShadowTextureCasterMaterial(passCaterMaterial);
    pssmSetup = make_shared<Ogre::PSSMShadowCameraSetup>();
    pssmSetup->calculateSplitPoints(pssmSplitCount, 0.001, sceneManager->getShadowFarDistance());
    pssmSplitPointList = pssmSetup->getSplitPoints();
    pssmSetup->setSplitPadding(0.0);
    for (int i = 0; i < pssmSplitCount; i++) pssmSetup->setOptimalAdjustFactor(i, static_cast<float>(0.5 * i));
    sceneManager->setShadowCameraSetup(pssmSetup);
    sceneManager->setShadowColour(Ogre::ColourValue::Black);
    auto *shaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
    auto *schemRenderState = shaderGenerator->getRenderState(Ogre::MSN_SHADERGEN);
    auto subRenderState = shaderGenerator->createSubRenderState<Ogre::RTShader::IntegratedPSSM3>();
    subRenderState->setSplitPoints(pssmSplitPointList);
    schemRenderState->addTemplateSubRenderState(subRenderState);
  }
}

}  // namespace Glue
