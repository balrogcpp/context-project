/// created by Andrey Vasiliev

#pragma once
#include "System.h"
#include <Terrain/OgreTerrainGroup.h>

namespace Glue {
class TerrainManager final : public System<TerrainManager> {
 public:
  TerrainManager();
  virtual ~TerrainManager();

  float GetHeight(float x, float z);
  void RegTerrainGroup(Ogre::TerrainGroup *terrainGroup);
  void LoadTerrainGroupLegacy(int x, int y, const std::string &filename);
  void ProcessTerrainCollider(Ogre::TerrainGroup *terrainGroup);

 protected:
  /// System impl
  void OnSetUp() override;
  void OnUpdate(float time) override;
  void OnClean() override;

  std::unique_ptr<Ogre::TerrainGroup> ogreTerrainGroup;

  Ogre::Root *ogreRoot = nullptr;
  Ogre::SceneManager *ogreSceneManager = nullptr;
  Ogre::Camera *ogreCamera = nullptr;
};
}  // namespace Glue
