/// created by Andrey Vasiliev

#pragma once
#include "System.h"
#include <Terrain/OgreTerrainGroup.h>

namespace gge {

class TerrainManager final : public System<TerrainManager> {
 public:
  TerrainManager();
  virtual ~TerrainManager();

  float GetHeight(float x, float z);
  void RegTerrainGroup(std::shared_ptr<Ogre::TerrainGroup> terrainGroup);
  void LoadTerrainGroupLegacy(int x, int y, const std::string &filename);
  void ProcessTerrainCollider(Ogre::TerrainGroup *terrainGroup);

  /// System impl
  void OnSetUp() override;
  void OnUpdate(float time) override;
  void OnClean() override;

 protected:
  std::shared_ptr<Ogre::TerrainGroup> ogreTerrainGroup;
  Ogre::Root *ogreRoot = nullptr;
  Ogre::SceneManager *sceneManager = nullptr;
  Ogre::Camera *camera = nullptr;
};

}  // namespace gge
