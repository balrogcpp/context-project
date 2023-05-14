/// created by Andrey Vasiliev

#include "pch.h"
#include "TerrainManager.h"
#include "DotSceneLoaderB/DotSceneLoaderB.h"
#include "PhysicsManager.h"
#include <Ogre.h>

using namespace std;

namespace Ogre {

class TerrainMaterialGeneratorB final : public Ogre::TerrainMaterialGenerator {
 private:
  class SM2Profile : public Ogre::TerrainMaterialGenerator::Profile {
   public:
    SM2Profile(TerrainMaterialGenerator *parent, const Ogre::String &name, const Ogre::String &desc) : Profile(parent, name, desc) {}
    ~SM2Profile() {}

    Ogre::uint8 getMaxLayers(const Ogre::Terrain *terrain) const override { return terrainMaxLayers; }
    void updateParams(const Ogre::MaterialPtr &mat, const Ogre::Terrain *terrain) override {}
    void updateParamsForCompositeMap(const Ogre::MaterialPtr &mat, const Ogre::Terrain *terrain) override {}
    bool isVertexCompressionSupported() const override { return enableVertexCompression; }
    void setLightmapEnabled(bool enabled) override { enableLightmap = enabled; }

    void requestOptions(Ogre::Terrain *terrain) override {
      terrain->_setMorphRequired(true);
      terrain->_setNormalMapRequired(enableNormalmap);
      terrain->_setLightMapRequired(enableLightmap, true);
      terrain->_setCompositeMapRequired(enableCompositeMap);
    }

    Ogre::MaterialPtr generate(const Ogre::Terrain *terrain) override {
      const std::string materialName = "TerrainCustom";
      static unsigned long long generator = 0;
      std::string newName = materialName + std::to_string(generator++);

      auto newMaterial = Ogre::MaterialManager::getSingleton().getByName(materialName)->clone(newName);
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

          Ogre::Matrix4 posIndexToObjectSpace = terrain->getPointTransform();
          vp->setNamedConstant("posIndexToObjectSpace", posIndexToObjectSpace);
          Ogre::Real baseUVScale = 1.0f / (terrain->getSize() - 1);
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

    Ogre::MaterialPtr generateForCompositeMap(const Ogre::Terrain *terrain) override {
      std::string materialName = "TerrainCustom";
      static unsigned long long generator = 0;
      std::string newName = materialName + "Comp" + std::to_string(generator++);

      return Ogre::MaterialManager::getSingleton().getByName(materialName)->clone(newName);
    }

   protected:
    bool enableLightmap = true;
    const bool enableNormalmap = true;
    const bool enableCompositeMap = false;
    const bool enableVertexCompression = true;
    const int8_t terrainMaxLayers = 1;
  };

 public:
  TerrainMaterialGeneratorB() {
    mProfiles.push_back(OGRE_NEW SM2Profile(this, "SM2", "Profile for rendering on Shader Model 2 capable cards"));
    setActiveProfile(mProfiles.back());
  }
  virtual ~TerrainMaterialGeneratorB() {}
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
}  // namespace gge
