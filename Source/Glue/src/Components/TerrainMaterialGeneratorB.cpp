// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Components/TerrainMaterialGeneratorB.h"
#include "PBR.h"
#ifdef OGRE_BUILD_COMPONENT_TERRAIN
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainMaterialGenerator.h>
#endif

using namespace std;
using namespace Ogre;

namespace Glue {

TerrainMaterialGeneratorB::TerrainMaterialGeneratorB() {
  mProfiles.push_back(OGRE_NEW SM2Profile(this, "SM2", "Profile for rendering on Shader Model 2 capable cards"));
  setActiveProfile(mProfiles.back());
}

TerrainMaterialGeneratorB::~TerrainMaterialGeneratorB() {}

TerrainMaterialGeneratorB::SM2Profile::SM2Profile(TerrainMaterialGenerator *parent, const String &name, const String &desc)
    : Profile(parent, name, desc) {}

TerrainMaterialGeneratorB::SM2Profile::~SM2Profile() {}

bool TerrainMaterialGeneratorB::SM2Profile::isVertexCompressionSupported() const {
  return true;
}

void TerrainMaterialGeneratorB::SM2Profile::setLightmapEnabled(bool enabled) { EnableLightmap = enabled; }

void TerrainMaterialGeneratorB::SM2Profile::requestOptions(Ogre::Terrain *OgreTerrainPtr) {
  OgreTerrainPtr->_setMorphRequired(true);
  OgreTerrainPtr->_setNormalMapRequired(true);
  OgreTerrainPtr->_setLightMapRequired(EnableLightmap, false);
  OgreTerrainPtr->_setCompositeMapRequired(false);
}

static void FixTerrainShadowCaster(const Ogre::MaterialPtr &material, const Ogre::Terrain *OgreTerrainPtr) {
  auto *pass = material->getTechnique(0)->getPass(0);
  string material_name = material->getName();
  auto caster_material = MaterialManager::getSingleton().getByName("PSSM/shadow_caster_terrain");
  string caster_name = "PSSM/shadow_caster_terrain/" + material_name;
  MaterialPtr new_caster = MaterialManager::getSingleton().getByName(caster_name);

  if (!new_caster) {
    new_caster = caster_material->clone(caster_name);
  }

  Ogre::Matrix4 posIndexToObjectSpace = OgreTerrainPtr->getPointTransform();
  auto *new_pass = new_caster->getTechnique(0)->getPass(0);
  new_pass->setCullingMode(pass->getCullingMode());
  new_pass->setManualCullingMode(pass->getManualCullingMode());

  auto &vert_params = new_pass->getVertexProgram()->getDefaultParameters();
  vert_params->setIgnoreMissingParams(true);
  vert_params->setNamedConstant("posIndexToObjectSpace", posIndexToObjectSpace);
  Ogre::Real baseUVScale = 1.0f / (OgreTerrainPtr->getSize() - 1);
  vert_params->setNamedConstant("baseUVScale", baseUVScale);

  material->getTechnique(0)->setShadowCasterMaterial(new_caster);
}

static void FixTerrainShadowCaster(const std::string &material, const Ogre::Terrain *OgreTerrainPtr) {
  auto material_ptr = MaterialManager::getSingleton().getByName(material);
  FixTerrainShadowCaster(material_ptr, OgreTerrainPtr);
}

Ogre::MaterialPtr TerrainMaterialGeneratorB::SM2Profile::generate(const Ogre::Terrain *OgreTerrainPtr) {
  string material_name = "TerrainCustom";
  const string GENERATOR = "_0";

  if (isVertexCompressionSupported()) {
    auto material = Ogre::MaterialManager::getSingleton().getByName(material_name);
    if (material->getTechnique(0)->getPass(0)->hasVertexProgram()) {
      auto vert_params = material->getTechnique(0)->getPass(0)->getVertexProgramParameters();

      Ogre::Matrix4 posIndexToObjectSpace = OgreTerrainPtr->getPointTransform();

      vert_params->setIgnoreMissingParams(true);
      vert_params->setNamedConstant("posIndexToObjectSpace", posIndexToObjectSpace);
      Ogre::Real baseUVScale = 1.0f / (OgreTerrainPtr->getSize() - 1);
      vert_params->setNamedConstant("baseUVScale", baseUVScale);
    }
  }

  auto normalmap = OgreTerrainPtr->getTerrainNormalMap();
  string new_name = material_name + GENERATOR;

  if (Ogre::MaterialManager::getSingleton().resourceExists(new_name)) {
    // if(isVertexCompressionSupported()) FixTerrainShadowCaster(new_name, OgreTerrainPtr);
    return Ogre::MaterialManager::getSingleton().getByName(new_name);
  } else {
    auto new_material = Ogre::MaterialManager::getSingleton().getByName(material_name)->clone(new_name);

    auto *texture_state = new_material->getTechnique(0)->getPass(0)->getTextureUnitState("GlobalNormal");
    if (texture_state) {
      texture_state->setTexture(normalmap);
      texture_state->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);
      texture_state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
    }

    if(isVertexCompressionSupported()) FixTerrainShadowCaster(new_material, OgreTerrainPtr);

    return new_material;
  }
}

Ogre::MaterialPtr TerrainMaterialGeneratorB::SM2Profile::generateForCompositeMap(const Ogre::Terrain *terrain) {
  string material_name = "TerrainCustom";
  const int GENERATOR = 1;

  string new_name = material_name + to_string(GENERATOR);

  if (Ogre::MaterialManager::getSingleton().resourceExists(new_name)) {
    return Ogre::MaterialManager::getSingleton().getByName(new_name);
  } else {
    auto new_material = Ogre::MaterialManager::getSingleton().getByName(material_name)->clone(new_name);
    return new_material;
  }
}

}  // namespace Glue
