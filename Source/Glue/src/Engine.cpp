// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Engine.h"
#include "Components/ComponentsAll.h"
#include "Components/DotSceneLoaderB.h"
#include "Conf.h"
#include "Input/InputSequencer.h"
#include "Log.h"
#include "Platform.h"
#include "RTSS.h"
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
#ifdef OGRE_BUILD_PLUGIN_DOT_SCENE
#include <Plugins/DotScene/OgreDotSceneLoader.h>
#endif
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
#include "Android.h"
#endif
#include "Filesystem.h"
#include <algorithm>
extern "C" {
#include <SDL2/SDL_syswm.h>
}

using namespace std;
using namespace Ogre;

namespace Glue {

Engine::Engine() {}

Engine::~Engine() { SDL_SetWindowFullscreen(SDLWindowPtr, SDL_FALSE); }

void Engine::InitComponents() {
  ReadConfFile();
  ComponentI::SetConfig(ConfPtr.get());

  WindowWidth = ConfPtr->GetInt("window_width", WindowWidth);
  WindowHeight = ConfPtr->GetInt("window_high", WindowHeight);
  WindowFullScreen = ConfPtr->GetBool("window_fullscreen", WindowFullScreen);
  RenderSystemName = ConfPtr->Get("render_system", RenderSystemName);

  OgreRoot = new Root();

  // init Ogre
  InitSDLSubsystems();
  InitDefaultRenderSystem();
  InitOgrePlugins();
  OgreRoot->initialise(false);
  CreateSDLWindow();
  CreateOgreRenderWindow();

#ifdef ANDROID
  WindowRestoreFullscreenAndroid();
#endif

  InitResourceLocation();

  // RTSS
  InitRtss();
  CreateRTSSRuntime();

  // Materials
  InitTextureSettings();
  InitShadowSettings();

  InitOverlay();
  InitCompositor();
  InitPhysics();
  InitSound();
  InitScene();
}

void Engine::InitSDLSubsystems() {
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
}

void Engine::InitDefaultRenderSystem() {
#ifdef DESKTOP
#ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
  InitOgreRenderSystemGLES2();
#else
  InitOgreRenderSystemGL3();
#endif
#else
  InitOgreRenderSystemGLES2();
#endif
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

#ifdef DEBUG

#ifdef OGRE_BUILD_PLUGIN_FREEIMAGE
  Root::getSingleton().installPlugin(new FreeImagePlugin());
#endif

#ifdef OGRE_BUILD_PLUGIN_ASSIMP
  Root::getSingleton().installPlugin(new AssimpPlugin());
#endif

#endif  // DEBUG

#ifdef OGRE_BUILD_PLUGIN_OCTREE
  OgreSceneManager = OgreRoot->createSceneManager("OctreeSceneManager", "Default");
#else
  OgreSceneManager = OgreRoot->createSceneManager(ST_GENERIC, "Default");
#endif

  Root::getSingleton().installPlugin(new DotScenePluginB());

  //#ifdef OGRE_BUILD_PLUGIN_DOT_SCENE
  //  Root::getSingleton().installPlugin(new DotScenePlugin());
  //#endif
}

void Engine::CreateSDLWindow() {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID

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

  SDLWindowPtr = SDL_CreateWindow(nullptr, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ScreenWidth, ScreenHeight, SDLWindowFlags);
  SDLGLContextPtr = SDL_GL_CreateContext(SDLWindowPtr);

#endif
}

void Engine::CreateOgreRenderWindow() {
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
  JNIEnv *env = (JNIEnv *)SDL_AndroidGetJNIEnv();

  jclass class_activity = env->FindClass("android/app/Activity");
  jclass class_resources = env->FindClass("android/content/res/Resources");
  jmethodID method_get_resources = env->GetMethodID(class_activity, "getResources", "()Landroid/content/res/Resources;");
  jmethodID method_get_assets = env->GetMethodID(class_resources, "getAssets", "()Landroid/content/res/AssetManager;");
  jobject raw_activity = (jobject)SDL_AndroidGetActivity();
  jobject raw_resources = env->CallObjectMethod(raw_activity, method_get_resources);
  jobject raw_asset_manager = env->CallObjectMethod(raw_resources, method_get_assets);

  params["currentGLContext"] = "true";
  params["externalGLControl"] = "true";
  params["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(info.info.android.window));
  params["preserveContext"] = "true";

  AAssetManager *asset_manager = AAssetManager_fromJava(env, raw_asset_manager);
  AConfiguration *mAConfig = AConfiguration_new();
  AConfiguration_fromAssetManager(mAConfig, asset_manager);

  ArchiveManager::getSingleton().addArchiveFactory(new APKFileSystemArchiveFactory(asset_manager));
  ArchiveManager::getSingleton().addArchiveFactory(new APKZipArchiveFactory(asset_manager));
#endif

  const char TrueStr[] = "true";
  const char FalseStr[] = "false";

  WindowsVsync = ConfPtr->GetBool("vsync", WindowsVsync);
  WindowGammaCorrection = ConfPtr->GetBool("gamma", WindowGammaCorrection);
  FSAA = ConfPtr->GetInt("fsaa", FSAA);

  params["vsync"] = WindowsVsync ? TrueStr : FalseStr;
  params["gamma"] = WindowGammaCorrection ? TrueStr : FalseStr;
  params["FSAA"] = to_string(FSAA);

  OgreRenderWindowPtr = OgreRoot->createRenderWindow(WindowCaption, WindowWidth, WindowHeight, WindowFullScreen, &params);
  OgreRenderTarget = OgreRoot->getRenderTarget(OgreRenderWindowPtr->getName());

  OgreCamera = OgreSceneManager->createCamera("Default");
  OgreViewport = OgreRenderTarget->addViewport(OgreCamera);
  OgreViewport->setBackgroundColour(ColourValue::Black);
  OgreCamera->setAspectRatio(static_cast<float>(OgreViewport->getActualWidth()) / static_cast<float>(OgreViewport->getActualHeight()));
  OgreCamera->setAutoAspectRatio(true);
}

void Engine::InitShadowSettings() {
  bool shadows_enable = true;
  float shadow_far = 400;
  int16_t tex_size = 256;
  string tex_format = "D16";

  shadows_enable = ConfPtr->GetBool("shadows_enable", shadows_enable);
  shadow_far = ConfPtr->GetInt("shadow_far", shadow_far);
  tex_format = ConfPtr->GetString("tex_format", tex_format);
  tex_size = ConfPtr->GetInt("tex_size", tex_size);

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

#ifdef DESKTOP
  TextureManager::getSingleton().setDefaultNumMipmaps(MIP_UNLIMITED);
#endif
}

void Engine::InitTextureSettings() {
  string filtration = ConfPtr->GetString("filtration");
  int anisotropy = 4;

  anisotropy = ConfPtr->GetInt("anisotropy", anisotropy);

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

void Engine::Capture() {
  static auto &io = InputSequencer::GetInstance();
  io.Capture();
}

void Engine::ReadConfFile() {
#ifndef MOBILE
  ConfPtr = make_unique<Conf>("config.ini");
#else
  // TODO: fill defauls for Android
  ConfPtr = make_unique<Conf>("");
#endif
}

void Engine::InitSound() {
  SoundPtr = make_unique<Sound>();
  RegComponent(SoundPtr.get());
}

void Engine::InitOverlay() {
  OverlayPtr = make_unique<Overlay>(OgreRenderWindowPtr);
  RegComponent(OverlayPtr.get());
  auto &RGM = ResourceGroupManager::getSingleton();
  RGM.initialiseResourceGroup(RGN_INTERNAL);
  RGM.initialiseAllResourceGroups();
}

void Engine::InitCompositor() {
  CompositorUPtr = make_unique<Compositor>();
  RegComponent(CompositorUPtr.get());
  OverlayPtr->PrepareFontTexture("Roboto-Medium", RGN_INTERNAL);
  CompositorUPtr->SetUp();
}

void Engine::InitPhysics() {
  PhysicsPtr = make_unique<Physics>();
  RegComponent(PhysicsPtr.get());
}

void Engine::InitScene() {
  ScenePtr = make_unique<Scene>();
  RegComponent(ScenePtr.get());
}

void Engine::RegComponent(ComponentI *ComponentPtr) { ComponentList.push_back(ComponentPtr); }

void Engine::UnRegComponent(ComponentI *ComponentPtr) {
  // TODO: implement this
}

void Engine::Pause() {
  for (auto &it : ComponentList) it->OnPause();
}

void Engine::InMenu() {
  PhysicsPtr->OnPause();
  ScenePtr->OnPause();
}

void Engine::OffMenu() {
  PhysicsPtr->OnResume();
  ScenePtr->OnResume();
}

void Engine::Resume() {
  for (auto &it : ComponentList) it->OnResume();
}

void Engine::Cleanup() {
  for (auto &it : ComponentList) it->OnClean();
  InitShadowSettings();
  CompositorUPtr->SetUp();
}

void Engine::Update(float PassedTime) {
  for (auto &it : ComponentList) it->OnUpdate(PassedTime);
}

void Engine::RenderFirstFrame() {
  RenderOneFrame();
  CleanRTSSRuntime();
}

void Engine::RenderOneFrame() {
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
  if (Fullscreen)
    SetFullscreen();
  else
    SetWindowed();
}

void Engine::SetFullscreen() {
  WindowFullScreen = true;
  OgreRenderWindowPtr->setFullscreen(WindowFullScreen, WindowWidth, WindowHeight);
#ifdef DESKTOP
  SDL_SetWindowFullscreen(SDLWindowPtr, SDLWindowFlags | SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN_DESKTOP);
#else
  SDL_SetWindowFullscreen(SDLWindowPtr, SDLWindowFlags | SDL_WINDOW_FULLSCREEN);
#endif
}

void Engine::SetWindowed() {
  WindowFullScreen = false;
  OgreRenderWindowPtr->setFullscreen(WindowFullScreen, WindowWidth, WindowHeight);
  SDL_SetWindowFullscreen(SDLWindowPtr, SDLWindowFlags);
  SDL_SetWindowSize(SDLWindowPtr, WindowWidth, WindowHeight);
}

void Engine::SetWindowCaption(const char *Caption) { SDL_SetWindowTitle(SDLWindowPtr, Caption); }

void Engine::WindowRestoreFullscreenAndroid() {
  SDL_DisplayMode DM;
  SDL_GetDesktopDisplayMode(0, &DM);
  int screen_w = static_cast<int>(DM.w);
  int screen_h = static_cast<int>(DM.h);
  OgreRenderWindowPtr->resize(screen_w, screen_h);
}

std::pair<int, int> Engine::GetWindowSize() const { return make_pair(WindowWidth, WindowHeight); }

void Engine::GrabMouse(bool grab) {
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

void Engine::GrabMouse() { GrabMouse(true); }

void Engine::FreeMouse() { GrabMouse(false); }

#ifndef MOBILE

string FindPath(const string &Path, int Depth = 2) {
#ifdef DEBUG
  Depth = 6;
#endif

  string result = Path;

  for (int i = 0; i < Depth; i++) {
    if (fs::exists(result))
      return result;
    else if (fs::exists(result + ".zip"))
      return result.append(".zip");
    else
      result = string("../").append(result);
  }

  return string();
}

static inline bool CheckSymbol(char c) { return isalpha(c) || isdigit(c) || c == '.' || c == ',' || c == ';' || c == '_' || c == '-' || c == '/'; }

static inline bool StringSanityCheck(const string &str) {
  if (str.empty() || str[0] == '#') {
    return true;
  }

  return any_of(str.begin(), str.end(), CheckSymbol);
}

static inline void LeftTrim(string &s) {
  auto it = find_if(s.begin(), s.end(), [](char c) { return !isspace(c); });
  s.erase(s.begin(), it);
}

static inline void RightTrim(string &s) {
  auto it = find_if(s.rbegin(), s.rend(), [](char c) { return !isspace(c); });
  s.erase(it.base(), s.end());
}

static inline void TrimString(string &s) {
  RightTrim(s);
  LeftTrim(s);
}

static void LoadResources() {
  auto &rgm = ResourceGroupManager::getSingleton();
  rgm.initialiseResourceGroup(RGN_INTERNAL);
  rgm.initialiseAllResourceGroups();
}

static void AddLocation(const std::string &Path, const std::string &GroupName = RGN_DEFAULT, const std::string &ResourceFile = "",
                        bool Verbose = false) {
  const string file_system = "FileSystem";
  const string zip = "Zip";

  std::vector<string> file_list;
  std::vector<string> dir_list;
  std::vector<tuple<string, string, string>> resource_list;
  auto &RGM = ResourceGroupManager::getSingleton();

  string path = FindPath(Path);

  if (fs::exists(path)) {
    if (fs::is_directory(path))
      resource_list.push_back({path, file_system, GroupName});
    else if (fs::is_regular_file(path) && fs::path(path).extension() == ".zip")
      RGM.addResourceLocation(path, zip, GroupName);
  }

  if (!ResourceFile.empty()) {
    fstream list_file;
    list_file.open(ResourceFile);

    string line;
    string type;
    string group;

    if (list_file.is_open()) {
      while (getline(list_file, line)) {
        TrimString(line);

        if (!StringSanityCheck(line)) {
          throw Exception(string("Sanity check of ") + ResourceFile + " is failed. Aborting.");
        }

        if (line[0] == '#') {
          continue;
        }

        stringstream ss(line);
        getline(ss, path, ',');
        getline(ss, type, ',');
        getline(ss, group, ';');
        resource_list.push_back({path, type, group});
      }
    }
  }

  const std::vector<string> extensions_list = {".glsl",       ".glslt",    ".hlsl", ".hlslt",   ".gles", ".cg",  ".vert", ".frag", ".material",
                                               ".compositor", ".particle", ".fx",   ".program", ".dds",  ".bmp", ".png",  ".tga",  ".jpg",
                                               ".jpeg",       ".mesh",     ".xml",  ".scene",   ".json", ".wav", ".ogg",  ".mp3",  ".flac"};

  for (const auto &it : resource_list) {
    RGM.addResourceLocation(get<0>(it), get<1>(it), get<2>(it));
    dir_list.push_back(get<0>(it));

    auto jt = fs::recursive_directory_iterator(get<0>(it));

    for (; jt != fs::recursive_directory_iterator(); ++jt) {
      const auto full_path = jt->path().string();
      const auto file_name = jt->path().filename().string();

      if (jt->is_directory()) {
        if (Verbose) {
          Log::Message(string("Found directory: ") + full_path);
        }

        dir_list.push_back(file_name);

        RGM.addResourceLocation(full_path, file_system, get<2>(it));

      } else if (jt->is_regular_file()) {
        if (Verbose) {
          Log::Message(string("Found file: ") + full_path);
        }
        if (fs::path(full_path).extension() == ".zip") {
          if (find(extensions_list.begin(), extensions_list.end(), fs::path(file_name).extension()) != extensions_list.end()) {
            file_list.push_back(file_name);
          }

          RGM.addResourceLocation(full_path, zip, get<2>(it));
        }
      }
    }
  }
}

#endif  // !MOBILE

void Engine::InitResourceLocation() {
#ifndef MOBILE
  AddLocation("Programs", RGN_INTERNAL);
  AddLocation("Assets", RGN_DEFAULT);
#else
  auto &RGM = ResourceGroupManager::getSingleton();
  RGM.addResourceLocation("/Programs/Core.zip", "APKZip", RGN_INTERNAL);
  RGM.addResourceLocation("/Programs/Other.zip", "APKZip", RGN_INTERNAL);
  RGM.addResourceLocation("/Assets.zip", "APKZip", RGN_DEFAULT);
#endif
}

Engine &GetEngine() {
  static auto &EnginePtr = Engine::GetInstance();
  return EnginePtr;
}

Physics &GetPhysics() { return *GetComponent<Physics>(); }

Sound &GetAudio() { return *GetComponent<Sound>(); }

Scene &GetScene() { return *GetComponent<Scene>(); }

}  // namespace Glue
