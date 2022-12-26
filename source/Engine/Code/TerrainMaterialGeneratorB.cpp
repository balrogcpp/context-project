/// created by Andrey Vasiliev

#include "pch.h"
#include "TerrainMaterialGeneratorB.h"
#include "SystemLocator.h"
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainMaterialGenerator.h>

namespace Ogre {
TerrainMaterialGeneratorB::TerrainMaterialGeneratorB() {
  mProfiles.push_back(OGRE_NEW SM2Profile(this, "SM2", "Profile for rendering on Shader Model 2 capable cards"));
  setActiveProfile(mProfiles.back());
}
TerrainMaterialGeneratorB::~TerrainMaterialGeneratorB() {}

TerrainMaterialGeneratorB::SM2Profile::SM2Profile(TerrainMaterialGenerator *parent, const String &name, const String &desc)
    : Profile(parent, name, desc), enableLightmap(false), enableNormalmap(false), enableCompositeMap(false), terrainMaxLayers(4) {}
TerrainMaterialGeneratorB::SM2Profile::~SM2Profile() {}

bool TerrainMaterialGeneratorB::SM2Profile::isVertexCompressionSupported() const { return false; }
void TerrainMaterialGeneratorB::SM2Profile::setLightmapEnabled(bool enabled) { enableLightmap = enabled; }
bool TerrainMaterialGeneratorB::SM2Profile::isLightmapEnabled() { return enableLightmap; }
void TerrainMaterialGeneratorB::SM2Profile::setNormalmapEnabled(bool enabled) { enableNormalmap = enabled; }
bool TerrainMaterialGeneratorB::SM2Profile::isNormalmapEnabled() { return enableNormalmap; }
void TerrainMaterialGeneratorB::SM2Profile::setCompositeMapEnabled(bool enabled) { enableCompositeMap = enabled; }
bool TerrainMaterialGeneratorB::SM2Profile::isCompositeMapEnabled() { return enableCompositeMap; }

void TerrainMaterialGeneratorB::SM2Profile::requestOptions(Terrain *terrain) {
  terrain->_setMorphRequired(true);
  terrain->_setNormalMapRequired(enableNormalmap);
  terrain->_setLightMapRequired(enableLightmap, true);
  terrain->_setCompositeMapRequired(enableCompositeMap);
}

MaterialPtr TerrainMaterialGeneratorB::SM2Profile::generate(const Terrain *terrain) {
  std::string materialName = "Terrain";

  if (isVertexCompressionSupported()) {
    materialName += "_vc";
  }

  static unsigned long long GENERATOR = 0;
  std::string newName = materialName + std::to_string(GENERATOR++);

  auto newMaterial = MaterialManager::getSingleton().getByName(materialName)->clone(newName);
  auto *pass = newMaterial->getTechnique(0)->getPass(0);
  auto *texState = pass->getTextureUnitState("GlobalNormal");
  auto *texState2 = pass->getTextureUnitState("GlobalLight");
  float uvScale = 2.0f * (terrain->getSize() - 1) / terrain->getWorldSize();

  if (pass->hasFragmentProgram()) {
    const auto &fp = pass->getFragmentProgramParameters();

    fp->setNamedConstant("uTexScale", uvScale);
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


  if (texState && SM2Profile::isNormalmapEnabled()) {
    texState->setTexture(terrain->getTerrainNormalMap());
  }

  if (texState2 && SM2Profile::isLightmapEnabled()) {
    texState2->setTexture(terrain->getLightmap());
  }

  return newMaterial;
}

MaterialPtr TerrainMaterialGeneratorB::SM2Profile::generateForCompositeMap(const Terrain *terrain) {
  std::string materialName = "TerrainCustom";
  static unsigned long long GENERATOR = 0;
  std::string newName = materialName + "Composite" + std::to_string(GENERATOR++);

  return MaterialManager::getSingleton().getByName(materialName)->clone(newName);
}
}  // namespace Ogre
