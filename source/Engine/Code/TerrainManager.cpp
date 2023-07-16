/// created by Andrey Vasiliev

#include "pch.h"
#include "TerrainManager.h"
#include "DotSceneLoaderB/DotSceneLoaderB.h"
#include "PhysicsManager.h"
#include <Ogre.h>
#include <OgreTerrainMaterialGenerator.h>

using namespace std;

namespace Ogre {

class TerrainMaterialGeneratorB final : public TerrainMaterialGenerator {
 private:
  class SM2Profile : public Profile {
   public:
    SM2Profile(TerrainMaterialGenerator *parent) {}
    virtual ~SM2Profile() {}
  };

  unique_ptr<SM2Profile> activeProfile;

 public:
  TerrainMaterialGeneratorB() { activeProfile.reset(new SM2Profile(this)); }
  virtual ~TerrainMaterialGeneratorB() {}

  uint8 getMaxLayers(const Terrain *terrain) const override { return terrainMaxLayers; }
  void updateParams(const MaterialPtr &mat, const Terrain *terrain) override {}
  void updateParamsForCompositeMap(const MaterialPtr &mat, const Terrain *terrain) override {}
  bool isVertexCompressionSupported() const override { return enableVertexCompression; }
  void setLightmapEnabled(bool enabled) override { enableLightmap = enabled; }

  void requestOptions(Terrain *terrain) override {
    terrain->_setMorphRequired(true);
    terrain->_setNormalMapRequired(enableNormalmap);
    terrain->_setLightMapRequired(enableLightmap, true);
    terrain->_setCompositeMapRequired(enableCompositeMap);
  }

  MaterialPtr generate(const Terrain *terrain) override {
    const string materialName = "TerrainCustom";
    static unsigned long long generator = 0;
    string newName = materialName + std::to_string(generator++);

    auto newMaterial = MaterialManager::getSingleton().getByName(materialName)->clone(newName);
    auto *pass = newMaterial->getTechnique(0)->getPass(0);
    auto *texState = pass->getTextureUnitState("TerraNormal");
    auto *texState2 = pass->getTextureUnitState("TerraLight");
    float uvScale = 2.0f * (terrain->getSize() - 1) / terrain->getWorldSize();

    if (pass->hasFragmentProgram()) {
      const auto &fp = pass->getFragmentProgramParameters();
      fp->setNamedConstant("TexScale", uvScale);
    }

    if (isVertexCompressionSupported()) {
      if (pass->hasVertexProgram()) {
        const auto &vp = pass->getVertexProgramParameters();

        Matrix4 posIndexToObjectSpace = terrain->getPointTransform();
        vp->setNamedConstant("posIndexToObjectSpace", posIndexToObjectSpace);
        Real baseUVScale = 1.0f / (terrain->getSize() - 1);
        vp->setNamedConstant("baseUVScale", baseUVScale);
      }
    }

    if (texState && enableNormalmap) {
      texState->setTexture(terrain->getTerrainNormalMap());
    }

    if (texState2 && enableLightmap) {
      texState2->setTexture(terrain->getLightmap());
    }

    return newMaterial;
  }

  MaterialPtr generateForCompositeMap(const Terrain *terrain) override {
    string materialName = "TerrainCustom";
    static unsigned long long generator = 0;
    string newName = materialName + "Comp" + std::to_string(generator++);

    return MaterialManager::getSingleton().getByName(materialName)->clone(newName);
  }

 protected:
  bool enableLightmap = true;
  const bool enableNormalmap = true;
  const bool enableCompositeMap = false;
  const bool enableVertexCompression = true;
  const int8_t terrainMaxLayers = 1;
};
}  // namespace Ogre

namespace gge {
TerrainManager::TerrainManager() {}
TerrainManager::~TerrainManager() {
  OnClean();

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
  ASSERTION(ogreSceneManager->hasCamera("Camera"), "[TerrainManager] ogreCamera is not initialised");
  ogreCamera = ogreSceneManager->getCamera("Camera");

  auto *terrainGlobalOptions = Ogre::TerrainGlobalOptions::getSingletonPtr();
  if (!terrainGlobalOptions) terrainGlobalOptions = new Ogre::TerrainGlobalOptions();

  terrainGlobalOptions->setDefaultMaterialGenerator(make_shared<Ogre::TerrainMaterialGeneratorB>());
  terrainGlobalOptions->setMaxPixelError(8);
  terrainGlobalOptions->setUseRayBoxDistanceCalculation(true);
  terrainGlobalOptions->setCompositeMapDistance(500.0);
  terrainGlobalOptions->setCastsDynamicShadows(false);
  terrainGlobalOptions->setUseVertexCompressionWhenAvailable(true);
  terrainGlobalOptions->setLightMapSize(256);
  terrainGlobalOptions->setLightMapDirection(Ogre::Vector3(40.659888, -20.704975, -30.950829).normalisedCopy());
}

float TerrainManager::GetHeight(float x, float z) { return ogreTerrainGroup ? ogreTerrainGroup->getHeightAtWorldPosition(x, 10000.0, z) : 0.0; }

void TerrainManager::RegTerrainGroup(Ogre::TerrainGroup *terrainGroup) {
  ASSERTION(terrainGroup, "[TerrainManager] terrainGroup can't be NULL");
  ASSERTION(!ogreTerrainGroup, "[TerrainManager] terrainGroup already registered");
  ogreTerrainGroup.reset(terrainGroup);
}

void TerrainManager::ProcessTerrainCollider(Ogre::TerrainGroup *terrainGroup) {
  GetComponent<PhysicsManager>().CreateTerrainHeightfieldShape(terrainGroup);
}

void TerrainManager::LoadTerrainGroupLegacy(int x, int y, const string &filename) {
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
}  // namespace gge
