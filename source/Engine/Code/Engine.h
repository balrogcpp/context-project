/// created by Andrey Vasiliev

#pragma once

#include "Audio.h"
#include "ImGuiInputListener.h"
#include "Observer.h"
#include "Physics.h"
#include "Singleton.h"
#include "Video.h"
extern "C" {
#include <SDL2/SDL_video.h>
}
#include <imgui.h>
#include <memory>
#include <vector>
#ifdef OGRE_BUILD_COMPONENT_OVERLAY
#include <Overlay/OgreImGuiOverlay.h>
#include <Overlay/OgreOverlay.h>
#include <Overlay/OgreOverlayManager.h>
#include <Overlay/OgreOverlaySystem.h>
#endif

namespace Ogre {
class TerrainGroup;
}  // namespace Ogre

namespace Forests {
class PagedGeometry;
class PageLoader;
class GeometryPage;
}  // namespace Forests

namespace Glue {
class CameraMan;
class Physics;
class Audio;
class SinbadCharacterController;
}  // namespace Glue

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

  /// Initialized all components. Must be called once after Engine object created
  void Init();

  /// Called every frame to read input
  void Capture();

  /// Set all components to Pause state
  void OnPause();

  /// Called when in-game menu turned on
  void OnMenuOn();

  /// Called when in-game menu turned off
  void OnMenuOff();

  /// Called when game is resumed
  void OnResume();

  /// Called on game exit
  void OnCleanup();

  /// Called every frame
  void Update(float PassedTime);

  /// Called to start render
  void RenderFrame();

  /// Internal function to add component
  void RegComponent(SystemI* ComponentPtr);

  /// Internal function to unload component
  void UnRegComponent(SystemI* ComponentPtr);

  ///
  void SetFullscreen(bool Fullscreen);

  ///
  bool IsWindowFullscreen();

  ///
  void ResizeWindow(int Width, int Height);

  ///
  void SetWindowCaption(const char* Caption);

  ///
  void ShowCursor(bool Show);

  ///
  void GrabCursor(bool Grab);

  ///
  std::string GetWindowCaption();

  ///
  int GetWindowSizeX();

  ///
  int GetWindowSizeY();

  ///
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
  std::string RenderSystemName;
  Ogre::Root* OgreRoot = nullptr;
  Ogre::SceneNode* RootNode = nullptr;
  Ogre::SceneManager* OgreSceneManager = nullptr;
  Ogre::RenderWindow* OgreRenderWindowPtr = nullptr;
  Ogre::RenderTarget* OgreRenderTargetPtr = nullptr;
  Ogre::Camera* OgreCamera = nullptr;
  Ogre::Viewport* OgreViewport = nullptr;
  std::string GroupName = Ogre::RGN_DEFAULT;

  ///
  std::shared_ptr<Ogre::PSSMShadowCameraSetup> PSSMSetupPtr;
  std::vector<float> PSSMSplitPointList;
  const int PSSM_SPLITS = 3;

  ///
  std::unique_ptr<Ogre::TerrainGroup> OgreTerrainList;
  std::vector<std::unique_ptr<Forests::PagedGeometry>> PagedGeometryList;
  std::unique_ptr<SinbadCharacterController> Sinbad;
  Ogre::GpuProgramParametersSharedPtr SkyBoxFpParams;
  bool SkyNeedsUpdate = false;
  const std::array<const char*, 10> HosikParamList{"A", "B", "C", "D", "E", "F", "G", "H", "I", "Z"};
  std::array<Ogre::Vector3, 10> HosekParams;
  std::vector<Ogre::GpuProgramParametersSharedPtr> GpuFpParams;
  std::vector<Ogre::GpuProgramParametersSharedPtr> GpuVpParams;
  bool Paused = false;

  ///
  std::string windowCaption = "Example0";
  int windowWidth = 1270;
  int windowHeight = 720;
  bool windowFullScreen = false;
  int screenWidth = 0;
  int screenHeight = 0;
  bool windowVsync = true;
  int currentDisplay = 0;
  SDL_DisplayMode displayMode;
  std::vector<SDL_DisplayMode> sdlMonitorList;
  SDL_Window* sdlWindow = nullptr;
  SDL_GLContext sdlContext = nullptr;
  uint32_t sdlWindowFlags = 0;
  int windowPositionFlag = SDL_WINDOWPOS_CENTERED;

  /// Compositor stuff
  Ogre::CompositorManager* compositorManager = nullptr;
  Ogre::CompositorChain* compositorChain = nullptr;
  int ViewportSizeX = 0;
  int ViewportSizeY = 0;
  std::map<std::string, CompositorFX> compositorList;

  /// ImGui stuff
  std::unique_ptr<ImGuiInputListener> imguiListener;
  Ogre::ImGuiOverlay* imguiOverlay = nullptr;
  Ogre::OverlaySystem* overlay = nullptr;

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
