//MIT License
//
//Copyright (c) 2020 Andrey Vasiliev
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
#include "DotSceneLoaderB.h"

#include "ShaderUtils.h"
using namespace xio;

namespace xio {
//---------------------------------------------------------------------
TerrainMaterialGeneratorB::TerrainMaterialGeneratorB() {
  mProfiles.push_back(OGRE_NEW SM2Profile(this, "SM2", "Profile for rendering on Shader Model 2 capable cards"));

  // TODO - check hardware capabilities & use fallbacks if required (more profiles needed)
  setActiveProfile(mProfiles.back());
}
//---------------------------------------------------------------------
TerrainMaterialGeneratorB::~TerrainMaterialGeneratorB() = default;
//---------------------------------------------------------------------
TerrainMaterialGeneratorB::SM2Profile::SM2Profile(TerrainMaterialGenerator *parent,
                                                  const Ogre::String &name,
                                                  const Ogre::String &desc)
    : Profile(parent, name, desc)
{
}
//---------------------------------------------------------------------
TerrainMaterialGeneratorB::SM2Profile::~SM2Profile() {
}
//---------------------------------------------------------------------
void TerrainMaterialGeneratorB::SM2Profile::requestOptions(Ogre::Terrain *terrain) {
  terrain->_setMorphRequired(true);
  terrain->_setNormalMapRequired(true);
  terrain->_setLightMapRequired(false, false);
  terrain->_setCompositeMapRequired(false);
}
//---------------------------------------------------------------------
Ogre::MaterialPtr TerrainMaterialGeneratorB::SM2Profile::generate(const Ogre::Terrain *terrain) {
  std::string material_name = "Terrain";

  UpdatePbrParams(material_name);
  UpdatePbrShadowReceiver(material_name);
  if (isVertexCompressionSupported()) {
    auto material = Ogre::MaterialManager::getSingleton().getByName(material_name);
    if (material->getTechnique(0)->getPass(0)->hasVertexProgram()) {
      auto vert_params = material->getTechnique(0)->getPass(0)->getVertexProgramParameters();
      auto &constants = vert_params->getConstantDefinitions();
      Ogre::Matrix4 posIndexToObjectSpace;
      terrain->getPointTransform(&posIndexToObjectSpace);

      vert_params->setIgnoreMissingParams(true);
      vert_params->setNamedConstant("posIndexToObjectSpace", posIndexToObjectSpace);

      Ogre::Real baseUVScale = 1.0f / (terrain->getSize() - 1);
      vert_params->setNamedConstant("baseUVScale", baseUVScale);
    }
  }

  static long long counter = 0;
  auto normalmap = terrain->getTerrainNormalMap();

  auto new_material = Ogre::MaterialManager::getSingleton().getByName(material_name)->clone(material_name + std::to_string(counter));
  auto *texture_state = new_material->getTechnique(0)->getPass(0)->getTextureUnitState("GlobalNormal");
  if (texture_state) {
    texture_state->setTexture(normalmap);
    texture_state->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_NONE);
    texture_state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
  }
  counter++;

  return new_material;
}
//---------------------------------------------------------------------
Ogre::MaterialPtr TerrainMaterialGeneratorB::SM2Profile::generateForCompositeMap(const Ogre::Terrain *terrain) {
  std::string material_name = "Terrain";
  UpdatePbrParams(material_name);
  UpdatePbrShadowReceiver(material_name);

  static long long counter = 0;
  counter++;
  return Ogre::MaterialManager::getSingleton().getByName(material_name)->clone(material_name + std::to_string(counter));
}
}