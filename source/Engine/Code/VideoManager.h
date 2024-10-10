/// created by Andrey Vasiliev

#pragma once
#include "SDLListener.h"
#include "System.h"
#include "Window.h"
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
class VideoManager final : public WindowListener, public System<VideoManager> {
 protected:
  class ShaderResolver;

 public:
  VideoManager();
  virtual ~VideoManager();

  void RenderFrame();
  Window& GetWindow(int number = 0);
  Window& GetMainWindow();
  void ShowWindow(bool show, int index = 0);

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
  void MakeWindow();
  void InitOgreRoot();
  void InitSDL();
  void InitOgreRTSS();
  void InitOgreOverlay();
  void LoadResources();
  void InitOgreSceneManager();
  void InitTerrain();
  void InitOgreAudio();
  void InitBtOgre();

  /// WindowListener impl
  void OnEvent(const SDL_Event& event) override;

  Window* mainWindow = nullptr;
  std::vector<Window> windowList;
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

  Ogre::Root* ogreRoot = nullptr;
  Ogre::SceneManager* sceneManager = nullptr;
  Ogre::Camera* camera = nullptr;
  Ogre::ImGuiOverlay* imguiOverlay = nullptr;
  std::shared_ptr<Ogre::TerrainGroup> ogreTerrainGroup;
  std::unique_ptr<OgreOggSound::Root> audioRoot;
  std::unique_ptr<BtOgre::DynamicsWorld> dynamicWorld;
  std::unique_ptr<BtOgre::DebugDrawer> debugDrawer;
};
}  // namespace gge
