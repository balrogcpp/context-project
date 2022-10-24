/// created by Andrey Vasiliev

#include "pch.h"
#include "TerrainMaterialGeneratorB.h"
#include "Engine.h"
#ifdef OGRE_BUILD_COMPONENT_TERRAIN
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainMaterialGenerator.h>
#endif

using namespace std;

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

static void FixTerrainShadowCaster(const MaterialPtr &material, const Terrain *OgreTerrainPtr) {
  auto *pass = material->getTechnique(0)->getPass(0);
  string material_name = material->getName();
  auto caster_material = MaterialManager::getSingleton().getByName("PSSM/shadow_caster_terrain");
  string caster_name = "PSSM/shadow_caster_terrain/" + material_name;
  MaterialPtr new_caster = MaterialManager::getSingleton().getByName(caster_name);

  if (!new_caster) {
    new_caster = caster_material->clone(caster_name);
  }

  Matrix4 posIndexToObjectSpace = OgreTerrainPtr->getPointTransform();
  auto *new_pass = new_caster->getTechnique(0)->getPass(0);
  new_pass->setCullingMode(pass->getCullingMode());
  new_pass->setManualCullingMode(pass->getManualCullingMode());

  auto &vert_params = new_pass->getVertexProgram()->getDefaultParameters();
  vert_params->setIgnoreMissingParams(true);
  vert_params->setNamedConstant("posIndexToObjectSpace", posIndexToObjectSpace);
  Real baseUVScale = 1.0f / (OgreTerrainPtr->getSize() - 1);
  vert_params->setNamedConstant("baseUVScale", baseUVScale);

  material->getTechnique(0)->setShadowCasterMaterial(new_caster);
}

static void FixTerrainShadowCaster(const std::string &material, const Terrain *OgreTerrainPtr) {
  auto material_ptr = MaterialManager::getSingleton().getByName(material);
  FixTerrainShadowCaster(material_ptr, OgreTerrainPtr);
}

MaterialPtr TerrainMaterialGeneratorB::SM2Profile::generate(const Terrain *OgreTerrainPtr) {
  string material_name;

  if (Root::getSingleton().getSceneManager("Default")->getShadowTechnique() != SHADOWTYPE_NONE)
    material_name = "TerrainCustom";
  else
    material_name = "TerrainCustomPBRS";

  const string GENERATOR = "_0";

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

  string new_name = material_name + GENERATOR;
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

    Glue::GetComponent<Glue::SceneManager>().RegMaterial(new_material);
    return new_material;
  }
}

MaterialPtr TerrainMaterialGeneratorB::SM2Profile::generateForCompositeMap(const Terrain *terrain) {
  string material_name = "TerrainCustom";
  const int GENERATOR = 1;

  string new_name = material_name + "_" + std::to_string(GENERATOR);

  if (MaterialManager::getSingleton().resourceExists(new_name)) {
    return MaterialManager::getSingleton().getByName(new_name);
  } else {
    auto new_material = MaterialManager::getSingleton().getByName(material_name)->clone(new_name);
    return new_material;
  }
}

}  // namespace Ogre
