// This source file is part of Glue Engine. Created by Andrey Vasiliev

#include "pch.h"
#include "Engine.h"
#include "DotSceneLoaderB.h"
#include "InputObserver.h"
#include "InputSequencer.h"
#include "PagedGeometry/PagedGeometryAll.h"
#include "SinbadCharacterController.h"
#include "SkyModel/ArHosekSkyModel.h"
#include "SkyModel/SkyModel.h"
#include "imgui_user/IconsFontAwesome5.h"
#include "imgui_user/IconsKenney.h"
#include "imgui_user/IconsMaterialDesign.h"
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
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
#ifdef OGRE_BUILD_COMPONENT_MESHLODGENERATOR
#include <MeshLodGenerator/OgreLodConfig.h>
#include <MeshLodGenerator/OgreMeshLodGenerator.h>
#include <OgreDistanceLodStrategy.h>
#include <OgrePixelCountLodStrategy.h>
#endif
#ifdef OGRE_BUILD_COMPONENT_TERRAIN
#include "TerrainMaterialGeneratorB.h"
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainAutoUpdateLod.h>
#include <Terrain/OgreTerrainGroup.h>
#include <Terrain/OgreTerrainMaterialGeneratorA.h>
#include <Terrain/OgreTerrainQuadTreeNode.h>
#endif
#ifdef OGRE_BUILD_COMPONENT_PAGING
#include <Paging/OgrePage.h>
#include <Paging/OgrePaging.h>
#endif
#ifdef OGRE_BUILD_COMPONENT_OVERLAY
#include <Overlay/OgreFont.h>
#include <Overlay/OgreFontManager.h>
#include <Overlay/OgreImGuiOverlay.h>
#include <Overlay/OgreOverlay.h>
#include <Overlay/OgreOverlayManager.h>
#include <Overlay/OgreOverlaySystem.h>
#endif

#ifndef MOBILE
#if __has_include( \
    <filesystem>) && ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || (defined(__cplusplus) && __cplusplus >= 201703L && !defined(__APPLE__)) || (!defined(__MAC_OS_X_VERSION_MIN_REQUIRED) || __MAC_OS_X_VERSION_MIN_REQUIRED >= 101500))
#include <filesystem>
namespace fs = std::filesystem;
#else  // APPLE
#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;
#endif  // !APPLE
#endif  // DESKTOP

using namespace std;
using namespace Ogre;

namespace Glue {

bool GlobalMRTIsEnabled() { return true; }

bool CPUSupportsSSE() {
#if OGRE_CPU == OGRE_CPU_X86
  static bool Result = Ogre::PlatformInformation::hasCpuFeature(Ogre::PlatformInformation::CPU_FEATURE_SSE);
  return Result;
#else
  return false;
#endif
};

bool CPUSupportsSSE2() {
#if OGRE_CPU == OGRE_CPU_X86
  static bool Result = Ogre::PlatformInformation::hasCpuFeature(Ogre::PlatformInformation::CPU_FEATURE_SSE2);
  return Result;
#else
  return false;
#endif
};

bool CPUSupportsSSE3() {
#if OGRE_CPU == OGRE_CPU_X86
  static bool Result = Ogre::PlatformInformation::hasCpuFeature(Ogre::PlatformInformation::CPU_FEATURE_SSE3);
  return Result;
#else
  return false;
#endif
};

bool CPUSupportsSSE41() {
#if OGRE_CPU == OGRE_CPU_X86
  static bool Result = Ogre::PlatformInformation::hasCpuFeature(Ogre::PlatformInformation::CPU_FEATURE_SSE41);
  return Result;
#else
  return false;
#endif
};

bool CPUSupportsSSE42() {
#if OGRE_CPU == OGRE_CPU_X86
  static bool Result = Ogre::PlatformInformation::hasCpuFeature(Ogre::PlatformInformation::CPU_FEATURE_SSE42);
  return Result;
#else
  return false;
#endif
};

bool CPUSupportsNEON() {
#if OGRE_CPU == OGRE_CPU_ARM
  static bool Result = Ogre::PlatformInformation::hasCpuFeature(Ogre::PlatformInformation::CPU_FEATURE_NEON);
  return Result;
#else
  return false;
#endif
};

bool IsFullscreen() { return GetEngine().IsWindowFullscreen(); }

bool RenderSystemIsGL() {
  static bool Result = !Ogre::Root::getSingleton().getRenderSystem()->getName().compare("OpenGL Rendering Subsystem");
  return Result;
};

bool RenderSystemIsGL3() {
  static bool Result = !Ogre::Root::getSingleton().getRenderSystem()->getName().compare("OpenGL 3+ Rendering Subsystem");
  return Result;
};

bool RenderSystemIsGLES2() {
  static bool Result = !Ogre::Root::getSingleton().getRenderSystem()->getName().compare("OpenGL ES 2.x Rendering Subsystem");
  return Result;
};

SceneManager *OgreSceneManager() {
  static SceneManager *Result = Ogre::Root::getSingleton().getSceneManager("Default");
  return Result;
}

SceneNode *OgreRootNode() {
  static SceneNode *Result = Ogre::Root::getSingleton().getSceneManager("Default")->getRootSceneNode();
  return Result;
}

Camera *OgreCamera() {
  static Camera *Result = Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default");
  return Result;
}

SceneNode *OgreCameraNode() {
  static SceneNode *Result = Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default")->getParentSceneNode();
  return Result;
}

Vector3 SunDirection() {
  Vector3 Result = -OgreSceneManager()->getLight("Sun")->getParentSceneNode()->getPosition();
  return Result;
}

int WindowSizeX() { return GetEngine().GetWindowSizeX(); }

int WindowSizeY() { return GetEngine().GetWindowSizeY(); }

std::string WindowCaption() { return GetEngine().GetWindowCaption(); }

void ShowMouseCursor(bool draw) {
  //  static ImGuiIO& io = ImGui::GetIO();
  //  io.MouseDrawCursor = draw;
}

#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
class ShaderResolver final : public Ogre::MaterialManager::Listener {
 public:
  ShaderResolver(Ogre::RTShader::ShaderGenerator *ShaderGeneratorPtr) { this->ShaderGeneratorPtr = ShaderGeneratorPtr; }

  static bool FixMaterial(const std::string &material_name) {
    auto &mShaderGenerator = RTShader::ShaderGenerator::getSingleton();
    auto originalMaterial = MaterialManager::getSingleton().getByName(material_name);
    const auto DSNOld = MaterialManager::DEFAULT_SCHEME_NAME;
    const auto DSN = MSN_SHADERGEN;

    if (!originalMaterial) originalMaterial = MaterialManager::getSingleton().getByName(material_name, RGN_INTERNAL);

    if (!originalMaterial) return false;

    // SetUp shader generated technique for this material.
    bool techniqueCreated = mShaderGenerator.createShaderBasedTechnique(*originalMaterial, DSNOld, DSN);

    // Case technique registration succeeded.
    // Force creating the shaders for the generated technique.

    if (techniqueCreated)
      mShaderGenerator.validateMaterial(DSN, originalMaterial->getName(), originalMaterial->getGroup());
    else
      originalMaterial->getTechnique(0)->setSchemeName(DSN);

    return true;
  }

  Ogre::Technique *handleSchemeNotFound(unsigned short SchemeIndex, const std::string &SchemeName, Ogre::Material *OriginalMaterialPtr,
                                        unsigned short LodIndex, const Ogre::Renderable *OgreRenderable) {
    const auto DSNOld = MaterialManager::DEFAULT_SCHEME_NAME;
    const auto DSN = MSN_SHADERGEN;

    if (SchemeName != DSN) return nullptr;

    // SetUp shader generated technique for this material.
    bool techniqueCreated = ShaderGeneratorPtr->createShaderBasedTechnique(*OriginalMaterialPtr, DSNOld, SchemeName);

    // Case technique registration succeeded.
    if (techniqueCreated) {
      // Force creating the shaders for the generated technique.
      ShaderGeneratorPtr->validateMaterial(SchemeName, OriginalMaterialPtr->getName(), OriginalMaterialPtr->getGroup());

      // Grab the generated technique.
      for (const auto &it : OriginalMaterialPtr->getTechniques()) {
        if (it->getSchemeName() == SchemeName) {
          return it;
        }
      }
    } else {
      OriginalMaterialPtr->getTechnique(0)->setSchemeName(DSN);
    }

    return nullptr;
  }

  bool afterIlluminationPassesCreated(Ogre::Technique *OgreTechnique) {
    const auto DSNOld = MaterialManager::DEFAULT_SCHEME_NAME;
    const auto DSN = MSN_SHADERGEN;

    if (OgreTechnique->getSchemeName() == DSN) {
      Material *mat = OgreTechnique->getParent();
      ShaderGeneratorPtr->validateMaterialIlluminationPasses(OgreTechnique->getSchemeName(), mat->getName(), mat->getGroup());
      return true;
    } else {
      return false;
    }
  }

  bool beforeIlluminationPassesCleared(Ogre::Technique *OgreTechnique) {
    if (OgreTechnique->getSchemeName() == MSN_SHADERGEN) {
      Material *mat = OgreTechnique->getParent();
      ShaderGeneratorPtr->invalidateMaterialIlluminationPasses(OgreTechnique->getSchemeName(), mat->getName(), mat->getGroup());
      return true;
    } else {
      return false;
    }
  }

 protected:
  Ogre::RTShader::ShaderGenerator *ShaderGeneratorPtr = nullptr;
};

static unique_ptr<ShaderResolver> ResolverPtr;

void InitRTSS() { OgreAssert(RTShader::ShaderGenerator::initialize(), "OGRE RTSS init failed"); }

void InitRTSSRuntime(string CachePath = "") {
  auto *OgreScene = Root::getSingleton().getSceneManager("Default");
  auto *OgreViewport = OgreScene->getCamera("Default")->getViewport();
  auto *ShaderGenerator = RTShader::ShaderGenerator::getSingletonPtr();

  OgreViewport->setMaterialScheme(MSN_SHADERGEN);
  ShaderGenerator->addSceneManager(OgreScene);
  OgreViewport->setMaterialScheme(MSN_SHADERGEN);
  ShaderGenerator->setShaderCachePath(CachePath);
  ResolverPtr = make_unique<ShaderResolver>(ShaderGenerator);
  MaterialManager::getSingleton().addListener(ResolverPtr.get());
}

void ClearRTSSRuntime() {
  if (ResolverPtr) {
    MaterialManager::getSingleton().removeListener(ResolverPtr.get());
    ResolverPtr.reset();
  }
}

void InitRTSSPSSM(const vector<float> &SplitPoints) {
  const auto DSN = MSN_SHADERGEN;
  auto &rtShaderGen = RTShader::ShaderGenerator::getSingleton();
  auto *schemRenderState = rtShaderGen.getRenderState(DSN);

  auto subRenderState = rtShaderGen.createSubRenderState<RTShader::IntegratedPSSM3>();
  subRenderState->setSplitPoints(SplitPoints);
  schemRenderState->addTemplateSubRenderState(subRenderState);
}

void InitRTSSInstansing() {
  auto *scene_ = Root::getSingleton().getSceneManager("Default");
  auto *camera_ = scene_->getCamera("Default");
  auto *viewport_ = camera_->getViewport();
  const auto DSN = MSN_SHADERGEN;

  RTShader::ShaderGenerator &rtShaderGen = RTShader::ShaderGenerator::getSingleton();
  viewport_->setMaterialScheme(DSN);
  RTShader::RenderState *schemRenderState = rtShaderGen.getRenderState(DSN);
  RTShader::SubRenderState *subRenderState = rtShaderGen.createSubRenderState<RTShader::IntegratedPSSM3>();
  schemRenderState->addTemplateSubRenderState(subRenderState);

  // Add the hardware skinning to the shader generator default render state
  subRenderState = rtShaderGen.createSubRenderState<RTShader::HardwareSkinning>();
  schemRenderState->addTemplateSubRenderState(subRenderState);

  // increase max bone count for higher efficiency
  RTShader::HardwareSkinningFactory::getSingleton().setMaxCalculableBoneCount(80);

  // re-generate shaders to include new SRSs
  rtShaderGen.invalidateScheme(DSN);
  rtShaderGen.validateScheme(DSN);

  // update scheme for FFP supporting rendersystems
  MaterialManager::getSingleton().setActiveScheme(viewport_->getMaterialScheme());
}

#endif  // OGRE_BUILD_COMPONENT_RTSHADERSYSTEM

Engine::Engine() { TestCPUCapabilities(); }

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

ImFont *AddFont(const String &name, const String &group OGRE_RESOURCE_GROUP_INIT, const ImFontConfig *fontCfg = NULL,
                const ImWchar *glyphRanges = NULL) {
  typedef std::vector<ImWchar> CodePointRange;
  std::vector<CodePointRange> mCodePointRanges;

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

void Engine::InitComponents() {
  InitSDLWindow();
  OgreRoot = new Root("", "", "");
  InitOgrePlugins();
  OgreRoot->initialise(false);
  InitOgreRenderWindow();
  TestGPUCapabilities();
  InitResourceLocation();
  InitRTSS();
  InitRTSSRuntime();
#ifdef DESKTOP
  InitRTSSInstansing();
#endif
  InitTextureSettings();
  InitOverlay();

  InitResources();

  InitShadowSettings();
  InitPhysics();
  InitSound();
  InitCompositor();
  for (auto it : ComponentList) it->OnSetUp();

  ImGuiIO &io = ImGui::GetIO();
  AddFont("NotoSans-Regular", Ogre::RGN_INTERNAL, nullptr, io.Fonts->GetGlyphRangesCyrillic());
  ImFontConfig config;
  config.MergeMode = true;
  static const ImWchar icon_ranges[] = {ICON_MIN_MD, ICON_MAX_MD, 0};
  AddFont("KenneyIcon-Regular", Ogre::RGN_INTERNAL, &config, icon_ranges);
  ImGuiOverlayPtr->show();

  auto *TGO = TerrainGlobalOptions::getSingletonPtr();
  if (!TGO) TGO = new TerrainGlobalOptions();
  TGO->setUseVertexCompressionWhenAvailable(true);
  TGO->setCastsDynamicShadows(false);
  TGO->setCompositeMapDistance(300);
  TGO->setMaxPixelError(8);
  TGO->setUseRayBoxDistanceCalculation(true);
  TGO->setDefaultMaterialGenerator(std::make_shared<TerrainMaterialGeneratorB>());
}

void Engine::InitSDLWindow() {
#ifndef EMSCRIPTEN
  OgreAssert(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER), "Failed to init SDL2");
#else
  OgreAssert(!SDL_Init(SDL_INIT_VIDEO), "Failed to init SDL2");
#endif
  SDL_DisplayMode Current;
  for (int i = 0; i < SDL_GetNumVideoDisplays(); i++) {
    if (SDL_GetCurrentDisplayMode(i, &Current) == 0) {
      SDL_Log("Display #%d: current display mode is %dx%dpx @ %dhz.", i, Current.w, Current.h, Current.refresh_rate);
      SDLMonitorList.push_back(Current);
      int ScreenDiag = sqrt(ScreenWidth * ScreenWidth + ScreenHeight * ScreenHeight);
      int TmpDiag = sqrt(Current.w * Current.w + Current.h * Current.h);
      if (TmpDiag > ScreenDiag) {
        CurrentSDLDisplayMode = Current;
        ScreenWidth = Current.w;
        ScreenHeight = Current.h;
        CurrentDisplay = i;
      } else {
        SDL_Log("Could not get display mode for video display #%d: %s", i, SDL_GetError());
      }
    }
  }
  for (int i = 0; i < SDL_NumJoysticks(); ++i)
    if (SDL_IsGameController(i)) SDL_GameControllerOpen(i);
#if defined(DESKTOP)
  if (WindowWidth == ScreenWidth && WindowHeight == ScreenHeight) SDLWindowFlags |= SDL_WINDOW_BORDERLESS;
  if (WindowFullScreen) {
    SDLWindowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    SDLWindowFlags |= SDL_WINDOW_BORDERLESS;
    WindowWidth = ScreenWidth;
    WindowHeight = ScreenHeight;
  }
  WindowPositionFlag = SDL_WINDOWPOS_CENTERED_DISPLAY(CurrentDisplay);
  SDLWindowPtr = SDL_CreateWindow(WindowCaption.c_str(), WindowPositionFlag, WindowPositionFlag, WindowWidth, WindowHeight, SDLWindowFlags);
#elif defined(EMSCRIPTEN)
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
  SDLWindowFlags |= SDL_WINDOW_OPENGL;
  SDLWindowFlags |= SDL_WINDOW_RESIZABLE;
  SDLWindowFlags |= SDL_WINDOW_ALLOW_HIGHDPI;
  WindowWidth = ScreenWidth;
  WindowHeight = ScreenHeight;
  WindowPositionFlag = SDL_WINDOWPOS_CENTERED;
  SDLWindowPtr = SDL_CreateWindow(nullptr, WindowPositionFlag, WindowPositionFlag, ScreenWidth, ScreenHeight, SDLWindowFlags);
  SDLGLContextPtr = SDL_GL_CreateContext(SDLWindowPtr);
#elif defined(ANDROID)
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
  SDLWindowFlags |= SDL_WINDOW_BORDERLESS;
  SDLWindowFlags |= SDL_WINDOW_FULLSCREEN;
  SDLWindowFlags |= SDL_WINDOW_ALLOW_HIGHDPI;
  SDLWindowFlags |= SDL_WINDOW_OPENGL;
  WindowWidth = ScreenWidth;
  WindowHeight = ScreenHeight;
  WindowPositionFlag = SDL_WINDOWPOS_UNDEFINED;
  SDLWindowPtr = SDL_CreateWindow(nullptr, WindowPositionFlag, WindowPositionFlag, ScreenWidth, ScreenHeight, SDLWindowFlags);
  SDLGLContextPtr = SDL_GL_CreateContext(SDLWindowPtr);
#endif
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

void Engine::InitOgrePlugins() {
#ifdef OGRE_STATIC_LIB
#ifdef DESKTOP
#if defined(OGRE_BUILD_RENDERSYSTEM_GL3PLUS)
  InitOgreRenderSystemGL3();
#elif defined(OGRE_BUILD_RENDERSYSTEM_GL)
  InitOgreRenderSystemGL();
#elif defined(OGRE_BUILD_RENDERSYSTEM_GLES2)
  InitOgreRenderSystemGLES2();
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

void Engine::InitOgreRenderWindow() {
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
  WindowVsync = true;
  OgreRenderParams["vsync"] = WindowVsync ? TrueStr : FalseStr;
  OgreRenderWindowPtr = OgreRoot->createRenderWindow(WindowCaption, WindowWidth, WindowHeight, WindowFullScreen, &OgreRenderParams);
  OgreRenderTargetPtr = OgreRoot->getRenderTarget(OgreRenderWindowPtr->getName());
  OgreCamera = OgreSceneManager->createCamera("Default");
  OgreViewport = OgreRenderTargetPtr->addViewport(OgreCamera);
  OgreCamera->setAspectRatio(static_cast<float>(OgreViewport->getActualWidth()) / static_cast<float>(OgreViewport->getActualHeight()));
  OgreCamera->setAutoAspectRatio(true);
#ifdef ANDROID
  AndroidRestoreWindow();
#endif
}

void Engine::InitShadowSettings() {
  bool ShadowsEnabled = true;
#ifdef MOBILE
  ShadowsEnabled = false;
#endif
  float ShadowFarDistance = 400;
  int16_t ShadowTexSize = 2048;
  //  ShadowsEnabled = ConfigPtr->GetBool("shadows_enable", ShadowsEnabled);
  //  ShadowFarDistance = ConfigPtr->GetInt("shadow_far", ShadowFarDistance);
  //  ShadowTexSize = ConfigPtr->GetInt("tex_size", ShadowTexSize);
  if (!ShadowsEnabled) {
    OgreSceneManager->setShadowTechnique(SHADOWTYPE_NONE);
    OgreSceneManager->setShadowTextureCountPerLightType(Light::LT_DIRECTIONAL, 0);
    OgreSceneManager->setShadowTextureCountPerLightType(Light::LT_SPOTLIGHT, 0);
    OgreSceneManager->setShadowTextureCountPerLightType(Light::LT_POINT, 0);
    return;
  }
#ifdef DESKTOP
  PixelFormat ShadowTextureFormat = PixelFormat::PF_DEPTH16;
#else
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
  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(MIP_UNLIMITED);

  if (Ogre::Root::getSingleton().getRenderSystem()->getCapabilities()->hasCapability(RSC_ANISOTROPY)) {
    Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(TFO_ANISOTROPIC);
    Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(8);
  } else {
    Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(TFO_BILINEAR);
  }
}

void Engine::Capture() {
  static auto &io = InputSequencer::GetInstance();
  io.Capture();
}

void Engine::InitSound() {
  SoundPtr = make_unique<Sound>();
  RegComponent(SoundPtr.get());
}

void Engine::InitResources() {
  auto &RGM = ResourceGroupManager::getSingleton();
  RGM.initialiseResourceGroup(RGN_INTERNAL);
  RGM.initialiseAllResourceGroups();
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

void Engine::InitOverlay() {
  OgreOverlayPtr = new Ogre::OverlaySystem();
  ImGuiOverlayPtr = new Ogre::ImGuiOverlay();

  // handle DPI scaling
  float vpScale = OverlayManager::getSingleton().getPixelRatio();
  ImGui::GetIO().FontGlobalScale = std::round(vpScale);  // default font does not work with fractional scaling
  ImGui::GetStyle().ScaleAllSizes(vpScale);

  ImGuiOverlayPtr->setZOrder(300);
  //  ImGuiOverlayPtr->show();
  Ogre::OverlayManager::getSingleton().addOverlay(ImGuiOverlayPtr);

  OgreSceneManager->addRenderQueueListener(OgreOverlayPtr);
  //  OgreRenderWindowPtr->addListener(this);

  ImGuiListener = make_unique<ImGuiInputListener>();
  ImGuiIO &io = ImGui::GetIO();
  io.IniFilename = nullptr;
  io.LogFilename = nullptr;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
  io.MouseDrawCursor = false;
  io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;
}

const static CompositorFX Bloom{"Bloom", false, "uBloomEnable", "Output", {"Bloom", "Downscale3x3"}};
const static CompositorFX SSAO{"SSAO", false, "uSSAOEnable", "Output", {"SSAO", "FilterX", "FilterY"}};
const static CompositorFX Blur{"Blur", false, "uBlurEnable", "Blur", {}};
const static CompositorFX FXAA{"FXAA", false, "uFXAAEnable", "FXAA", {}};
const static CompositorFX HDR{"HDR", false, "uHDREnable", "Output", {}};
const static std::string FX_SSAO = "SSAO";
const static std::string FX_BLOOM = "Bloom";
const static std::string FX_BLUR = "Blur";
const static std::string FX_FXAA = "FXAA";
const static std::string FX_HDR = "HDR";

static GpuProgramParametersSharedPtr GetFPparameters(const string &CompositorName) {
  static auto &MM = Ogre::MaterialManager::getSingleton();
  return MM.getByName(CompositorName)->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
}

void Engine::EnableEffect(const std::string &FX, bool Enable) {
  CompositorList[FX].Enabled = Enable;
  const string EnableStr = "uEnable";
  const float Flag = Enable ? 1.0f : 0.0f;
  const auto &FXPtr = CompositorList[FX];
  GetFPparameters(FXPtr.OutputCompositor)->setNamedConstant(FXPtr.EnableParam, Flag);
  for (const auto &it : FXPtr.CompositorChain) GetFPparameters(it)->setNamedConstant(EnableStr, Flag);
}

void Engine::InitCompositor() {
  OgreCompositorManager = Ogre::CompositorManager::getSingletonPtr();
  OgreCompositorChain = OgreCompositorManager->getCompositorChain(OgreViewport);
  CompositorList.insert(make_pair(FX_BLOOM, Bloom));
  CompositorList.insert(make_pair(FX_SSAO, SSAO));
  CompositorList.insert(make_pair(FX_BLUR, Blur));
  CompositorList.insert(make_pair(FX_FXAA, FXAA));
  CompositorList.insert(make_pair(FX_HDR, HDR));
  EnableEffect(FX_SSAO, false);
  EnableEffect(FX_BLOOM, false);
  EnableEffect(FX_BLUR, false);
  EnableEffect(FX_HDR, false);
  EnableEffect(FX_FXAA, false);

  const string MRTCompositor = "MRT";
  ViewportSizeY = OgreViewport->getActualDimensions().height();
  ViewportSizeX = OgreViewport->getActualDimensions().width();
  OgreAssert(OgreCompositorManager->addCompositor(OgreViewport, MRTCompositor, 0), "Failed to add MRT compositor");
  OgreCompositorManager->setCompositorEnabled(OgreViewport, MRTCompositor, false);
  auto *MRTCompositorPtr = OgreCompositorChain->getCompositor(MRTCompositor);
  auto *MRTTexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("mrt");
  OgreAssert(MRTTexturePtr, "MRTCompositor texture not created");
#ifdef MOBILE
  MRTTexturePtr->width = 1024;
  MRTTexturePtr->height = 768;
#else
  MRTTexturePtr->width = ViewportSizeX;
  MRTTexturePtr->height = ViewportSizeY;
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

  OgreCompositorManager->setCompositorEnabled(OgreViewport, MRTCompositor, true);
}

void Engine::InitPhysics() {
  PhysicsPtr = make_unique<Physics>();
  RegComponent(PhysicsPtr.get());
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

static void EnsureHasTangents(MeshPtr MeshSPtr) {
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
  auto MaterialSPtr = Ogre::MaterialManager::getSingleton().getByName(material);
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
  } catch (Ogre::Exception &e) {
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
  } catch (Ogre::Exception &e) {
    LogManager::getSingleton().logMessage(e.getFullDescription());
    LogManager::getSingleton().logMessage("[DotSceneLoader] Error loading an entity!");
  }
}

Ogre::Vector3 Engine::GetSunPosition() {
  auto *SunPtr = OgreSceneManager->getLight("Sun");
  if (SunPtr)
    return -Vector3(SunPtr->getDerivedDirection().normalisedCopy());
  else
    return Vector3::ZERO;
}

float Engine::GetHeight(float x, float z) {
  if (OgreTerrainList)
    return OgreTerrainList->getHeightAtWorldPosition(x, 1000, z);
  else
    return 0.0f;
}

void Engine::AddEntity(Ogre::Entity *EntityPtr) { AddEntityMaterial(EntityPtr); }

void Engine::AddMaterial(Ogre::MaterialPtr material) {
  const auto *Pass = material->getTechnique(0)->getPass(0);
  if (Pass->hasVertexProgram()) GpuVpParams.push_back(Pass->getVertexProgramParameters());
  if (Pass->hasFragmentProgram()) GpuFpParams.push_back(Pass->getFragmentProgramParameters());
}

void Engine::AddMaterial(const std::string &MaterialName) {
  auto MaterialPtr = MaterialManager::getSingleton().getByName(MaterialName);
  if (MaterialPtr) AddMaterial(MaterialPtr);
}

void Engine::AddCamera(Camera *OgreCameraPtr) {}

void Engine::AddSinbad(Camera *OgreCameraPtr) {
  Sinbad = make_unique<SinbadCharacterController>(OgreCameraPtr);
  InputSequencer::GetInstance().RegObserver(Sinbad.get());
}

void Engine::AddForests(Forests::PagedGeometry *PGPtr, const std::string &MaterialName) {
  PagedGeometryList.push_back(unique_ptr<Forests::PagedGeometry>(PGPtr));
  if (!MaterialName.empty()) AddMaterial(MaterialName);
}

void Engine::AddTerrain(TerrainGroup *TGP) { OgreTerrainList.reset(TGP); }

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
    for (int j = 0; j < 3; j++) HosekParams[i][j] = States[j]->configs[j][i];
  HosekParams[9] = Vector3(sky.StateX->radiances[0], sky.StateY->radiances[1], sky.StateZ->radiances[2]);

  auto SkyMaterial = MaterialManager::getSingleton().getByName("SkyBox");
  if (!SkyMaterial) return;

  auto FpParams = SkyMaterial->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
  if (FpParams) SkyBoxFpParams = FpParams;

  FpParams->setIgnoreMissingParams(true);
  for (int i = 0; i < 10; i++) FpParams->setNamedConstant(HosikParamList[i], HosekParams[i]);
}

void Engine::RegComponent(SystemI *ComponentPtr) {
  if (ComponentPtr) ComponentList.push_back(ComponentPtr);
}

void Engine::UnRegComponent(SystemI *ComponentPtr) {
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
  ShowMouseCursor(true);
}

void Engine::OnMenuOff() {
  PhysicsPtr->OnResume();
  ShowMouseCursor(false);
}

void Engine::OnCleanup() {
  for (auto &it : ComponentList) it->OnClean();
  InputSequencer::GetInstance().UnRegObserver(Sinbad.get());
  Sinbad.reset();
  PagedGeometryList.clear();
  if (OgreTerrainList) OgreTerrainList->removeAllTerrains();
  OgreTerrainList.reset();
  auto *TGO = Ogre::TerrainGlobalOptions::getSingletonPtr();
  if (TGO) delete TGO;
  if (OgreSceneManager) OgreSceneManager->setShadowTechnique(SHADOWTYPE_NONE);
  if (OgreSceneManager) OgreSceneManager->clearScene();
  ResourceGroupManager::getSingleton().unloadResourceGroup(GroupName);
  GpuFpParams.clear();
  GpuFpParams.shrink_to_fit();
  GpuVpParams.clear();
  GpuVpParams.shrink_to_fit();
  SkyBoxFpParams.reset();

  InitShadowSettings();
}

void Engine::Update(float PassedTime) {
  if (Paused) return;
  if (Sinbad) Sinbad->Update(PassedTime);
  for (auto &it : PagedGeometryList) it->update();

  static Camera *CameraPtr = Root::getSingleton().getSceneManager("Default")->getCamera("Default");
  static Matrix4 MVP;
  static Matrix4 MVPprev;

  MVPprev = MVP;
  MVP = CameraPtr->getProjectionMatrixWithRSDepth() * CameraPtr->getViewMatrix();

  for (auto &it : GpuVpParams) it->setNamedConstant("uWorldViewProjPrev", MVPprev);

  if (SkyNeedsUpdate && SkyBoxFpParams)
    for (int i = 0; i < 10; i++) SkyBoxFpParams->setNamedConstant(HosikParamList[i], HosekParams[i]);

  for (auto &it : ComponentList) it->OnUpdate(PassedTime);

  ImGuiOverlay::NewFrame();

  //  ImGui::ShowDemoWindow();
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

bool Engine::IsWindowFullscreen() { return WindowFullScreen; }

void Engine::SetWindowCaption(const char *Caption) { SDL_SetWindowTitle(SDLWindowPtr, Caption); }

#ifdef MOBILE
void Engine::AndroidRestoreWindow() {
  SDL_DisplayMode DM;
  SDL_GetDesktopDisplayMode(CurrentDisplay, &CurrentSDLDisplayMode);
  OgreRenderWindowPtr->resize(static_cast<int>(CurrentSDLDisplayMode.w), static_cast<int>(CurrentSDLDisplayMode.h));
}
#endif

void Engine::GrabCursor(bool grab) {
#ifndef MOBILE  // This breaks input @Android >9.0
  if (SDLWindowPtr) {
    SDL_ShowCursor(!grab);
    SDL_SetWindowGrab(SDLWindowPtr, static_cast<SDL_bool>(grab));
    SDL_SetRelativeMouseMode(static_cast<SDL_bool>(grab));
  }
#endif
}

void Engine::ShowCursor(bool show) {
#ifndef MOBILE  // This breaks input @Android >9.0
  if (SDLWindowPtr) SDL_ShowCursor(show);
#endif
}

#if defined(DESKTOP) && defined(DEBUG)
static inline bool IsHidden(const fs::path &p) {
  string name = p.filename().string();
  return name.compare("..") && name.compare(".") && name[0] == '.';
}

static inline string FindPath(string Path, int Depth = 4) {
  string result = Path;

  for (int i = 0; i < Depth; i++) {
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

void AddLocation(const std::string &Path, const std::string &GroupName = Ogre::RGN_DEFAULT) {
  const string FILE_SYSTEM = "FileSystem";
  const string ZIP = "Zip";

  std::vector<string> file_list;
  std::vector<string> dir_list;
  std::vector<tuple<string, string, string>> resource_list;
  auto &RGM = ResourceGroupManager::getSingleton();

  string path = FindPath(Path);

  if (fs::exists(path)) {
    if (fs::is_directory(path))
      resource_list.push_back(make_tuple(path, FILE_SYSTEM, GroupName));
    else if (fs::is_regular_file(path) && fs::path(path).extension() == ".zip")
      RGM.addResourceLocation(path, ZIP, GroupName);
  }

  for (const auto &it : resource_list) {
    RGM.addResourceLocation(get<0>(it), get<1>(it), get<2>(it));
    dir_list.push_back(get<0>(it));

    for (fs::recursive_directory_iterator end, jt(get<0>(it)); jt != end; ++jt) {
      const string full_path = jt->path().string();
      const string file_name = jt->path().filename().string();
      const string extention = jt->path().filename().extension().string();

      if (jt->is_directory()) {
        if (IsHidden(jt->path())) {
          jt.disable_recursion_pending();
          continue;
        }

        dir_list.push_back(file_name);

        RGM.addResourceLocation(full_path, FILE_SYSTEM, get<2>(it));
      } else if (jt->is_regular_file()) {
        if (IsHidden(jt->path())) continue;

        if (extention == ".zip") {
          RGM.addResourceLocation(full_path, ZIP, get<2>(it));
        } else {
          file_list.push_back(file_name);
        }
      }
    }
  }
}

#endif  // DESKTOP

void Engine::InitResourceLocation() {
  auto &RGM = ResourceGroupManager::getSingleton();
#if defined(ANDROID)
  RGM.addResourceLocation("/Programs.zip", "APKZip", RGN_INTERNAL);
  RGM.addResourceLocation("/Assets.zip", "APKZip", RGN_DEFAULT);
#elif defined(DEBUG)
  const string ProgramsDir = "Source/Programs";
  const string AssetsDir = "Example/Assets";
  AddLocation(ProgramsDir, RGN_INTERNAL);
  if (RenderSystemIsGLES2())
    AddLocation("Source/GLSLES", RGN_INTERNAL);
  else
    AddLocation("Source/GLSL", RGN_INTERNAL);
  AddLocation(AssetsDir);
#else
  RGM.addResourceLocation("Programs.zip", "Zip", RGN_INTERNAL);
  RGM.addResourceLocation("Assets.zip", "Zip", RGN_DEFAULT);
#endif
}

Engine &GetEngine() {
  static auto &EnginePtr = Engine::GetInstance();
  return EnginePtr;
}

Physics &GetPhysics() { return GetComponent<Physics>(); }

Sound &GetAudio() { return GetComponent<Sound>(); }

std::string Engine::GetWindowCaption() { return WindowCaption; }

int Engine::GetWindowSizeX() { return WindowWidth; }

int Engine::GetWindowSizeY() { return WindowHeight; }

}  // namespace Glue
