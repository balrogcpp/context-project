// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "TerrainMaterialGeneratorB.h"
#include "PbrShaderUtils.h"
#ifdef OGRE_BUILD_COMPONENT_TERRAIN
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainAutoUpdateLod.h>
#include <Terrain/OgreTerrainMaterialGenerator.h>
#include <Terrain/OgreTerrainQuadTreeNode.h>
#endif

using namespace std;
using namespace Ogre;

namespace glue {

//----------------------------------------------------------------------------------------------------------------------
TerrainMaterialGeneratorB::TerrainMaterialGeneratorB() {
  mProfiles.push_back(OGRE_NEW SM2Profile(this, "SM2", "Profile for rendering on Shader Model 2 capable cards"));

  // TODO - check hardware capabilities & use fallbacks if required (more
  // profiles needed)
  setActiveProfile(mProfiles.back());
}

//----------------------------------------------------------------------------------------------------------------------
TerrainMaterialGeneratorB::~TerrainMaterialGeneratorB() = default;

//----------------------------------------------------------------------------------------------------------------------
TerrainMaterialGeneratorB::SM2Profile::SM2Profile(TerrainMaterialGenerator *parent, const String &name,
                                                  const String &desc)
    : Profile(parent, name, desc) {}

//----------------------------------------------------------------------------------------------------------------------
TerrainMaterialGeneratorB::SM2Profile::~SM2Profile() {}

//----------------------------------------------------------------------------------------------------------------------
bool TerrainMaterialGeneratorB::SM2Profile::isVertexCompressionSupported() const {
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  return true;
#else
  static bool glsles =
      bool{Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL ES 2.x Rendering Subsystem"};
  return glsles;
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void TerrainMaterialGeneratorB::SM2Profile::setLightmapEnabled(bool enabled) { lightmap_ = enabled; }

//----------------------------------------------------------------------------------------------------------------------
void TerrainMaterialGeneratorB::SM2Profile::requestOptions(Ogre::Terrain *terrain) {
  terrain->_setMorphRequired(true);
  terrain->_setNormalMapRequired(true);
  terrain->_setLightMapRequired(lightmap_, false);
  terrain->_setCompositeMapRequired(false);
}

//----------------------------------------------------------------------------------------------------------------------
Ogre::MaterialPtr TerrainMaterialGeneratorB::SM2Profile::generate(const Ogre::Terrain *terrain) {
  string material_name = "TerrainCustom";
  const int GENERATOR = 0;

  if (isVertexCompressionSupported()) {
    auto material = Ogre::MaterialManager::getSingleton().getByName(material_name);
    if (material->getTechnique(0)->getPass(0)->hasVertexProgram()) {
      auto vert_params = material->getTechnique(0)->getPass(0)->getVertexProgramParameters();

      Ogre::Matrix4 posIndexToObjectSpace = terrain->getPointTransform();

      vert_params->setIgnoreMissingParams(true);
      vert_params->setNamedConstant("posIndexToObjectSpace", posIndexToObjectSpace);

      Ogre::Real baseUVScale = 1.0f / (terrain->getSize() - 1);
      vert_params->setNamedConstant("baseUVScale", baseUVScale);
    }
  }

  auto normalmap = terrain->getTerrainNormalMap();
  string new_name = material_name + to_string(GENERATOR);

  Pbr::UpdatePbrParams(material_name);
  Pbr::UpdatePbrShadowReceiver(material_name);

  if (Ogre::MaterialManager::getSingleton().resourceExists(new_name)) {
    Pbr::UpdatePbrParams(new_name);
    Pbr::UpdatePbrShadowReceiver(new_name);

    return Ogre::MaterialManager::getSingleton().getByName(new_name);
  } else {
    auto new_material = Ogre::MaterialManager::getSingleton().getByName(material_name)->clone(new_name);

    auto *texture_state = new_material->getTechnique(0)->getPass(0)->getTextureUnitState("GlobalNormal");
    if (texture_state) {
      texture_state->setTexture(normalmap);
      texture_state->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);
      texture_state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
    }

    Pbr::UpdatePbrParams(new_name);
    Pbr::UpdatePbrShadowReceiver(new_name);

    return new_material;
  }
}

//----------------------------------------------------------------------------------------------------------------------
Ogre::MaterialPtr TerrainMaterialGeneratorB::SM2Profile::generateForCompositeMap(const Ogre::Terrain *terrain) {
  string material_name = "TerrainCustom";
  const int GENERATOR = 1;

  Pbr::UpdatePbrParams(material_name);

  string new_name = material_name + to_string(GENERATOR);

  if (Ogre::MaterialManager::getSingleton().resourceExists(new_name)) {
    return Ogre::MaterialManager::getSingleton().getByName(new_name);
  } else {
    auto new_material = Ogre::MaterialManager::getSingleton().getByName(material_name)->clone(new_name);
    return new_material;
  }
}

}  // namespace glue
