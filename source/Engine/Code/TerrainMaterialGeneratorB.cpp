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
    : Profile(parent, name, desc), enableLightmap(false), enableNormalmap(true), terrainMaxLayers(4) {}

TerrainMaterialGeneratorB::SM2Profile::~SM2Profile() {}

bool TerrainMaterialGeneratorB::SM2Profile::isVertexCompressionSupported() const {
  static bool result = !Ogre::TerrainGlobalOptions::getSingletonPtr()->getCastsDynamicShadows();
  return result;
}

void TerrainMaterialGeneratorB::SM2Profile::setLightmapEnabled(bool enabled) { enableLightmap = enabled; }
bool TerrainMaterialGeneratorB::SM2Profile::isLightmapEnabled() { return enableLightmap; }
void TerrainMaterialGeneratorB::SM2Profile::setNormalmapEnabled(bool enabled) { enableNormalmap = enabled; }
bool TerrainMaterialGeneratorB::SM2Profile::isNormalmapEnabled() { return enableNormalmap; }

void TerrainMaterialGeneratorB::SM2Profile::requestOptions(Terrain *terrain) {
  terrain->_setMorphRequired(true);
  terrain->_setNormalMapRequired(true);
  terrain->_setLightMapRequired(enableLightmap, true);
  terrain->_setCompositeMapRequired(false);
}

MaterialPtr TerrainMaterialGeneratorB::SM2Profile::generate(const Terrain *terrain) {
  std::string materialName;

  if (Root::getSingleton().getSceneManager("Default")->getShadowTechnique() != SHADOWTYPE_NONE)
    materialName = "TerrainCustom";
  else
    materialName = "TerrainCustomPBRS";

  if (isVertexCompressionSupported())
    materialName += "_vc";

  const std::string GENERATOR = "_0";

  if (isVertexCompressionSupported()) {
    auto material = MaterialManager::getSingleton().getByName(materialName);
    if (material->getTechnique(0)->getPass(0)->hasVertexProgram()) {
      auto vert_params = material->getTechnique(0)->getPass(0)->getVertexProgramParameters();

      Matrix4 posIndexToObjectSpace = terrain->getPointTransform();

      vert_params->setNamedConstant("posIndexToObjectSpace", posIndexToObjectSpace);
      Real baseUVScale = 1.0f / (terrain->getSize() - 1);
      vert_params->setNamedConstant("baseUVScale", baseUVScale);
    }
  }

  std::string newName = materialName + GENERATOR;

  if (MaterialManager::getSingleton().resourceExists(newName)) {
    return MaterialManager::getSingleton().getByName(newName);
  } else {
    auto newMaterial = MaterialManager::getSingleton().getByName(materialName)->clone(newName);
    auto *pass = newMaterial->getTechnique(0)->getPass(0);

    auto *texState = pass->getTextureUnitState("GlobalNormal");
    if (texState && SM2Profile::isNormalmapEnabled()) {
      texState->setTexture(terrain->getTerrainNormalMap());
    }

    auto *texState2 = pass->getTextureUnitState("GlobalLight");
    if (texState2 && SM2Profile::isLightmapEnabled()) {
      texState2->setTexture(terrain->getLightmap());
    }

    return newMaterial;
  }
}

MaterialPtr TerrainMaterialGeneratorB::SM2Profile::generateForCompositeMap(const Terrain *terrain) {
  std::string materialName = "TerrainCustom";
  const std::string GENERATOR = "_1";
  std::string newName = materialName + GENERATOR;

  if (MaterialManager::getSingleton().resourceExists(newName)) {
    return MaterialManager::getSingleton().getByName(newName);
  } else {
    auto new_material = MaterialManager::getSingleton().getByName(materialName)->clone(newName);
    return new_material;
  }
}
}  // namespace Ogre
