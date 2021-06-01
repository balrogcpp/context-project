//MIT License
//
//Copyright (c) 2021 Andrew Vasiliev
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//    of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//    copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//    copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.
//

#include "pcheader.h"
#include "TerrainMaterialGeneratorB.h"
#include "PbrShaderUtils.h"

#ifdef OGRE_BUILD_COMPONENT_TERRAIN
#include <Terrain/OgreTerrainQuadTreeNode.h>
#include <Terrain/OgreTerrainMaterialGenerator.h>
#include <Terrain/OgreTerrainAutoUpdateLod.h>
#include <Terrain/OgreTerrain.h>
#endif

using namespace std;
using namespace Ogre;

namespace xio {

//----------------------------------------------------------------------------------------------------------------------
TerrainMaterialGeneratorB::TerrainMaterialGeneratorB() {
  mProfiles.push_back(OGRE_NEW SM2Profile(this, "SM2", "Profile for rendering on Shader Model 2 capable cards"));

  // TODO - check hardware capabilities & use fallbacks if required (more profiles needed)
  setActiveProfile(mProfiles.back());
}

//----------------------------------------------------------------------------------------------------------------------
TerrainMaterialGeneratorB::~TerrainMaterialGeneratorB() = default;

//----------------------------------------------------------------------------------------------------------------------
TerrainMaterialGeneratorB::SM2Profile::SM2Profile(TerrainMaterialGenerator *parent, const String &name, const String &desc)
	: Profile(parent, name, desc) {

}

//----------------------------------------------------------------------------------------------------------------------
TerrainMaterialGeneratorB::SM2Profile::~SM2Profile() {}

//----------------------------------------------------------------------------------------------------------------------
bool TerrainMaterialGeneratorB::SM2Profile::isVertexCompressionSupported() const {
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  return true;
#else
  static bool glsles = bool{Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL ES 2.x Rendering Subsystem"};
  return glsles;
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void TerrainMaterialGeneratorB::SM2Profile::setLightmapEnabled(bool enabled) {
  lightmap_ = enabled;
}

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
	  auto &constants = vert_params->getConstantDefinitions();
	  Ogre::Matrix4 posIndexToObjectSpace;
	  terrain->getPointTransform(&posIndexToObjectSpace);

	  vert_params->setIgnoreMissingParams(true);
	  vert_params->setNamedConstant("posIndexToObjectSpace", posIndexToObjectSpace);

	  Ogre::Real baseUVScale = 1.0f/(terrain->getSize() - 1);
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

} //namespace
