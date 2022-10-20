/// created by Andrey Vasiliev

#include "pch.h"
#include "VideoManager.h"
#include "Android.h"
#include "DotSceneLoaderB.h"
extern "C" {
#ifdef _MSC_VER
#define SDL_MAIN_HANDLED
#endif
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
}
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
#ifdef OGRE_BUILD_COMPONENT_OVERLAY
#include <Overlay/OgreFontManager.h>
#include <Overlay/OgreImGuiOverlay.h>
#include <Overlay/OgreOverlayManager.h>
#endif

using namespace std;
using namespace Ogre;

namespace Glue {

inline bool RenderSystemIsGL() { return Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL Rendering Subsystem"; };
inline bool RenderSystemIsGL3() { return Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL 3+ Rendering Subsystem"; };
inline bool RenderSystemIsGLES2() { return Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL ES 2.x Rendering Subsystem"; };
inline Ogre::SceneManager *OgreSceneManager() { return Ogre::Root::getSingleton().getSceneManager("Default"); }
inline Ogre::SceneNode *OgreRootNode() { return OgreSceneManager()->getRootSceneNode(); }
inline Ogre::Camera *OgreCamera() { return OgreSceneManager()->getCamera("Default"); }
inline Ogre::SceneNode *OgreCameraNode() { return OgreCamera()->getParentSceneNode(); }
inline Vector3 SunDirection() { return -OgreSceneManager()->getLight("Sun")->getParentSceneNode()->getPosition(); }
#ifdef OGRE_BUILD_RENDERSYSTEM_GL3PLUS
void InitOgreRenderSystemGL3();
#endif
#ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
void InitOgreRenderSystemGLES2();
#endif
#ifdef OGRE_BUILD_RENDERSYSTEM_GL
void InitOgreRenderSystemGL();
#endif

Window::Window(const std::string &caption)
    : sdlFlags(0), caption(caption), vsync(true), width(1024), height(728), fullscreen(false), ogreWindow(nullptr), renderTarget(nullptr) {
  if (caption.empty()) this->caption = "Example0";
}

Window::~Window() { SDL_SetWindowFullscreen(sdlWindow, SDL_FALSE); }

void Window::Create(int monitor, bool fullscreen, int width, int height) {
  this->width = width;
  this->height = height;
  this->fullscreen = fullscreen;

  // select biggest display
  SDL_DisplayMode displayMode;
  int screenWidth, screenHeight, currentDisplay;
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
  int32_t sdlWindowPositionFlag = SDL_WINDOWPOS_CENTERED_DISPLAY(currentDisplay);
  sdlWindow = SDL_CreateWindow(caption.c_str(), sdlWindowPositionFlag, sdlWindowPositionFlag, width, height, sdlFlags);
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
  sdlWindowFlags |= SDL_WINDOW_OPENGL;
#ifdef EMSCRIPTEN
  sdlWindowFlags |= SDL_WINDOW_RESIZABLE;
#endif
  sdlWindowFlags |= SDL_WINDOW_ALLOW_HIGHDPI;
#ifdef MOBILE
  sdlWindowFlags |= SDL_WINDOW_BORDERLESS;
  sdlWindowFlags |= SDL_WINDOW_FULLSCREEN;
#endif
  width = screenWidth;
  windowHeight = screenHeight;
  sdlWindowPositionFlag = SDL_WINDOWPOS_CENTERED;
  sdlWindow = SDL_CreateWindow(nullptr, sdlWindowPositionFlag, sdlWindowPositionFlag, screenWidth, screenHeight, sdlWindowFlags);
  SDL_GL_CreateContext(sdlWindow);
#endif

  NameValuePairList renderParams;
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
#ifdef ANDROID
  SDL_DisplayMode displayMode;
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
    SDL_SetWindowFullscreen(sdlWindow, sdlWindowFlags | SDL_WINDOW_FULLSCREEN);
#endif
  } else {
    fullscreen = false;
    ogreWindow->setFullscreen(fullscreen, width, height);
    SDL_SetWindowFullscreen(sdlWindow, sdlFlags);
    SDL_SetWindowSize(sdlWindow, width, height);
  }
}

void Window::Delete() {}

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
VideoManager::~VideoManager() {}

void VideoManager::OnSetUp() {
  InitOgreRoot();

  InitSDL();

  windowList.emplace_back("Example0");
  windowList[0].Create(-1, false, 1024, 768);
  CheckGPU();

  ogreCamera = sceneManager->createCamera("Default");
  ogreViewport = windowList[0].renderTarget->addViewport(ogreCamera);
  ogreCamera->setAspectRatio(static_cast<float>(ogreViewport->getActualWidth()) / static_cast<float>(ogreViewport->getActualHeight()));
  ogreCamera->setAutoAspectRatio(true);

// RTSS init
#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
  OgreAssert(RTShader::ShaderGenerator::initialize(), "OGRE RTSS init failed");
  auto *shaderGenerator = RTShader::ShaderGenerator::getSingletonPtr();
  ogreViewport->setMaterialScheme(MSN_SHADERGEN);
  shaderGenerator->addSceneManager(sceneManager);
  shaderGenerator->setShaderCachePath("");
  auto resolver = make_unique<ShaderResolver>(shaderGenerator);
  MaterialManager::getSingleton().addListener(resolver.get());
#endif

  // overlay init
  auto *ogreOverlay = new OverlaySystem();
  imguiOverlay = new ImGuiOverlay();
  float vpScale = OverlayManager::getSingleton().getPixelRatio();
  ImGui::GetIO().FontGlobalScale = round(vpScale);
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

  // scan and load resources
#ifdef DESKTOP
  TextureManager::getSingleton().setDefaultNumMipmaps(MIP_UNLIMITED);
  if (Root::getSingleton().getRenderSystem()->getCapabilities()->hasCapability(RSC_ANISOTROPY)) {
    MaterialManager::getSingleton().setDefaultTextureFiltering(TFO_ANISOTROPIC);
    MaterialManager::getSingleton().setDefaultAnisotropy(8);
  } else {
    MaterialManager::getSingleton().setDefaultTextureFiltering(TFO_BILINEAR);
  }
#endif
  auto &RGM = ResourceGroupManager::getSingleton();
#if defined(ANDROID)
  RGM.addResourceLocation("programs.zip", "APKZip", RGN_INTERNAL);
  RGM.addResourceLocation("assets.zip", "APKZip", RGN_DEFAULT);
#elif defined(DESKTOP) && defined(DEBUG)
  const char *PROGRAMS_DIR = "source/Programs";
  const char *ASSETS_DIR = "source/Example/Assets";
  ScanLocation(PROGRAMS_DIR, RGN_INTERNAL);
  if (RenderSystemIsGLES2())
    ScanLocation("source/GLSLES", RGN_INTERNAL);
  else
    ScanLocation("source/GLSL", RGN_INTERNAL);
  ScanLocation(ASSETS_DIR);
#else
  RGM.addResourceLocation("programs.zip", "Zip", RGN_INTERNAL);
  RGM.addResourceLocation("assets.zip", "Zip", RGN_DEFAULT);
#endif
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
}

void VideoManager::OnUpdate(float time) {
  static Matrix4 MVP;
  static Matrix4 MVPprev;
  MVPprev = MVP;
  MVP = ogreCamera->getProjectionMatrixWithRSDepth() * ogreCamera->getViewMatrix();
  for (auto &it : gpuVpParams) it->setNamedConstant("uWorldViewProjPrev", MVPprev);
  if (skyNeedsUpdate && skyBoxFpParams)
    for (int i = 0; i < 10; i++) skyBoxFpParams->setNamedConstant(hosekParamList[i], hosekParams[i]);
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

void VideoManager::RenderFrame() {
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

bool CheckGLVersion(int major, int minor) {
  return dynamic_cast<Ogre::GLRenderSystemCommon *>(Ogre::Root::getSingleton().getRenderSystem())->hasMinGLVersion(major, minor);
}

void VideoManager::CheckGPU() {
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
  for (int i = 0; i < hosekParamList.size(); i++) FpParams->setNamedConstant(hosekParamList[i], hosekParams[i]);
}

void VideoManager::InitSDL() {
  // init SDL subsystems
#ifndef EMSCRIPTEN
  OgreAssert(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER), "Failed to init SDL2");
  for (int i = 0; i < SDL_NumJoysticks(); i++) {
    if (SDL_IsGameController(i)) SDL_GameControllerOpen(i);
  }
#else
  OgreAssert(!SDL_Init(SDL_INIT_VIDEO), "Failed to init SDL2");
#endif
}

void VideoManager::InitOgreRoot() {
  ogreRoot = new Root("", "", "");
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
  ArchiveManager::getSingleton().addArchiveFactory(new APKFileSystemArchiveFactory(assetManager));
  ArchiveManager::getSingleton().addArchiveFactory(new APKZipArchiveFactory(assetManager));
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
}

}  // namespace Glue
