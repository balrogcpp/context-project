/// created by Andrey Vasiliev

#pragma once
#include "Component.h"
#include "SDLListener.h"
#include <Ogre.h>
#include <OgreImGuiOverlay.h>

// forward declaration
namespace Ogre {
class Terrain;
class TerrainGroup;
}  // namespace Ogre

namespace BtOgre {
class DynamicsWorld;
class DebugDrawer;
}  // namespace BtOgre

namespace OgreOggSound {
class Root;
class OgreOggSoundManager;
}  // namespace OgreOggSound

namespace gge {
class VideoComponent final : public WindowListener, public Component<VideoComponent> {
 protected:
  class ShaderResolver;
  class DefaultLogListener;
  class MutedLogListener;

 public:
  VideoComponent();
  virtual ~VideoComponent();

  void RenderFrame();
  //  Window& GetWindow(int number = 0);
  //  Window& GetMainWindow();
  void ShowWindow(bool show);

  void UnloadResources();
  void ClearScene();

  void EnableGamepadNav(bool enable);
  void EnableKeyboardNav(bool enable);
  void ShowOverlay(bool show);
  void ShowOgreProfiler(bool show);
  ImFont* AddOverlayFont(const std::string& name, const int size = 0, const std::string& group = Ogre::RGN_AUTODETECT,
                         const ImFontConfig* cfg = nullptr, const ImWchar* ranges = nullptr);
  void RebuildOverlayFontAtlas();

  void EnableShadows(bool enable);
  bool IsShadowEnabled();
  void SetShadowTexSize(unsigned short size);
  unsigned short GetShadowTexSize();
  void SetTexFiltering(unsigned int type, int anisotropy);
  std::vector<float> GetPSSMSplitPoints();

  int GetDisplaySizeX(int index);
  int GetDisplaySizeY(int index);
  float GetDisplayDPI(int index);
  float GetDisplayHDPI(int index);
  float GetDisplayVDPI(int index);

  /// System impl
  void OnSetUp() override;
  void OnClean() override;
  void OnUpdate(float time) override;

 protected:
  void CheckGPU();
  void CreateWindow();
  void InitOgreRoot();
  void InitSDL();
  void InitOgreRTSS();
  void InitOgreOverlay();
  void LoadResources();
  void InitOgreSceneManager();
  void InitOgreAudio();
  void InitBtOgre();

  /// WindowListener impl
  void OnSizeChanged(int x, int y, uint32_t id) override;
  void OnEvent(const SDL_Event& event) override;

  //  struct Window {
  std::string title = "My Window";
  bool fullscreen = false;
  int display = 0;
  int sizeX = 1270;
  int sizeY = 720;
  uint32_t sdlFlags = SDL_WINDOW_HIDDEN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE;
  int vsyncInt = 1;
  uint32_t id;
  //  };

  SDL_Window* sdlWindow = nullptr;
  SDL_GLContext glContext = nullptr;
  Ogre::RenderWindow* ogreWindow = nullptr;
  Ogre::RenderTarget* renderTarget = nullptr;
  Ogre::Viewport* ogreViewport = nullptr;

  Ogre::Root* ogreRoot = nullptr;
  Ogre::SceneManager* sceneManager = nullptr;
  Ogre::Camera* camera = nullptr;
  Ogre::ImGuiOverlay* imguiOverlay = nullptr;
  std::shared_ptr<Ogre::TerrainGroup> ogreTerrainGroup;
  std::unique_ptr<OgreOggSound::Root> audioRoot;
  OgreOggSound::OgreOggSoundManager* oggSoundManager = nullptr;
  std::unique_ptr<BtOgre::DynamicsWorld> dynamicWorld;
  std::unique_ptr<BtOgre::DebugDrawer> debugDrawer;

  std::string ogreLogFile;
  unsigned short ogreMinLogLevel;
  std::unique_ptr<Ogre::LogManager> ogreLogManager;
  std::unique_ptr<ShaderResolver> shaderResolver;
  Ogre::Real shadowNearDistance;
  Ogre::Real shadowFarDistance;
  bool shadowEnabled;
  unsigned short shadowTexSize;
  std::shared_ptr<Ogre::PSSMShadowCameraSetup> pssmSetup;
  std::vector<Ogre::Real> pssmSplitPointList;
  int shadowTexCount;
  int pssmSplitCount;
  float pssmSplitPadding;
};
}  // namespace gge
