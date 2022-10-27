/// created by Andrey Vasiliev

#include "pch.h"
#include "TerrainManager.h"
#include "DotSceneLoaderB/DotSceneLoaderB.h"
#include "PhysicsManager.h"
#include "Platform.h"
#include "TerrainMaterialGeneratorB.h"

using namespace std;

namespace Glue {
TerrainManager::TerrainManager() {}
TerrainManager::~TerrainManager() {}

void TerrainManager::OnSetUp() {
  ogreRoot = Ogre::Root::getSingletonPtr();
  OgreAssert(ogreRoot, "[TerrainManager] ogreRoot is not initialised");
  ogreSceneManager = ogreRoot->getSceneManager("Default");
  OgreAssert(ogreSceneManager, "[TerrainManager] ogreSceneManager is not initialised");
  OgreAssert(ogreSceneManager->hasCamera("Default"), "[TerrainManager] ogreCamera is not initialised");
  ogreCamera = ogreSceneManager->getCamera("Default");

  auto *terrainGlobalOption = Ogre::TerrainGlobalOptions::getSingletonPtr();
  if (!terrainGlobalOption) {
    terrainGlobalOption = new Ogre::TerrainGlobalOptions();
    terrainGlobalOption->setDefaultMaterialGenerator(make_shared<Ogre::TerrainMaterialGeneratorB>());
    terrainGlobalOption->setUseRayBoxDistanceCalculation(true);
  }
}

void TerrainManager::RegTerrainGroup(Ogre::TerrainGroup *terrainGroup) {
  OgreAssert(terrainGroup, "[TerrainManager] terrainGroup can't be NULL");
  OgreAssert(!ogreTerrainGroup, "[TerrainManager] terrainGroup already registered");
  ogreTerrainGroup.reset(terrainGroup);
}

void TerrainManager::ProcessTerrainCollider(Ogre::TerrainGroup *terrainGroup) {
  GetComponent<PhysicsManager>().CreateTerrainHeightfieldShape(terrainGroup);
}

void TerrainManager::LoadTerrainGroupLegacy(int x, int y, const std::string &filename) {
  ogreTerrainGroup->loadLegacyTerrain(filename, x, y, true);
  ogreTerrainGroup->setOrigin(Ogre::Vector3::ZERO);
  ogreTerrainGroup->freeTemporaryResources();
}

void TerrainManager::OnUpdate(float time) {}
void TerrainManager::OnClean() {
  if (ogreTerrainGroup) {
    ogreTerrainGroup->removeAllTerrains();
    ogreTerrainGroup.reset();
  }
}

}  // namespace Glue
