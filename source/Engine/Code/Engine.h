/// created by Andrey Vasiliev

#pragma once

#include "AudioManager.h"
#include "CompositorManager.h"
#include "Observer.h"
#include "PhysicsManager.h"
#include "SinbadCharacterController.h"
#include "Singleton.h"
#include "VideoManager.h"
//extern "C" {
//#include <SDL2/SDL_video.h>
//}
//#ifdef OGRE_BUILD_COMPONENT_TERRAIN
//#include <Terrain/OgreTerrainGroup.h>
//#endif
//#include "PagedGeometry/PagedGeometry.h"

int ErrorWindow(const char* caption, const char* text);

namespace Glue {

/// Forward declaration
class Engine;

/// Global component getters
Engine& GetEngine();
PhysicsManager& GetPhysics();
AudioManager& GetAudio();

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

  void RegComponent(SystemI* component);
  void UnRegComponent(SystemI* component);
  float GetHeight(float x, float z);
  void AddEntity(Ogre::Entity* EntityPtr);
  void AddMaterial(Ogre::MaterialPtr material);
  void AddMaterial(const std::string& MaterialName);
  void AddCamera(Ogre::Camera* OgreCameraPtr);
  void AddSinbad(Ogre::Camera* OgreCameraPtr);
//  void AddForests(Forests::PagedGeometry* PGPtr, const std::string& MaterialName = "");
//  void AddTerrain(Ogre::TerrainGroup* TGP);

 protected:
//  std::unique_ptr<Ogre::TerrainGroup> terrainGroup;
//  std::vector<std::unique_ptr<Forests::PagedGeometry>> pgList;
  std::unique_ptr<SinbadCharacterController> sinbad;
  bool paused = false;

  /// Components
  std::unique_ptr<VideoManager> video;
  std::unique_ptr<CompositorManager> compositor;
  std::unique_ptr<PhysicsManager> physics;
  std::unique_ptr<AudioManager> audio;
  std::vector<SystemI*> componentList;

  /// Global access to base components
  friend Engine& GetEngine();
  friend PhysicsManager& GetPhysics();
  friend AudioManager& GetAudio();
};

}  // namespace Glue
