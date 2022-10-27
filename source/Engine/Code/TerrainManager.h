/// created by Andrey Vasiliev

#pragma once
#include "System.h"
#include <Terrain/OgreTerrainGroup.h>

namespace Glue {

class TerrainManager final : public System<TerrainManager> {
 public:
  TerrainManager();
  virtual ~TerrainManager();

  void OnSetUp() override;
  void OnUpdate(float time) override;
  void OnClean() override;

  void RegTerrainGroup(Ogre::TerrainGroup *terrainGroup);
  void ProcessTerrainGroupLegacy(int x, int y, const std::string &filename);
  void ProcessTerrainCollider(Ogre::TerrainGroup *terrainGroup);

 protected:
  Ogre::TerrainGroup *ogreTerrainGroup = nullptr;

  Ogre::Root *ogreRoot = nullptr;
  Ogre::SceneManager *ogreSceneManager = nullptr;
  Ogre::Camera *ogreCamera = nullptr;
};

}  // namespace Glue
