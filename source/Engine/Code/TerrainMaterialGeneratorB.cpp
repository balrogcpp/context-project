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
    : Profile(parent, name, desc) {}

TerrainMaterialGeneratorB::SM2Profile::~SM2Profile() {}

bool TerrainMaterialGeneratorB::SM2Profile::isVertexCompressionSupported() const { return false; }

void TerrainMaterialGeneratorB::SM2Profile::setLightmapEnabled(bool enabled) { enableLightmap = enabled; }

void TerrainMaterialGeneratorB::SM2Profile::requestOptions(Terrain *OgreTerrainPtr) {
  OgreTerrainPtr->_setMorphRequired(true);
  OgreTerrainPtr->_setNormalMapRequired(true);
  OgreTerrainPtr->_setLightMapRequired(enableLightmap, false);
  OgreTerrainPtr->_setCompositeMapRequired(false);
}

MaterialPtr TerrainMaterialGeneratorB::SM2Profile::generate(const Terrain *OgreTerrainPtr) {
  String material_name;

  if (Root::getSingleton().getSceneManager("Default")->getShadowTechnique() != SHADOWTYPE_NONE)
    material_name = "TerrainCustom";
  else
    material_name = "TerrainCustomPBRS";

  const String GENERATOR = "_0";

  if (isVertexCompressionSupported()) {
    auto material = MaterialManager::getSingleton().getByName(material_name);
    if (material->getTechnique(0)->getPass(0)->hasVertexProgram()) {
      auto vert_params = material->getTechnique(0)->getPass(0)->getVertexProgramParameters();

      Matrix4 posIndexToObjectSpace = OgreTerrainPtr->getPointTransform();

      vert_params->setIgnoreMissingParams(true);
      vert_params->setNamedConstant("posIndexToObjectSpace", posIndexToObjectSpace);
      Real baseUVScale = 1.0f / (OgreTerrainPtr->getSize() - 1);
      vert_params->setNamedConstant("baseUVScale", baseUVScale);
    }
  }

  String new_name = material_name + GENERATOR;
  bool EnableCastShadows = TerrainGlobalOptions::getSingleton().getCastsDynamicShadows();

  if (MaterialManager::getSingleton().resourceExists(new_name)) {
    return MaterialManager::getSingleton().getByName(new_name);
  } else {
    auto new_material = MaterialManager::getSingleton().getByName(material_name)->clone(new_name);

    auto *texture_state = new_material->getTechnique(0)->getPass(0)->getTextureUnitState("GlobalNormal");
    if (texture_state) {
      texture_state->setTexture(OgreTerrainPtr->getTerrainNormalMap());
      texture_state->setTextureFiltering(FO_LINEAR, FO_LINEAR, FO_NONE);
      texture_state->setTextureAddressingMode(TextureUnitState::TAM_CLAMP);
    }

    return new_material;
  }
}

MaterialPtr TerrainMaterialGeneratorB::SM2Profile::generateForCompositeMap(const Terrain *terrain) {
  String material_name = "TerrainCustom";
  const int GENERATOR = 1;

  String new_name = material_name + "_" + std::to_string(GENERATOR);

  if (MaterialManager::getSingleton().resourceExists(new_name)) {
    return MaterialManager::getSingleton().getByName(new_name);
  } else {
    auto new_material = MaterialManager::getSingleton().getByName(material_name)->clone(new_name);
    return new_material;
  }
}
}  // namespace Ogre
