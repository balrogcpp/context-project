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
    : Profile(parent, name, desc), enableLightmap(true), enableNormalmap(true), terrainMaxLayers(4) {}

TerrainMaterialGeneratorB::SM2Profile::~SM2Profile() {}

bool TerrainMaterialGeneratorB::SM2Profile::isVertexCompressionSupported() const {
  return !Ogre::TerrainGlobalOptions::getSingletonPtr()->getCastsDynamicShadows();
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

  if (Root::getSingleton().getSceneManager("Default")->getShadowTechnique() != SHADOWTYPE_NONE) {
    materialName = "TerrainCustom";
  } else {
    materialName = "TerrainCustomPBRS";
  }

  if (isVertexCompressionSupported()) {
    materialName += "_vc";
  }

  static unsigned long long GENERATOR = 0;
  std::string newName = materialName + std::to_string(GENERATOR++);

  if (isVertexCompressionSupported()) {
    auto material = MaterialManager::getSingleton().getByName(materialName);
    auto *pass = material->getTechnique(0)->getPass(0);
    if (pass->hasVertexProgram()) {
      auto vert_params = pass->getVertexProgramParameters();

      Matrix4 posIndexToObjectSpace = terrain->getPointTransform();
      vert_params->setNamedConstant("posIndexToObjectSpace", posIndexToObjectSpace);
      Real baseUVScale = 1.0f / (terrain->getSize() - 1);
      vert_params->setNamedConstant("baseUVScale", baseUVScale);
    }
  }


  auto newMaterial = MaterialManager::getSingleton().getByName(materialName)->clone(newName);
  auto *pass = newMaterial->getTechnique(0)->getPass(0);
  auto *texState = pass->getTextureUnitState("GlobalNormal");
  auto *texState2 = pass->getTextureUnitState("GlobalLight");

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
