/// created by Andrey Vasiliev

#include "pch.h"
#include "TerrainManager.h"
#include "DotSceneLoaderB/DotSceneLoaderB.h"
#include "PhysicsManager.h"
#include "TerrainMaterialGeneratorB.h"
#include <Ogre.h>

using namespace std;

namespace Glue {
TerrainManager::TerrainManager() {}
TerrainManager::~TerrainManager() {
  if (ogreTerrainGroup) {
    ogreTerrainGroup->removeAllTerrains();
    ogreTerrainGroup.reset();
  }

  auto *terrainGlobalOptions = Ogre::TerrainGlobalOptions::getSingletonPtr();
  if (terrainGlobalOptions) {
    delete terrainGlobalOptions;
  }
}

void TerrainManager::OnSetUp() {
  ogreRoot = Ogre::Root::getSingletonPtr();
  ASSERTION(ogreRoot, "[TerrainManager] ogreRoot is not initialised");
  ogreSceneManager = ogreRoot->getSceneManager("Default");
  ASSERTION(ogreSceneManager, "[TerrainManager] ogreSceneManager is not initialised");
  ASSERTION(ogreSceneManager->hasCamera("Default"), "[TerrainManager] ogreCamera is not initialised");
  ogreCamera = ogreSceneManager->getCamera("Default");

  auto *terrainGlobalOptions = Ogre::TerrainGlobalOptions::getSingletonPtr();
  if (!terrainGlobalOptions) {
    terrainGlobalOptions = new Ogre::TerrainGlobalOptions();
    terrainGlobalOptions->setDefaultMaterialGenerator(make_shared<Ogre::TerrainMaterialGeneratorB>());
    terrainGlobalOptions->setUseRayBoxDistanceCalculation(true);
    terrainGlobalOptions->setCompositeMapDistance(300);
    terrainGlobalOptions->setCastsDynamicShadows(false);
  }
}

float TerrainManager::GetHeight(float x, float z) { return ogreTerrainGroup ? ogreTerrainGroup->getHeightAtWorldPosition(x, 1000, z) : 0; }

void TerrainManager::RegTerrainGroup(Ogre::TerrainGroup *terrainGroup) {
  ASSERTION(terrainGroup, "[TerrainManager] terrainGroup can't be NULL");
  ASSERTION(!ogreTerrainGroup, "[TerrainManager] terrainGroup already registered");
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
