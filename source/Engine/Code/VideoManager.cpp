/// created by Andrey Vasiliev

#include "pch.h"
#include "VideoManager.h"
#include "Android.h"
#include "DotSceneLoaderB/DotSceneLoaderB.h"
#include "ImguiHelpers.h"
#include "Platform.h"
#include <Ogre.h>
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
#endif
#ifdef OGRE_BUILD_COMPONENT_OVERLAY
#include <Overlay/OgreFontManager.h>
#include <Overlay/OgreImGuiOverlay.h>
#include <Overlay/OgreOverlayManager.h>
#endif
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
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
#ifdef APPLE
#include <mach-o/dyld.h>
#endif
#ifdef LINUX
#include <unistd.h>
#endif
#ifdef WINDOWS
#include <windows.h>
#undef CreateWindow
#endif
#endif  // DESKTOP

using namespace std;

namespace {

inline bool RenderSystemIsGL() { return Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL Rendering Subsystem"; };
inline bool RenderSystemIsGL3() { return Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL 3+ Rendering Subsystem"; };
inline bool RenderSystemIsGLES2() { return Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL ES 2.x Rendering Subsystem"; };

#ifdef DESKTOP

// based on twnsorflow GetBinaryDir
// https://github.com/tensorflow/tensorflow/blob/e895d5ca395c2362df4f5c8f08b68501b41f8a98/tensorflow/stream_executor/cuda/cuda_gpu_executor.cc#L202
std::string GetBinaryDir() {
#ifndef MAX_PATH
#define MAX_PATH 4096
#endif
  char buffer[MAX_PATH] = {0};
#ifdef APPLE
  uint32_t bufferSize = 0;
  _NSGetExecutablePath(nullptr, &bufferSize);
  char unresolvedPath[bufferSize];
  _NSGetExecutablePath(unresolvedPath, &bufferSize);
  OgreAssert(realpath(unresolvedPath, buffer), "macos realpath failed");
#elif defined(WINDOWS)
  GetModuleFileNameA(NULL, buffer, MAX_PATH);
#elif defined(LINUX)
  OgreAssert(readlink("/proc/self/exe", buffer, MAX_PATH), "linux readlink failed");
#endif
  buffer[MAX_PATH - 1] = 0;
  auto pos = std::string(buffer).find_last_of("\\/");
  return std::string(buffer).substr(0, pos + 1);
}

inline std::string FindPath(const std::string &path, int depth) {
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
  OgreAssert(fs::is_directory(path), "path is not directory");
  auto &ogreResourceManager = Ogre::ResourceGroupManager::getSingleton();
  ogreResourceManager.addResourceLocation(path, FILE_SYSTEM, groupName);

  for (fs::recursive_directory_iterator end, it(path); it != end; ++it) {
    const std::string fullPath = it->path().string();
    const std::string fileExtention = it->path().filename().extension().string();

    if (it->is_directory())
      ogreResourceManager.addResourceLocation(fullPath, FILE_SYSTEM, groupName);
    else if (it->is_regular_file() && fileExtention == ".zip")
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

Window::Window() : sdlFlags(SDL_WINDOW_HIDDEN), vsync(true), width(1270), height(720), fullscreen(false) {}

Window::~Window() { SDL_SetWindowFullscreen(sdlWindow, SDL_FALSE); }

void Window::Create(const string &caption, Ogre::Camera *camera, int monitor, bool fullscreen, int width, int height) {
  this->caption = caption;
  this->width = width;
  this->height = height;
  this->fullscreen = fullscreen;
  this->ogreCamera = camera;

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
      }
    } else {
      SDL_Log("Could not get display mode for video display #%d: %s", i, SDL_GetError());
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
  const char *TRUE_STR = "true";
  const char *FALSE_STR = "false";
  SDL_SysWMinfo info;
  SDL_VERSION(&info.version);
  SDL_GetWindowWMInfo(sdlWindow, &info);
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
  auto *ogreRoot = Ogre::Root::getSingletonPtr();
  OgreAssert(ogreRoot, "ogreRoot not initialised");
  ogreWindow = ogreRoot->createRenderWindow(caption, width, height, fullscreen, &renderParams);
  renderTarget = ogreRoot->getRenderTarget(ogreWindow->getName());
  ogreViewport = renderTarget->addViewport(ogreCamera);
  ogreCamera->setAspectRatio(static_cast<float>(ogreViewport->getActualWidth()) / static_cast<float>(ogreViewport->getActualHeight()));
  ogreCamera->setAutoAspectRatio(true);
#ifdef ANDROID
  SDL_GetDesktopDisplayMode(currentDisplay, &displayMode);
  ogreWindow->resize(static_cast<int>(displayMode.w), static_cast<int>(displayMode.h));
#endif
}

void Window::Show(bool show) { show ? SDL_ShowWindow(sdlWindow) : SDL_HideWindow(sdlWindow); }

void Window::Resize(int width, int height) {
  width = width;
  height = height;
  SDL_SetWindowSize(sdlWindow, width, height);
  ogreWindow->resize(width, height);
}

void Window::GrabCursor(bool grab) {
#ifndef MOBILE  // This breaks input @Android >9.0
  OgreAssert(sdlWindow, "sdlWindow not initialised");
  SDL_SetWindowGrab(sdlWindow, static_cast<SDL_bool>(grab));
  SDL_SetRelativeMouseMode(static_cast<SDL_bool>(grab));
  grab ? SDL_ShowCursor(SDL_DISABLE) : SDL_ShowCursor(SDL_ENABLE);
#endif
}

void Window::ShowCursor(bool show) {
#ifndef MOBILE  // This breaks input @Android >9.0
  OgreAssert(sdlWindow, "sdlWindow not initialised");
  show ? SDL_ShowCursor(SDL_ENABLE) : SDL_ShowCursor(SDL_DISABLE);
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

VideoManager::VideoManager()
    : ogreMinLogLevel(Ogre::LML_TRIVIAL), ogreLogFile("Ogre.log"), shadowEnabled(true), shadowTechnique(Ogre::SHADOWTYPE_NONE), pssmSplitCount(3) {}
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
}

void VideoManager::OnUpdate(float time) {
  static ImGuiIO &io = ImGui::GetIO();
  Ogre::ImGuiOverlay::NewFrame();
  ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Always);
  ImGui::SetNextWindowSize({0, 0}, ImGuiCond_Always);
  ImGui::SetNextWindowCollapsed(false, ImGuiCond_Always);
  ImGui::SetNextWindowBgAlpha(0.5);
  ImGui::Begin("FPS", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0 / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  ImGui::End();
}

void VideoManager::OnClean() {
  Ogre::ResourceGroupManager::getSingleton().unloadResourceGroup(Ogre::RGN_DEFAULT);
  if (sceneManager) sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
  if (sceneManager) sceneManager->clearScene();
}

void VideoManager::OnPause() {}

void VideoManager::OnResume() {}

void VideoManager::RenderFrame() {
  ogreRoot->renderOneFrame();
#if defined(WINDOWS) || defined(ANDROID)
  SDL_GL_SwapWindow(mainWindow->sdlWindow);
#endif
}

Window &VideoManager::GetWindow(int number) { return windowList[0]; }

Window &VideoManager::GetMainWindow() { return *mainWindow; }

void VideoManager::ShowMainWindow(bool show) { mainWindow->Show(show); }

void VideoManager::CheckGPU() {
  const auto *ogreRenderCapabilities = Ogre::Root::getSingleton().getRenderSystem()->getCapabilities();
  const auto *ogreRenderSystemCommon = dynamic_cast<Ogre::GLRenderSystemCommon *>(Ogre::Root::getSingleton().getRenderSystem());
  OgreAssert(ogreRenderCapabilities->hasCapability(Ogre::RSC_HWRENDER_TO_TEXTURE), "Render to texture support required");
  OgreAssert(ogreRenderCapabilities->hasCapability(Ogre::RSC_TEXTURE_FLOAT), "Float texture support required");
  OgreAssert(ogreRenderCapabilities->hasCapability(Ogre::RSC_TEXTURE_COMPRESSION), "Texture compression support required");
#if defined(DESKTOP)
  OgreAssert(ogreRenderCapabilities->hasCapability(Ogre::RSC_TEXTURE_COMPRESSION_DXT), "DXT compression support required");
#elif defined(ANDROID)
  OgreAssert(ogreRenderCapabilities->hasCapability(Ogre::RSC_TEXTURE_COMPRESSION_ETC1), "ETC1 compression support required");
#elif defined(IOS)
  OgreAssert(ogreRenderCapabilities->hasCapabilityOgre::(RSC_TEXTURE_COMPRESSION_PVRTC), "PVRTC compression support required");
#endif
  if (RenderSystemIsGL3()) {
#ifdef OGRE_BUILD_RENDERSYSTEM_GL3PLUS
    OgreAssert(ogreRenderSystemCommon->hasMinGLVersion(3, 3), "OpenGL 3.3 is not supported");
#endif
  } else if (RenderSystemIsGLES2()) {
#ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
    OgreAssert(ogreRenderSystemCommon->hasMinGLVersion(3, 0), "OpenGLES 3.0 is not supported");
#endif
  }
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
  ogreMinLogLevel = Ogre::LML_TRIVIAL;
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
  auto *terrainGlobalOption = Ogre::TerrainGlobalOptions::getSingletonPtr();
  if (!terrainGlobalOption) terrainGlobalOption = new Ogre::TerrainGlobalOptions();
  terrainGlobalOption->setDefaultMaterialGenerator(make_shared<Ogre::TerrainMaterialGeneratorB>());
  terrainGlobalOption->setUseRayBoxDistanceCalculation(true);
#endif
  ogreRoot->initialise(false);
}

void VideoManager::CreateWindow() {
  windowList.emplace_back();
  mainWindow = &windowList[0];
  ogreCamera = sceneManager->createCamera("Default");
  mainWindow->Create("Example0", ogreCamera, -1, false, 1270, 720);
  ogreViewport = mainWindow->ogreViewport;
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
  io.ConfigFlags |= ImGuiBackendFlags_HasGamepad;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
  io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;
  //  AddFont("NotoSans-Regular", RGN_INTERNAL, nullptr, io.Fonts->GetGlyphRangesCyrillic());
  //  ImFontConfig config;
  //  config.MergeMode = true;
  //  static const ImWchar icon_ranges[] = {ICON_MIN_MD, ICON_MAX_MD, 0};
  //  AddFont("KenneyIcon-Regular", RGN_INTERNAL, &config, icon_ranges);
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
  auto &ogreResourceManager = Ogre::ResourceGroupManager::getSingleton();
  const char *FILE_SYSTEM = "FileSystem";
  const char *ZIP = "Zip";
  const char *APKZIP = "APKZip";
  const char *PROGRAMS_ZIP = "programs.zip";
  const char *ASSETS_ZIP = "assets.zip";
  const int SCAN_DEPTH = 4;
#if defined(DESKTOP) && defined(DEBUG)
  const char *PROGRAMS_DIR = "source/Programs";
  const char *GLSL_DIR = "source/GLSL";
  const char *GLSLES_DIR = "source/GLSLES";
  const char *ASSETS_DIR = "source/Example/Assets";
  ScanLocation(FindPath(PROGRAMS_DIR, SCAN_DEPTH), Ogre::RGN_INTERNAL);
  if (RenderSystemIsGLES2())
    ScanLocation(FindPath(GLSLES_DIR, SCAN_DEPTH), Ogre::RGN_INTERNAL);
  else
    ScanLocation(FindPath(GLSL_DIR, SCAN_DEPTH), Ogre::RGN_INTERNAL);
  ScanLocation(FindPath(ASSETS_DIR, SCAN_DEPTH), Ogre::RGN_DEFAULT);
#elif defined(ANDROID)
  ogreResourceManager.addResourceLocation(PROGRAMS_ZIP, APKZIP, Ogre::RGN_INTERNAL);
  ogreResourceManager.addResourceLocation(ASSETS_ZIP, APKZIP, Ogre::RGN_DEFAULT);
#else
  ogreResourceManager.addResourceLocation(FindPath(PROGRAMS_ZIP, SCAN_DEPTH), ZIP, Ogre::RGN_INTERNAL);
  ogreResourceManager.addResourceLocation(FindPath(ASSETS_ZIP, SCAN_DEPTH), ZIP, Ogre::RGN_DEFAULT);
#endif
  ogreResourceManager.initialiseResourceGroup(Ogre::RGN_INTERNAL);
  ogreResourceManager.initialiseAllResourceGroups();
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
    shadowTexSize = 2048;
    sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
    sceneManager->setShadowFarDistance(shadowFarDistance);
    sceneManager->setShadowTextureSize(shadowTexSize);
    sceneManager->setShadowTexturePixelFormat(ShadowTextureFormat);
    sceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, 3);
    sceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_SPOTLIGHT, 1);
    sceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_POINT, 0);
    auto passCaterMaterial = Ogre::MaterialManager::getSingleton().getByName("PSSM/shadow_caster");
    sceneManager->setShadowTextureCasterMaterial(passCaterMaterial);
    pssmSetup = make_shared<Ogre::PSSMShadowCameraSetup>();
    pssmSetup->calculateSplitPoints(pssmSplitCount, 0.001, sceneManager->getShadowFarDistance());
    pssmSplitPointList = pssmSetup->getSplitPoints();
    pssmSetup->setSplitPadding(0.0);
    for (int i = 0; i < pssmSplitCount; i++) pssmSetup->setOptimalAdjustFactor(i, static_cast<Ogre::Real>(0.5 * i));
    sceneManager->setShadowCameraSetup(pssmSetup);
    auto *shaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
    auto *schemRenderState = shaderGenerator->getRenderState(Ogre::MSN_SHADERGEN);
    auto subRenderState = shaderGenerator->createSubRenderState<Ogre::RTShader::IntegratedPSSM3>();
    subRenderState->setSplitPoints(pssmSplitPointList);
    schemRenderState->addTemplateSubRenderState(subRenderState);
  } else {
    sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
    sceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, 0);
    sceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_SPOTLIGHT, 0);
    sceneManager->setShadowTextureCountPerLightType(Ogre::Light::LT_POINT, 0);
  }
}

}  // namespace Glue
