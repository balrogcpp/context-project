/// created by Andrey Vasiliev

#pragma once

#include "Audio.h"
#include "ImGuiInputListener.h"
#include "Observer.h"
#include "Physics.h"
#include "SinbadCharacterController.h"
#include "Singleton.h"
#include "Video.h"
extern "C" {
#include <SDL2/SDL_video.h>
}
#include <memory>
#include <vector>
#ifdef OGRE_BUILD_COMPONENT_OVERLAY
#include <Overlay/OgreImGuiOverlay.h>
#include <Overlay/OgreOverlaySystem.h>
#endif
#ifdef OGRE_BUILD_COMPONENT_TERRAIN
#include <Terrain/OgreTerrainGroup.h>
#endif
#include "PagedGeometry/PagedGeometry.h"

int ErrorWindow(const char* WindowCaption, const char* MessageText);

namespace Glue {

/// Forward declaration
class Engine;

/// Global component getters
Engine& GetEngine();
Physics& GetPhysics();
Audio& GetAudio();

///
Ogre::Camera* OgreCamera();

///
Ogre::SceneNode* OgreCameraNode();

///
Ogre::Vector3 SunDirection();

///
int WindowSizeX();

///
int WindowSizeY();

///
std::string WindowCaption();

struct CompositorFX {
  std::string Name;
  bool Enabled = false;
  std::string EnableParam;
  std::string OutputCompositor;
  std::vector<std::string> CompositorChain;
};

class Engine final : public Singleton<Engine>, public Ogre::RenderTargetListener {
 public:
  Engine();
  virtual ~Engine();

  void Init();
  void Capture();
  void OnPause();
  void OnMenuOn();
  void OnMenuOff();
  void OnResume();
  void OnCleanup();
  void Update(float PassedTime);

  void RenderFrame();

  void RegComponent(SystemI* ComponentPtr);
  void UnRegComponent(SystemI* ComponentPtr);
  void SetFullscreen(bool Fullscreen);
  bool IsWindowFullscreen();
  void ResizeWindow(int Width, int Height);
  void SetWindowCaption(const char* Caption);
  void ShowCursor(bool Show);
  void GrabCursor(bool Grab);
  std::string GetWindowCaption();
  int GetWindowSizeX();
  int GetWindowSizeY();
  float GetHeight(float x, float z);
  void AddEntity(Ogre::Entity* EntityPtr);
  void AddMaterial(Ogre::MaterialPtr material);
  void AddMaterial(const std::string& MaterialName);
  void AddCamera(Ogre::Camera* OgreCameraPtr);
  void AddSinbad(Ogre::Camera* OgreCameraPtr);
  void AddForests(Forests::PagedGeometry* PGPtr, const std::string& MaterialName = "");
  void AddTerrain(Ogre::TerrainGroup* TGP);
  void AddSkyBox();
  Ogre::Vector3 GetSunPosition();

 protected:
  /// Creates and initiates Compositor component. Internal
  void EnableEffect(const std::string& FX, bool Enable);

  ///
  void InitCompositor();

  ///
  Ogre::Root* ogreRoot = nullptr;
  Ogre::SceneManager* sceneManager = nullptr;
  Ogre::RenderWindow* ogreWindow = nullptr;
  Ogre::Camera* camera = nullptr;
  Ogre::Viewport* viewport = nullptr;

  ///
  std::shared_ptr<Ogre::PSSMShadowCameraSetup> pssmSetup;
  std::vector<float> pssmSplitPointList;
  const int PSSM_SPLITS = 3;

  ///
  std::unique_ptr<Ogre::TerrainGroup> terrainGroup;
  std::vector<std::unique_ptr<Forests::PagedGeometry>> pgList;
  std::unique_ptr<SinbadCharacterController> sinbad;
  Ogre::GpuProgramParametersSharedPtr skyBoxFpParams;
  bool skyNeedsUpdate = false;
  const std::array<const char*, 10> hosekParamList{"A", "B", "C", "D", "E", "F", "G", "H", "I", "Z"};
  std::array<Ogre::Vector3, 10> hosekParams;
  std::vector<Ogre::GpuProgramParametersSharedPtr> gpuFpParams;
  std::vector<Ogre::GpuProgramParametersSharedPtr> gpuVpParams;
  bool paused = false;

  ///
  std::string windowCaption = "Example0";
  int windowWidth = 1270;
  int windowHeight = 720;
  bool windowFullScreen = false;
  int screenWidth = 0;
  int screenHeight = 0;
  bool windowVsync = true;
  int currentDisplay = 0;
  std::vector<SDL_DisplayMode> sdlMonitorList;
  SDL_Window* sdlWindow = nullptr;
  uint32_t sdlWindowFlags = 0;
  int sdlWindowPositionFlag = SDL_WINDOWPOS_CENTERED;

  /// Compositor stuff
  Ogre::CompositorManager* compositorManager = nullptr;
  Ogre::CompositorChain* compositorChain = nullptr;
  std::map<std::string, CompositorFX> compositorList;

  /// ImGui stuff
  std::unique_ptr<ImGuiInputListener> imguiListener;
  Ogre::ImGuiOverlay* imguiOverlay = nullptr;

  /// Components
  std::unique_ptr<Physics> physics;
  std::unique_ptr<Audio> audio;
  std::vector<SystemI*> componentList;

  /// Global access to base components
  friend Engine& GetEngine();
  friend Physics& GetPhysics();
  friend Audio& GetAudio();
};

}  // namespace Glue
