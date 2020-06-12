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

/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2014 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#include "pcheader.hpp"

#include "TerrainMaterialGeneratorC.hpp"

namespace Context {
//---------------------------------------------------------------------
TerrainMaterialGeneratorC::TerrainMaterialGeneratorC() {
  // define the layers
  // We expect terrain textures to have no alpha, so we use the alpha channel
  // in the albedo texture to store specular reflection
  // similarly we double-up the normal and height (for parallax)
  mLayerDecl.samplers.push_back(Ogre::TerrainLayerSampler("albedo_specular", Ogre::PF_BYTE_RGBA));
  mLayerDecl.samplers.push_back(Ogre::TerrainLayerSampler("normal_height", Ogre::PF_BYTE_RGBA));

  mLayerDecl.elements.push_back(
      Ogre::TerrainLayerSamplerElement(0, Ogre::TLSS_ALBEDO, 0, 3));
  mLayerDecl.elements.push_back(
      Ogre::TerrainLayerSamplerElement(0, Ogre::TLSS_SPECULAR, 3, 1));
  mLayerDecl.elements.push_back(
      Ogre::TerrainLayerSamplerElement(1, Ogre::TLSS_NORMAL, 0, 3));
  mLayerDecl.elements.push_back(
      Ogre::TerrainLayerSamplerElement(1, Ogre::TLSS_HEIGHT, 3, 1));

  mProfiles.push_back(OGRE_NEW SM2Profile(this, "SM2", "Profile for rendering on Shader Model 2 capable cards"));

  // TODO - check hardware capabilities & use fallbacks if required (more profiles needed)
  setActiveProfile(mProfiles.back());
}
//---------------------------------------------------------------------
TerrainMaterialGeneratorC::~TerrainMaterialGeneratorC() = default;
//---------------------------------------------------------------------
//---------------------------------------------------------------------
TerrainMaterialGeneratorC::SM2Profile::SM2Profile(TerrainMaterialGenerator *parent,
                                                  const Ogre::String &name,
                                                  const Ogre::String &desc)
    : Profile(parent, name, desc),
      mShaderGen(nullptr),
      mLayerNormalMappingEnabled(true),
      mLayerParallaxMappingEnabled(true),
      mLayerSpecularMappingEnabled(true),
      mGlobalColourMapEnabled(true),
      mLightmapEnabled(true),
      mCompositeMapEnabled(true),
      mReceiveDynamicShadows(true),
      mPSSM(nullptr),
      mDepthShadows(false),
      mLowLodShadows(false) {
  Ogre::HighLevelGpuProgramManager &hmgr = Ogre::HighLevelGpuProgramManager::getSingleton();
}
//---------------------------------------------------------------------
TerrainMaterialGeneratorC::SM2Profile::~SM2Profile() {
  OGRE_DELETE mShaderGen;
}
//---------------------------------------------------------------------
void TerrainMaterialGeneratorC::SM2Profile::requestOptions(Ogre::Terrain *terrain) {
  terrain->_setMorphRequired(false);
  terrain->_setNormalMapRequired(true);
  terrain->_setLightMapRequired(false, false);
  terrain->_setCompositeMapRequired(false);
  terrain->_setMorphRequired(true);
  terrain->_setNormalMapRequired(true);
  terrain->_setLightMapRequired(mLightmapEnabled, false);
  terrain->_setCompositeMapRequired(false);
}
//---------------------------------------------------------------------
bool TerrainMaterialGeneratorC::SM2Profile::isVertexCompressionSupported() const {
  return false;
}
//---------------------------------------------------------------------
void TerrainMaterialGeneratorC::SM2Profile::setLightmapEnabled(bool enabled) {
}
//---------------------------------------------------------------------
Ogre::uint8 TerrainMaterialGeneratorC::SM2Profile::getMaxLayers(const Ogre::Terrain *terrain) const {
  // count the texture units free
  Ogre::uint8 freeTextureUnits = 16;
//  // lightmap
//  --freeTextureUnits;
//  // normalmap
  --freeTextureUnits;
//  // colourmap
//  if (terrain->getGlobalColourMapEnabled())
//    --freeTextureUnits;
//  if (isShadowingEnabled(HIGH_LOD, terrain)) {
//    Ogre::uint8 numShadowTextures = 1;
//    if (getReceiveDynamicShadowsPSSM()) {
//      numShadowTextures = (Ogre::uint8) getReceiveDynamicShadowsPSSM()->getSplitCount();
//    }
//    freeTextureUnits -= numShadowTextures;
//  }

  freeTextureUnits -= 3;

  // each layer needs 2.25 units (1xdiffusespec, 1xnormalheight, 0.25xblend)
  return static_cast<Ogre::uint8>(freeTextureUnits / 2.25f);

}
//---------------------------------------------------------------------
Ogre::MaterialPtr TerrainMaterialGeneratorC::SM2Profile::generate(const Ogre::Terrain *terrain) {

    return Ogre::MaterialManager::getSingleton().getByName("Plane");
}
//---------------------------------------------------------------------
Ogre::MaterialPtr TerrainMaterialGeneratorC::SM2Profile::generateForCompositeMap(const Ogre::Terrain *terrain) {
  return Ogre::MaterialManager::getSingleton().getByName("Plane");
}
//---------------------------------------------------------------------
void TerrainMaterialGeneratorC::SM2Profile::updateParams(const Ogre::MaterialPtr &mat, const Ogre::Terrain *terrain) {
}
//---------------------------------------------------------------------
void TerrainMaterialGeneratorC::SM2Profile::updateParamsForCompositeMap(const Ogre::MaterialPtr &mat,
                                                                        const Ogre::Terrain *terrain) {
}

} //namespace Context