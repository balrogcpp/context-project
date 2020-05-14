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

#include "TerrainMaterialGeneratorB.hpp"

namespace Context {
//---------------------------------------------------------------------
TerrainMaterialGeneratorB::TerrainMaterialGeneratorB() {
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
TerrainMaterialGeneratorB::~TerrainMaterialGeneratorB() = default;
//---------------------------------------------------------------------
//---------------------------------------------------------------------
TerrainMaterialGeneratorB::SM2Profile::SM2Profile(TerrainMaterialGenerator *parent,
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

  if (hmgr.isLanguageSupported("glsl") || hmgr.isLanguageSupported("glsles")) {
    mShaderGen = OGRE_NEW ShaderHelperGLSL();
    mShaderGen->SetTerrainFogPerpixel(this->terrain_fog_perpixel_);
  } else if (hmgr.isLanguageSupported("cg") || hmgr.isLanguageSupported("hlsl")) {
    mShaderGen = OGRE_NEW ShaderHelperCg();
    mShaderGen->SetTerrainFogPerpixel(this->terrain_fog_perpixel_);
  }
}
//---------------------------------------------------------------------
TerrainMaterialGeneratorB::SM2Profile::~SM2Profile() {
  OGRE_DELETE mShaderGen;
}
//---------------------------------------------------------------------
void TerrainMaterialGeneratorB::SM2Profile::requestOptions(Ogre::Terrain *terrain) {
  terrain->_setMorphRequired(true);
  terrain->_setNormalMapRequired(true);
  terrain->_setLightMapRequired(mLightmapEnabled, true);
  terrain->_setCompositeMapRequired(mCompositeMapEnabled);
}
//---------------------------------------------------------------------
bool TerrainMaterialGeneratorB::SM2Profile::isVertexCompressionSupported() const {
  return mShaderGen && mShaderGen->isVertexCompressionSupported();
}
//---------------------------------------------------------------------
void TerrainMaterialGeneratorB::SM2Profile::setLayerNormalMappingEnabled(bool enabled) {
  if (enabled != mLayerNormalMappingEnabled) {
    mLayerNormalMappingEnabled = enabled;
    mParent->_markChanged();
  }
}
//---------------------------------------------------------------------
void TerrainMaterialGeneratorB::SM2Profile::setLayerParallaxMappingEnabled(bool enabled) {
  if (enabled != mLayerParallaxMappingEnabled) {
    mLayerParallaxMappingEnabled = enabled;
    mParent->_markChanged();
  }
}
//---------------------------------------------------------------------
void TerrainMaterialGeneratorB::SM2Profile::setLayerSpecularMappingEnabled(bool enabled) {
  if (enabled != mLayerSpecularMappingEnabled) {
    mLayerSpecularMappingEnabled = enabled;
    mParent->_markChanged();
  }
}
//---------------------------------------------------------------------
void TerrainMaterialGeneratorB::SM2Profile::setGlobalColourMapEnabled(bool enabled) {
  if (enabled != mGlobalColourMapEnabled) {
    mGlobalColourMapEnabled = enabled;
    mParent->_markChanged();
  }
}
//---------------------------------------------------------------------
void TerrainMaterialGeneratorB::SM2Profile::setLightmapEnabled(bool enabled) {
  if (enabled != mLightmapEnabled) {
    mLightmapEnabled = enabled;
    mParent->_markChanged();
  }
}
//---------------------------------------------------------------------
void TerrainMaterialGeneratorB::SM2Profile::setCompositeMapEnabled(bool enabled) {
  if (enabled != mCompositeMapEnabled) {
    mCompositeMapEnabled = enabled;
    mParent->_markChanged();
  }
}
//---------------------------------------------------------------------
void TerrainMaterialGeneratorB::SM2Profile::setReceiveDynamicShadowsEnabled(bool enabled) {
  if (enabled != mReceiveDynamicShadows) {
    mReceiveDynamicShadows = enabled;
    mParent->_markChanged();
  }
}
//---------------------------------------------------------------------
void TerrainMaterialGeneratorB::SM2Profile::setReceiveDynamicShadowsPSSM(Ogre::PSSMShadowCameraSetup *pssmSettings) {
  if (pssmSettings != mPSSM) {
    mPSSM = pssmSettings;
    mParent->_markChanged();
  }
}
//---------------------------------------------------------------------
void TerrainMaterialGeneratorB::SM2Profile::setReceiveDynamicShadowsDepth(bool enabled) {
  if (enabled != mDepthShadows) {
    mDepthShadows = enabled;
    mParent->_markChanged();
  }

}
//---------------------------------------------------------------------
void TerrainMaterialGeneratorB::SM2Profile::setReceiveDynamicShadowsLowLod(bool enabled) {
  if (enabled != mLowLodShadows) {
    mLowLodShadows = enabled;
    mParent->_markChanged();
  }
}
//---------------------------------------------------------------------
Ogre::uint8 TerrainMaterialGeneratorB::SM2Profile::getMaxLayers(const Ogre::Terrain *terrain) const {
  // count the texture units free
  Ogre::uint8 freeTextureUnits = 16;
  // lightmap
  --freeTextureUnits;
  // normalmap
  --freeTextureUnits;
  // colourmap
  if (terrain->getGlobalColourMapEnabled())
    --freeTextureUnits;
  if (isShadowingEnabled(HIGH_LOD, terrain)) {
    Ogre::uint8 numShadowTextures = 1;
    if (getReceiveDynamicShadowsPSSM()) {
      numShadowTextures = (Ogre::uint8) getReceiveDynamicShadowsPSSM()->getSplitCount();
    }
    freeTextureUnits -= numShadowTextures;
  }

  // each layer needs 2.25 units (1xdiffusespec, 1xnormalheight, 0.25xblend)
  return static_cast<Ogre::uint8>(freeTextureUnits / 2.25f);

}
//---------------------------------------------------------------------
Ogre::MaterialPtr TerrainMaterialGeneratorB::SM2Profile::generate(const Ogre::Terrain *terrain) {
  // re-use old material if exists
  Ogre::MaterialPtr mat = terrain->_getMaterial();
  if (!mat) {
    Ogre::MaterialManager &matMgr = Ogre::MaterialManager::getSingleton();
    const Ogre::String &matName = terrain->getMaterialName();
    mat = matMgr.getByName(matName);
    if (!mat) {
      mat = matMgr.create(matName, terrain->_getDerivedResourceGroup());
    }
  }
  // clear everything
  mat->removeAllTechniques();

  // Automatically disable normal & parallax mapping if card cannot handle it
  // We do this rather than having a specific technique for it since it's simpler
  Ogre::GpuProgramManager &gmgr = Ogre::GpuProgramManager::getSingleton();
  if (!gmgr.isSyntaxSupported("ps_4_0") && !gmgr.isSyntaxSupported("ps_3_0") && !gmgr.isSyntaxSupported("ps_2_x")
      && !gmgr.isSyntaxSupported("fp40") && !gmgr.isSyntaxSupported("arbfp1") && !gmgr.isSyntaxSupported("glsl")
      && !gmgr.isSyntaxSupported("glsles")) {
    setLayerNormalMappingEnabled(false);
    setLayerParallaxMappingEnabled(false);
  }

  addTechnique(mat, terrain, HIGH_LOD);

  // LOD
  if (mCompositeMapEnabled) {
    addTechnique(mat, terrain, LOW_LOD);
    Ogre::Material::LodValueList lodValues;
    lodValues.push_back(Ogre::TerrainGlobalOptions::getSingleton().getCompositeMapDistance());
    mat->setLodLevels(lodValues);
    Ogre::Technique *lowLodTechnique = mat->getTechnique(1);
    lowLodTechnique->setLodIndex(1);
  }

  updateParams(mat, terrain);

  return mat;

}
//---------------------------------------------------------------------
Ogre::MaterialPtr TerrainMaterialGeneratorB::SM2Profile::generateForCompositeMap(const Ogre::Terrain *terrain) {
  // re-use old material if exists
  Ogre::MaterialPtr mat = terrain->_getCompositeMapMaterial();
  if (!mat) {
    Ogre::MaterialManager &matMgr = Ogre::MaterialManager::getSingleton();

    // it's important that the names are deterministic for a given terrain, so
    // use the terrain pointer as an ID
    const Ogre::String &matName = terrain->getMaterialName() + "/comp";
    mat = matMgr.getByName(matName);
    if (!mat) {
      mat = matMgr.create(matName, terrain->_getDerivedResourceGroup());
    }
  }
  // clear everything
  mat->removeAllTechniques();

  addTechnique(mat, terrain, RENDER_COMPOSITE_MAP);

  updateParamsForCompositeMap(mat, terrain);

  return mat;

}
//---------------------------------------------------------------------
void TerrainMaterialGeneratorB::SM2Profile::addTechnique(
    const Ogre::MaterialPtr &mat, const Ogre::Terrain *terrain, TechniqueType tt) {
  Ogre::Technique *tech = mat->createTechnique();

  // Only supporting one pass
  Ogre::Pass *pass = tech->createPass();

  Ogre::HighLevelGpuProgramPtr vprog = mShaderGen->generateVertexProgram(this, terrain, tt);
  Ogre::HighLevelGpuProgramPtr fprog = mShaderGen->generateFragmentProgram(this, terrain, tt);

  pass->setVertexProgram(vprog->getName());
  pass->setFragmentProgram(fprog->getName());

  if (tt == HIGH_LOD || tt == RENDER_COMPOSITE_MAP) {
    // global normal map
    Ogre::TextureUnitState *tu = pass->createTextureUnitState();
    tu->setTextureName(terrain->getTerrainNormalMap()->getName());
    tu->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

    // global colour map
    if (terrain->getGlobalColourMapEnabled() && isGlobalColourMapEnabled()) {
      tu = pass->createTextureUnitState(terrain->getGlobalColourMap()->getName());
      tu->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
    }

    // light map
    if (isLightmapEnabled()) {
      tu = pass->createTextureUnitState(terrain->getLightmap()->getName());
      tu->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
    }

    // blend maps
    Ogre::uint maxLayers = getMaxLayers(terrain);
    Ogre::uint numBlendTextures = std::min(terrain->getBlendTextureCount(maxLayers), terrain->getBlendTextureCount());
    Ogre::uint numLayers = std::min(maxLayers, static_cast<Ogre::uint>(terrain->getLayerCount()));
    for (Ogre::uint i = 0; i < numBlendTextures; ++i) {
      tu = pass->createTextureUnitState(terrain->getBlendTextureName(i));
      tu->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
    }

    // layer textures
    for (Ogre::uint i = 0; i < numLayers; ++i) {
      // diffuse / specular
      pass->createTextureUnitState(terrain->getLayerTextureName(i, 0));
      // normal / height
      if (mLayerNormalMappingEnabled)
        pass->createTextureUnitState(terrain->getLayerTextureName(i, 1));
    }

  } else {
    // LOW_LOD textures
    // composite map
    Ogre::TextureUnitState *tu = pass->createTextureUnitState();
    tu->setTextureName(terrain->getCompositeMap()->getName());
    tu->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

    // That's it!

  }

  // Add shadow textures (always at the end)
  if (isShadowingEnabled(tt, terrain)) {
    Ogre::uint numTextures = 1;
    if (getReceiveDynamicShadowsPSSM()) {
      numTextures = (Ogre::uint) getReceiveDynamicShadowsPSSM()->getSplitCount();
    }
    for (Ogre::uint i = 0; i < numTextures; ++i) {
      Ogre::TextureUnitState *tu = pass->createTextureUnitState();
      tu->setContentType(Ogre::TextureUnitState::CONTENT_SHADOW);
      tu->setTextureAddressingMode(Ogre::TextureUnitState::TAM_BORDER);
      tu->setTextureBorderColour(Ogre::ColourValue::White);
    }
  }

}
//---------------------------------------------------------------------
bool TerrainMaterialGeneratorB::SM2Profile::isShadowingEnabled(TechniqueType tt,
                                                               const Ogre::Terrain *terrain) const {
  return getReceiveDynamicShadowsEnabled() && tt != RENDER_COMPOSITE_MAP &&
      (tt != LOW_LOD || mLowLodShadows) &&
      terrain->getSceneManager()->isShadowTechniqueTextureBased();

}
//---------------------------------------------------------------------
void TerrainMaterialGeneratorB::SM2Profile::updateParams(const Ogre::MaterialPtr &mat, const Ogre::Terrain *terrain) {
  mShaderGen->updateParams(this, mat, terrain, false);

}
//---------------------------------------------------------------------
void TerrainMaterialGeneratorB::SM2Profile::updateParamsForCompositeMap(const Ogre::MaterialPtr &mat,
                                                                        const Ogre::Terrain *terrain) {
  mShaderGen->updateParams(this, mat, terrain, true);
}
//---------------------------------------------------------------------
bool TerrainMaterialGeneratorB::SM2Profile::IsTerrainFogPerpixel() const {
  return terrain_fog_perpixel_;
}
//---------------------------------------------------------------------
void TerrainMaterialGeneratorB::SM2Profile::SetTerrainFogPerpixel(bool terrain_fog_perpixel) {
  SM2Profile::terrain_fog_perpixel_ = terrain_fog_perpixel;
}
//---------------------------------------------------------------------
Ogre::HighLevelGpuProgramPtr
ShaderHelper::generateVertexProgram(
    const SM2Profile *prof, const Ogre::Terrain *terrain, TechniqueType tt) {
  Ogre::HighLevelGpuProgramPtr ret = createVertexProgram(prof, terrain, tt);

  Ogre::StringStream sourceStr;
  generateVertexProgramSource(prof, terrain, tt, sourceStr);

  ret->setSource(sourceStr.str());
  ret->load();
  defaultVpParams(prof, terrain, tt, ret);

#if OGRE_DEBUG_MODE
  LogManager::getSingleton().stream(LML_TRIVIAL) << "*** Ogre::Terrain Vertex Program: "
            << ret->getName() << " ***\n" << ret->getSource() << "\n***   ***";
#endif
  return ret;

}
//---------------------------------------------------------------------
Ogre::HighLevelGpuProgramPtr
ShaderHelper::generateFragmentProgram(
    const SM2Profile *prof, const Ogre::Terrain *terrain, TechniqueType tt) {
  Ogre::HighLevelGpuProgramPtr ret = createFragmentProgram(prof, terrain, tt);

  Ogre::StringStream sourceStr;
  generateFragmentProgramSource(prof, terrain, tt, sourceStr);
  ret->setSource(sourceStr.str());
  ret->load();
  defaultFpParams(prof, terrain, tt, ret);

#if OGRE_DEBUG_MODE
  LogManager::getSingleton().stream(LML_TRIVIAL) << "*** Ogre::Terrain Fragment Program: "
            << ret->getName() << " ***\n" << ret->getSource() << "\n*** ***";
#endif
  return ret;
}
//---------------------------------------------------------------------
void ShaderHelper::generateVertexProgramSource(
    const SM2Profile *prof, const Ogre::Terrain *terrain, TechniqueType tt, Ogre::StringStream &outStream) {
  generateVpHeader(prof, terrain, tt, outStream);

  if (tt != LOW_LOD) {
    Ogre::uint maxLayers = prof->getMaxLayers(terrain);
    Ogre::uint numLayers = std::min(maxLayers, static_cast<Ogre::uint>(terrain->getLayerCount()));

    for (Ogre::uint i = 0; i < numLayers; ++i)
      generateVpLayer(prof, terrain, tt, i, outStream);
  }

  generateVpFooter(prof, terrain, tt, outStream);

}
//---------------------------------------------------------------------
void ShaderHelper::generateFragmentProgramSource(
    const SM2Profile *prof, const Ogre::Terrain *terrain, TechniqueType tt, Ogre::StringStream &outStream) {
  generateFpHeader(prof, terrain, tt, outStream);

  if (tt != LOW_LOD) {
    Ogre::uint maxLayers = prof->getMaxLayers(terrain);
    Ogre::uint numLayers = std::min(maxLayers, static_cast<Ogre::uint>(terrain->getLayerCount()));

    for (Ogre::uint i = 0; i < numLayers; ++i)
      generateFpLayer(prof, terrain, tt, i, outStream);
  }

  generateFpFooter(prof, terrain, tt, outStream);
}
//---------------------------------------------------------------------
void ShaderHelper::defaultVpParams(
    const SM2Profile *prof,
    const Ogre::Terrain *terrain,
    TechniqueType tt,
    const Ogre::HighLevelGpuProgramPtr &prog) {
  Ogre::GpuProgramParametersSharedPtr params = prog->getDefaultParameters();
  params->setIgnoreMissingParams(true);
  params->setNamedAutoConstant("worldMatrix", Ogre::GpuProgramParameters::ACT_WORLD_MATRIX);
  params->setNamedAutoConstant("viewProjMatrix", Ogre::GpuProgramParameters::ACT_VIEWPROJ_MATRIX);
  params->setNamedAutoConstant("lodMorph", Ogre::GpuProgramParameters::ACT_CUSTOM,
                               Ogre::Terrain::LOD_MORPH_CUSTOM_PARAM);
  params->setNamedAutoConstant("fogParams", Ogre::GpuProgramParameters::ACT_FOG_PARAMS);

  if (prof->isShadowingEnabled(tt, terrain)) {
    Ogre::uint numTextures = 1;
    if (prof->getReceiveDynamicShadowsPSSM()) {
      numTextures = (Ogre::uint) prof->getReceiveDynamicShadowsPSSM()->getSplitCount();
    }
    for (Ogre::uint i = 0; i < numTextures; ++i) {
      params->setNamedAutoConstant("texViewProjMatrix" + Ogre::StringConverter::toString(i),
                                   Ogre::GpuProgramParameters::ACT_TEXTURE_VIEWPROJ_MATRIX, i);
    }
  }

  if (terrain->_getUseVertexCompression() && tt != RENDER_COMPOSITE_MAP) {
    Ogre::Matrix4 posIndexToObjectSpace;
    terrain->getPointTransform(&posIndexToObjectSpace);
    params->setNamedConstant("posIndexToObjectSpace", posIndexToObjectSpace);
  }

}
//---------------------------------------------------------------------
void ShaderHelper::defaultFpParams(
    const SM2Profile *prof,
    const Ogre::Terrain *terrain,
    TechniqueType tt,
    const Ogre::HighLevelGpuProgramPtr &prog) {
  Ogre::GpuProgramParametersSharedPtr params = prog->getDefaultParameters();
  params->setIgnoreMissingParams(true);

  params->setNamedAutoConstant("ambient", Ogre::GpuProgramParameters::ACT_AMBIENT_LIGHT_COLOUR);
  params->setNamedAutoConstant("lightPosObjSpace", Ogre::GpuProgramParameters::ACT_LIGHT_POSITION_OBJECT_SPACE, 0);
  params->setNamedAutoConstant("lightDiffuseColour", Ogre::GpuProgramParameters::ACT_LIGHT_DIFFUSE_COLOUR, 0);
  params->setNamedAutoConstant("lightSpecularColour", Ogre::GpuProgramParameters::ACT_LIGHT_SPECULAR_COLOUR, 0);
  params->setNamedAutoConstant("eyePosObjSpace", Ogre::GpuProgramParameters::ACT_CAMERA_POSITION_OBJECT_SPACE);
  params->setNamedAutoConstant("fogColour", Ogre::GpuProgramParameters::ACT_FOG_COLOUR);
  params->setNamedAutoConstant("fogParams", Ogre::GpuProgramParameters::ACT_FOG_PARAMS);

  if (prof->isShadowingEnabled(tt, terrain)) {
    Ogre::uint numTextures = 1;
    if (prof->getReceiveDynamicShadowsPSSM()) {
      Ogre::PSSMShadowCameraSetup *pssm = prof->getReceiveDynamicShadowsPSSM();
      numTextures = (Ogre::uint) pssm->getSplitCount();
      Ogre::Vector4 splitPoints;
      const Ogre::PSSMShadowCameraSetup::SplitPointList &splitPointList = pssm->getSplitPoints();
      // Populate from split point 1, not 0, since split 0 isn't useful (usually 0)
      for (Ogre::uint i = 1; i < numTextures; ++i) {
        splitPoints[i - 1] = splitPointList[i];
      }
      params->setNamedConstant("pssmSplitPoints", splitPoints);
    }

    if (prof->getReceiveDynamicShadowsDepth()) {
      size_t samplerOffset = (tt == HIGH_LOD) ? mShadowSamplerStartHi : mShadowSamplerStartLo;
      for (Ogre::uint i = 0; i < numTextures; ++i) {
        params->setNamedAutoConstant("inverseShadowmapSize" + Ogre::StringConverter::toString(i),
                                     Ogre::GpuProgramParameters::ACT_INVERSE_TEXTURE_SIZE, i + samplerOffset);
      }
    }
  }

  // Explicitly bind samplers for GLSL
  if (mIsGLSL) {
    int numSamplers = 0;
    if (tt == LOW_LOD) {
      params->setNamedConstant("compositeMap", (int) numSamplers++);
    } else {
      params->setNamedConstant("globalNormal", (int) numSamplers++);

      if (terrain->getGlobalColourMapEnabled() && prof->isGlobalColourMapEnabled()) {
        params->setNamedConstant("globalColourMap", (int) numSamplers++);
      }
      if (prof->isLightmapEnabled()) {
        params->setNamedConstant("lightMap", (int) numSamplers++);
      }

      Ogre::uint maxLayers = prof->getMaxLayers(terrain);
      Ogre::uint numBlendTextures = std::min(terrain->getBlendTextureCount(maxLayers), terrain->getBlendTextureCount());
      Ogre::uint numLayers = std::min(maxLayers, static_cast<Ogre::uint>(terrain->getLayerCount()));
      // Blend textures - sampler definitions
      for (Ogre::uint i = 0; i < numBlendTextures; ++i) {
        params->setNamedConstant("blendTex" + Ogre::StringConverter::toString(i), (int) numSamplers++);
      }

      // Layer textures - sampler definitions & UV multipliers
      for (Ogre::uint i = 0; i < numLayers; ++i) {
        params->setNamedConstant("difftex" + Ogre::StringConverter::toString(i), (int) numSamplers++);
        if (prof->isLayerNormalMappingEnabled())
          params->setNamedConstant("normtex" + Ogre::StringConverter::toString(i), (int) numSamplers++);
      }

      Ogre::uint numShadowTextures = 1;
      if (prof->getReceiveDynamicShadowsPSSM())
        numShadowTextures = (Ogre::uint) prof->getReceiveDynamicShadowsPSSM()->getSplitCount();

      for (Ogre::uint i = 0; i < numShadowTextures; ++i) {
        if (prof->isShadowingEnabled(tt, terrain))
          params->setNamedConstant("shadowMap" + Ogre::StringConverter::toString(i), (int) numSamplers++);
      }
    }
  }
}
//---------------------------------------------------------------------
void ShaderHelper::updateParams(
    const SM2Profile *prof, const Ogre::MaterialPtr &mat, const Ogre::Terrain *terrain, bool compositeMap) {
  Ogre::Pass *p = mat->getTechnique(0)->getPass(0);
  if (compositeMap) {
    updateVpParams(prof, terrain, RENDER_COMPOSITE_MAP, p->getVertexProgramParameters());
    updateFpParams(prof, terrain, RENDER_COMPOSITE_MAP, p->getFragmentProgramParameters());
  } else {
    // high lod
    updateVpParams(prof, terrain, HIGH_LOD, p->getVertexProgramParameters());
    updateFpParams(prof, terrain, HIGH_LOD, p->getFragmentProgramParameters());

    if (prof->isCompositeMapEnabled()) {
      // low lod
      p = mat->getTechnique(1)->getPass(0);
      updateVpParams(prof, terrain, LOW_LOD, p->getVertexProgramParameters());
      updateFpParams(prof, terrain, LOW_LOD, p->getFragmentProgramParameters());
    }
  }
}
//---------------------------------------------------------------------
void ShaderHelper::updateVpParams(
    const SM2Profile *prof,
    const Ogre::Terrain *terrain,
    TechniqueType tt,
    const Ogre::GpuProgramParametersSharedPtr &params) {
  params->setIgnoreMissingParams(true);
  Ogre::uint maxLayers = prof->getMaxLayers(terrain);
  Ogre::uint numLayers = std::min(maxLayers, static_cast<Ogre::uint>(terrain->getLayerCount()));
  Ogre::uint numUVMul = numLayers / 4;
  if (numLayers % 4)
    ++numUVMul;
  for (Ogre::uint i = 0; i < numUVMul; ++i) {
    Ogre::Vector4 uvMul(
        terrain->getLayerUVMultiplier(i * 4),
        terrain->getLayerUVMultiplier(i * 4 + 1),
        terrain->getLayerUVMultiplier(i * 4 + 2),
        terrain->getLayerUVMultiplier(i * 4 + 3)
    );
    params->setNamedConstant("uvMul_" + Ogre::StringConverter::toString(i), uvMul);
  }

  if (terrain->_getUseVertexCompression() && tt != RENDER_COMPOSITE_MAP) {
    Ogre::Real baseUVScale = 1.0f / (terrain->getSize() - 1);
    params->setNamedConstant("baseUVScale", baseUVScale);
  }

}
//---------------------------------------------------------------------
void ShaderHelper::updateFpParams(
    const SM2Profile *prof,
    const Ogre::Terrain *terrain,
    TechniqueType tt,
    const Ogre::GpuProgramParametersSharedPtr &params) {
  params->setIgnoreMissingParams(true);
  // TODO - parameterise this?
  Ogre::Vector4 scaleBiasSpecular(0.03, -0.04, 32, 1);
  params->setNamedConstant("scaleBiasSpecular", scaleBiasSpecular);

}
//---------------------------------------------------------------------
Ogre::String ShaderHelper::getChannel(Ogre::uint idx) {
  Ogre::uint rem = idx % 4;
  switch (rem) {
    case 0:
    default:return "r";
    case 1:return "g";
    case 2:return "b";
    case 3:return "a";
  };
}
//---------------------------------------------------------------------
Ogre::String ShaderHelper::getVertexProgramName(
    const SM2Profile *prof, const Ogre::Terrain *terrain, TechniqueType tt) {
  Ogre::String progName = terrain->getMaterialName() + "/sm2/vp";

  switch (tt) {
    case HIGH_LOD:progName += "/hlod";
      break;
    case LOW_LOD:progName += "/llod";
      break;
    case RENDER_COMPOSITE_MAP:progName += "/comp";
      break;
  }

  return progName;

}
//---------------------------------------------------------------------
Ogre::String ShaderHelper::getFragmentProgramName(
    const SM2Profile *prof, const Ogre::Terrain *terrain, TechniqueType tt) {

  Ogre::String progName = terrain->getMaterialName() + "/sm2/fp";

  switch (tt) {
    case HIGH_LOD:progName += "/hlod";
      break;
    case LOW_LOD:progName += "/llod";
      break;
    case RENDER_COMPOSITE_MAP:progName += "/comp";
      break;
  }

  return progName;
}
//---------------------------------------------------------------------
bool ShaderHelper::IsTerrainFogPerpixel() const {
  return terrain_fog_perpixel_;
}
//---------------------------------------------------------------------
void ShaderHelper::SetTerrainFogPerpixel(bool terrain_fog_perpixel) {
  ShaderHelper::terrain_fog_perpixel_ = terrain_fog_perpixel;
}
//---------------------------------------------------------------------
ShaderHelperGLSL::ShaderHelperGLSL() : ShaderHelper(true) {
  mIsGLES = Ogre::HighLevelGpuProgramManager::getSingleton().isLanguageSupported("glsles");
}
//---------------------------------------------------------------------
Ogre::HighLevelGpuProgramPtr
ShaderHelperGLSL::createVertexProgram(
    const SM2Profile *prof, const Ogre::Terrain *terrain, TechniqueType tt) {
  Ogre::HighLevelGpuProgramManager &mgr = Ogre::HighLevelGpuProgramManager::getSingleton();
  Ogre::String progName = getVertexProgramName(prof, terrain, tt);

  Ogre::String lang = mgr.isLanguageSupported("glsles") ? "glsles" : "glsl";

  Ogre::HighLevelGpuProgramPtr ret = mgr.getByName(progName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  if (!ret) {
    ret = mgr.createProgram(progName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                            lang, Ogre::GPT_VERTEX_PROGRAM);
  } else {
    ret->unload();
  }

  return ret;
}
//---------------------------------------------------------------------
Ogre::HighLevelGpuProgramPtr
ShaderHelperGLSL::createFragmentProgram(
    const SM2Profile *prof, const Ogre::Terrain *terrain, TechniqueType tt) {
  Ogre::HighLevelGpuProgramManager &mgr = Ogre::HighLevelGpuProgramManager::getSingleton();
  Ogre::String progName = getFragmentProgramName(prof, terrain, tt);

  Ogre::String lang = mgr.isLanguageSupported("glsles") ? "glsles" : "glsl";

  Ogre::HighLevelGpuProgramPtr ret = mgr.getByName(progName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  if (!ret) {
    ret = mgr.createProgram(progName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                            lang, Ogre::GPT_FRAGMENT_PROGRAM);
  } else {
    ret->unload();
  }

  return ret;
}

//---------------------------------------------------------------------
void ShaderHelperGLSL::generateVpHeader(const SM2Profile *prof, const Ogre::Terrain *terrain,
                                        TechniqueType tt, Ogre::StringStream &outStream) {
  int version = mIsGLES ? 100 : 120;

  outStream << "#version " << version << "\n";

  if (mIsGLES) {
    outStream << "precision highp int;\n";
    outStream << "precision highp float;\n";
  }

  bool compression = terrain->_getUseVertexCompression() && tt != RENDER_COMPOSITE_MAP;
  if (compression) {
    outStream <<
              "attribute vec2 posIndex;\n"
              "attribute float height;\n";
  } else {
    outStream <<
              "attribute vec4 position;\n"
              "attribute vec2 uv0;\n";
  }
  if (tt != RENDER_COMPOSITE_MAP)
    outStream << "attribute vec2 delta;\n"; // lodDelta, lodThreshold

  outStream <<
            "uniform mat4 worldMatrix;\n"
            "uniform mat4 viewProjMatrix;\n"
            "uniform vec2 lodMorph;\n"; // morph amount, morph LOD target

  if (compression) {
    outStream <<
              "uniform mat4 posIndexToObjectSpace;\n"
              "uniform float baseUVScale;\n";
  }
  // uv multipliers
  Ogre::uint maxLayers = prof->getMaxLayers(terrain);
  Ogre::uint numLayers = std::min(maxLayers, static_cast<Ogre::uint>(terrain->getLayerCount()));
  Ogre::uint numUVMultipliers = (numLayers / 4);
  if (numLayers % 4)
    ++numUVMultipliers;
  for (Ogre::uint i = 0; i < numUVMultipliers; ++i)
    outStream << "uniform vec4 uvMul_" << i << ";\n";

  outStream <<
            "varying vec4 oPosObj;\n";

  Ogre::uint texCoordSet = 1;
  outStream <<
            "varying vec4 oUVMisc; // xy = uv, z = camDepth\n";

  // layer UV's premultiplied, packed as xy/zw
  Ogre::uint numUVSets = numLayers / 2;
  if (numLayers % 2)
    ++numUVSets;
  if (tt != LOW_LOD) {
    for (Ogre::uint i = 0; i < numUVSets; ++i) {
      outStream <<
                "varying vec4 layerUV" << i << ";\n";
    }
  }

  if (prof->getParent()->getDebugLevel() && tt != RENDER_COMPOSITE_MAP) {
    outStream << "varying vec2 lodInfo;\n";
  }

  bool fog = terrain->getSceneManager()->getFogMode() != Ogre::FOG_NONE && tt != RENDER_COMPOSITE_MAP;
  if (fog) {
    outStream <<
              "uniform vec4 fogParams;\n"
              "varying float fogVal;\n";
  }

  if (prof->isShadowingEnabled(tt, terrain)) {
    texCoordSet = generateVpDynamicShadowsParams(texCoordSet, prof, terrain, tt, outStream);
  }

  // check we haven't exceeded texture coordinates
  if (texCoordSet > 8) {
    OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS,
                "Requested options require too many texture coordinate sets! Try reducing the number of layers.");
  }

  outStream << "void main(void) {\n";
  if (compression) {
    outStream <<
              "    vec4 position = posIndexToObjectSpace * vec4(posIndex, height, 1);\n"
              "    vec2 uv0 = vec2(posIndex.x * baseUVScale, 1.0 - (posIndex.y * baseUVScale));\n";
  }
  outStream <<
            "    vec4 worldPos = worldMatrix * position;\n"
            "    oPosObj = position;\n";

  if (tt != RENDER_COMPOSITE_MAP) {
    // determine whether to apply the LOD morph to this vertex
    // we store the deltas against all vertices so we only want to apply
    // the morph to the ones which would disappear. The target LOD which is
    // being morphed to is stored in lodMorph.y, and the LOD at which
    // the vertex should be morphed is stored in uv.w. If we subtract
    // the former from the latter, and arrange to only morph if the
    // result is negative (it will only be -1 in fact, since after that
    // the vertex will never be indexed), we will achieve our aim.
    // sign(vertexLOD - targetLOD) == -1 is to morph
    outStream <<
              "    float toMorph = -min(0.0, sign(delta.y - lodMorph.y));\n";
    // this will either be 1 (morph) or 0 (don't morph)
    if (prof->getParent()->getDebugLevel()) {
      // x == LOD level (-1 since value is target level, we want to display actual)
      outStream << "    lodInfo.x = (lodMorph.y - 1) / " << terrain->getNumLodLevels() << ";\n";
      // y == LOD morph
      outStream << "    lodInfo.y = toMorph * lodMorph.x;\n";
    }

    // morph
    switch (terrain->getAlignment()) {
      case Ogre::Terrain::ALIGN_X_Y:outStream << "    worldPos.z += delta.x * toMorph * lodMorph.x;\n";
        break;
      case Ogre::Terrain::ALIGN_X_Z:outStream << "    worldPos.y += delta.x * toMorph * lodMorph.x;\n";
        break;
      case Ogre::Terrain::ALIGN_Y_Z:outStream << "    worldPos.x += delta.x * toMorph * lodMorph.x;\n";
        break;
    };
  }

  // generate UVs
  if (tt != LOW_LOD) {
    for (Ogre::uint i = 0; i < numUVSets; ++i) {
      Ogre::uint layer = i * 2;
      Ogre::uint uvMulIdx = layer / 4;

      outStream <<
                "    layerUV" << i << ".xy = " << " uv0.xy * uvMul_" << uvMulIdx << "." << getChannel(layer) << ";\n";
      outStream <<
                "    layerUV" << i << ".zw = " << " uv0.xy * uvMul_" << uvMulIdx << "." << getChannel(layer + 1)
                << ";\n";
    }
  }
}
//---------------------------------------------------------------------
void ShaderHelperGLSL::generateFpHeader(const SM2Profile *prof, const Ogre::Terrain *terrain,
                                        TechniqueType tt, Ogre::StringStream &outStream) {
  int version = mIsGLES ? 100 : 120;

  // Main header
  outStream << "#version " << version << "\n";

  if (mIsGLES) {
    outStream << "precision highp int;\n";
    outStream << "precision highp float;\n";
  }

  // helpers
  outStream << "#include <TerrainHelpers.glsl>\n";

  if (prof->isShadowingEnabled(tt, terrain))
    generateFpDynamicShadowsHelpers(prof, terrain, tt, outStream);

  outStream << "varying vec4 oPosObj;\n"
               "varying vec4 oUVMisc;\n";

  Ogre::uint texCoordSet = 1;

  // UV's premultiplied, packed as xy/zw
  Ogre::uint maxLayers = prof->getMaxLayers(terrain);
  Ogre::uint numBlendTextures = std::min(terrain->getBlendTextureCount(maxLayers), terrain->getBlendTextureCount());
  Ogre::uint numLayers = std::min(maxLayers, static_cast<Ogre::uint>(terrain->getLayerCount()));
  Ogre::uint numUVSets = numLayers / 2;
  if (numLayers % 2)
    ++numUVSets;
  if (tt != LOW_LOD) {
    for (Ogre::uint i = 0; i < numUVSets; ++i) {
      outStream <<
                "varying vec4 layerUV" << i << ";\n";
    }
  }
  if (prof->getParent()->getDebugLevel() && tt != RENDER_COMPOSITE_MAP) {
    outStream << "varying vec2 lodInfo;\n";
  }

  bool fog = terrain->getSceneManager()->getFogMode() != Ogre::FOG_NONE && tt != RENDER_COMPOSITE_MAP;
  if (fog) {
    outStream <<
              "uniform vec3 fogColour;\n"
              "uniform vec4 fogParams;\n"
              "varying float fogVal;\n";
  }

  Ogre::uint currentSamplerIdx = 0;

  outStream <<
            // Only 1 light supported in this version
            // deferred shading profile / generator later, ok? :)
            "uniform vec4 lightPosObjSpace;\n"
            "uniform vec3 lightDiffuseColour;\n"
            "uniform vec3 lightSpecularColour;\n"
            "uniform vec3 eyePosObjSpace;\n"
            "uniform vec4 ambient;\n"
            // pack scale, bias and specular
            "uniform vec4 scaleBiasSpecular;\n";

  if (tt == LOW_LOD) {
    // single composite map covers all the others below
    outStream <<
              "uniform sampler2D compositeMap;\n";
  } else {
    outStream <<
              "uniform sampler2D globalNormal;\n";

    if (terrain->getGlobalColourMapEnabled() && prof->isGlobalColourMapEnabled()) {
      outStream << "uniform sampler2D globalColourMap;\n";
    }
    if (prof->isLightmapEnabled()) {
      outStream << "uniform sampler2D lightMap;\n";
    }
    // Blend textures - sampler definitions
    for (Ogre::uint i = 0; i < numBlendTextures; ++i) {
      outStream << "uniform sampler2D blendTex" << i << ";\n";
    }

    // Layer textures - sampler definitions & UV multipliers
    for (Ogre::uint i = 0; i < numLayers; ++i) {
      outStream << "uniform sampler2D difftex" << i << ";\n";
      outStream << "uniform sampler2D normtex" << i << ";\n";
    }
  }

  if (prof->isShadowingEnabled(tt, terrain)) {
    generateFpDynamicShadowsParams(&texCoordSet, &currentSamplerIdx, prof, terrain, tt, outStream);
  }

  // check we haven't exceeded samplers
  if (currentSamplerIdx > 16) {
    OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS,
                "Requested options require too many texture samplers! Try reducing the number of layers.");
  }

  outStream << "void main(void) {\n"
               "    float shadow = 1.0;\n"
               "    vec2 uv = oUVMisc.xy;\n"
               // base colour
               "    gl_FragColor = vec4(0,0,0,1);\n";

  if (tt != LOW_LOD) {
    outStream <<
              // global normal
              "    vec3 normal = expand(texture2D(globalNormal, uv)).rgb;\n";
  }

  outStream <<
            "    vec3 lightDir = \n"
            "        lightPosObjSpace.xyz - (oPosObj.xyz * lightPosObjSpace.w);\n"
            "    vec3 eyeDir = eyePosObjSpace - oPosObj.xyz;\n"

            // set up accumulation areas
            "    vec3 diffuse = vec3(0,0,0);\n"
            "    float specular = 0.0;\n";

  if (tt == LOW_LOD) {
    // we just do a single calculation from composite map
    outStream <<
              "    vec4 composite = texture2D(compositeMap, uv);\n"
              "    diffuse = composite.rgb;\n";
    // TODO - specular; we'll need normals for this!
  } else {
    // set up the blend values
    for (Ogre::uint i = 0; i < numBlendTextures; ++i) {
      outStream << "    vec4 blendTexVal" << i << " = texture2D(blendTex" << i << ", uv);\n";
    }

    if (prof->isLayerNormalMappingEnabled()) {
      // derive the tangent space basis
      // we do this in the pixel shader because we don't have per-vertex normals
      // because of the LOD, we use a normal map
      // tangent is always +x or -z in object space depending on alignment
      switch (terrain->getAlignment()) {
        case Ogre::Terrain::ALIGN_X_Y:
        case Ogre::Terrain::ALIGN_X_Z:outStream << "    vec3 tangent = vec3(1, 0, 0);\n";
          break;
        case Ogre::Terrain::ALIGN_Y_Z:outStream << "    vec3 tangent = vec3(0, 0, -1);\n";
          break;
      };

      outStream << "    vec3 binormal = normalize(cross(tangent, normal));\n";
      // note, now we need to re-cross to derive tangent again because it wasn't orthonormal
      outStream << "    tangent = normalize(cross(normal, binormal));\n";
      // derive final matrix
      outStream << "    mat3 TBN = mat3(tangent, binormal, normal);\n";

      // set up lighting result placeholders for interpolation
      outStream << "    vec4 litRes, litResLayer;\n";
      outStream << "    vec3 TSlightDir, TSeyeDir, TShalfAngle, TSnormal;\n";
      if (prof->isLayerParallaxMappingEnabled())
        outStream << "    float displacement;\n";
      // move
      outStream << "    TSlightDir = normalize(TBN * lightDir);\n";
      outStream << "    TSeyeDir = normalize(TBN * eyeDir);\n";
    } else {
      // simple per-pixel lighting with no normal mapping
      outStream << "    lightDir = normalize(lightDir);\n";
      outStream << "    eyeDir = normalize(eyeDir);\n";
      outStream << "    vec3 halfAngle = normalize(lightDir + eyeDir);\n";
      outStream << "    vec4 litRes = lit(dot(normal, lightDir), dot(normal, halfAngle), scaleBiasSpecular.z);\n";
    }
  }
}
//---------------------------------------------------------------------
void ShaderHelperGLSL::generateVpLayer(const SM2Profile *prof, const Ogre::Terrain *terrain,
                                       TechniqueType tt, Ogre::uint layer, Ogre::StringStream &outStream) {
  // nothing to do
}
//---------------------------------------------------------------------
void ShaderHelperGLSL::generateFpLayer(const SM2Profile *prof, const Ogre::Terrain *terrain,
                                       TechniqueType tt, Ogre::uint layer, Ogre::StringStream &outStream) {
  Ogre::uint uvIdx = layer / 2;
  Ogre::String uvChannels = (layer % 2) ? ".zw" : ".xy";
  Ogre::uint blendIdx = (layer - 1) / 4;
  Ogre::String blendChannel = getChannel(layer - 1);
  Ogre::String
      blendWeightStr = Ogre::String("blendTexVal") + Ogre::StringConverter::toString(blendIdx) + "." + blendChannel;

  // generate early-out conditional
  /* Disable - causing some issues even when trying to force the use of texldd
   if (layer && prof->_isSM3Available())
   outStream << "  if (" << blendWeightStr << " > 0.0003)\n  { \n";
   */

  // generate UV
  outStream << "    vec2 uv" << layer << " = layerUV" << uvIdx << uvChannels << ";\n";

  // calculate lighting here if normal mapping
  if (prof->isLayerNormalMappingEnabled()) {
    if (prof->isLayerParallaxMappingEnabled() && tt != RENDER_COMPOSITE_MAP) {
      // modify UV - note we have to sample an extra time
      outStream << "    displacement = texture2D(normtex" << layer << ", uv" << layer << ").a\n"
                                                                                         "        * scaleBiasSpecular.x + scaleBiasSpecular.y;\n";
      outStream << "    uv" << layer << " += TSeyeDir.xy * displacement;\n";
    }

    // access TS normal map
    outStream << "    TSnormal = expand(texture2D(normtex" << layer << ", uv" << layer << ")).rgb;\n";
    outStream << "    TShalfAngle = normalize(TSlightDir + TSeyeDir);\n";
    outStream << "    litResLayer = lit(dot(TSnormal, TSlightDir), dot(TSnormal, TShalfAngle), scaleBiasSpecular.z);\n";
    if (!layer)
      outStream << "    litRes = litResLayer;\n";
    else
      outStream << "    litRes = mix(litRes, litResLayer, " << blendWeightStr << ");\n";

  }

  // sample diffuse texture
  outStream << "    vec4 diffuseSpecTex" << layer
            << " = texture2D(difftex" << layer << ", uv" << layer << ");\n";

  // apply to common
  if (!layer) {
    outStream << "    diffuse = diffuseSpecTex0.rgb;\n";
    outStream << "    diffuse = pow(diffuse, vec3(2.2));\n";
    if (prof->isLayerSpecularMappingEnabled())
      outStream << "    specular = diffuseSpecTex0.a;\n";
    outStream << "    specular = specular;\n";
  } else {
    outStream << "    diffuse = mix(diffuse, diffuseSpecTex" << layer
              << ".rgb, " << blendWeightStr << ");\n";
    if (prof->isLayerSpecularMappingEnabled())
      outStream << "    specular = mix(specular, diffuseSpecTex" << layer
                << ".a, " << blendWeightStr << ");\n";
  }

  // End early-out
  /* Disable - causing some issues even when trying to force the use of texldd
   if (layer && prof->_isSM3Available())
   outStream << "  } // early-out blend value\n";
   */
}
//---------------------------------------------------------------------
void ShaderHelperGLSL::generateVpFooter(const SM2Profile *prof, const Ogre::Terrain *terrain,
                                        TechniqueType tt, Ogre::StringStream &outStream) {
  outStream <<
            "    gl_Position = viewProjMatrix * worldPos;\n"
            "    oUVMisc.xy = uv0.xy;\n";

  if (!terrain_fog_perpixel_) {
    bool fog = terrain->getSceneManager()->getFogMode() != Ogre::FOG_NONE && tt != RENDER_COMPOSITE_MAP;
    if (fog) {
      if (terrain->getSceneManager()->getFogMode() == Ogre::FOG_LINEAR) {
        outStream <<
                  "    fogVal = clamp((gl_Position.z - fogParams.y) * fogParams.w, 0.0, 1.0);\n";
      } else if (terrain->getSceneManager()->getFogMode() == Ogre::FOG_EXP) {
        outStream <<
                  "    float exponent = gl_Position.z * fogParams.x;\n"
                  "    fogVal = 1.0 - clamp(1.0 / (pow(2.71828, exponent)), 0.0, 1.0);\n";
      } else if (terrain->getSceneManager()->getFogMode() == Ogre::FOG_EXP2) {
        outStream <<
                  "    float exponent = gl_Position.z * fogParams.x;\n"
                  "    fogVal = 1.0 - clamp(1.0 / (pow(2.71828, exponent * exponent)), 0.0, 1.0);\n";
      } else {
        //Fog none, do nothing
      }
    }
  }

  if (prof->isShadowingEnabled(tt, terrain))
    generateVpDynamicShadows(prof, terrain, tt, outStream);

  outStream <<
            "    // pass cam depth\n"
            "    oUVMisc.z = gl_Position.z;\n";

  outStream << "}\n";
}
//---------------------------------------------------------------------
void ShaderHelperGLSL::generateFpFooter(const SM2Profile *prof, const Ogre::Terrain *terrain,
                                        TechniqueType tt, Ogre::StringStream &outStream) {
  if (tt == LOW_LOD) {
    if (prof->isShadowingEnabled(tt, terrain)) {
      generateFpDynamicShadows(prof, terrain, tt, outStream);
      outStream <<
                "    gl_FragColor.rgb = diffuse * rtshadow;\n";
    } else {
      outStream <<
                "    gl_FragColor.rgb = diffuse;\n";
    }
  } else {
    if (terrain->getGlobalColourMapEnabled() && prof->isGlobalColourMapEnabled()) {
      // sample colour map and apply to diffuse
      outStream << "    diffuse *= texture2D(globalColourMap, uv).rgb;\n";
    }
    if (prof->isLightmapEnabled()) {
      // sample lightmap
      outStream << "    shadow = texture2D(lightMap, uv).r;\n";
    }

    if (prof->isShadowingEnabled(tt, terrain)) {
      generateFpDynamicShadows(prof, terrain, tt, outStream);
    }

    // diffuse lighting
    outStream << "    gl_FragColor.rgb += ambient.rgb * diffuse + litRes.y * lightDiffuseColour * diffuse;\n";

    // specular default
    if (!prof->isLayerSpecularMappingEnabled())
      outStream << "    specular = 1.0;\n";

    if (tt == RENDER_COMPOSITE_MAP) {
      // Lighting embedded in alpha
      outStream <<
                "    gl_FragColor.a = shadow;\n";
    } else {
      // Apply specular
      outStream << "    gl_FragColor.rgb += litRes.z * lightSpecularColour * specular;\n";

      if (prof->getParent()->getDebugLevel()) {
        outStream << "    gl_FragColor.rg += lodInfo.xy;\n";
      }
    }
  }

  outStream << "    gl_FragColor.rgb = pow(gl_FragColor.rgb, vec3(1/2.2));\n";
  outStream << "    gl_FragColor.a = shadow;\n";

  bool fog = terrain->getSceneManager()->getFogMode() != Ogre::FOG_NONE && tt != RENDER_COMPOSITE_MAP;

  if (fog) {
    if (terrain_fog_perpixel_) {
      if (terrain->getSceneManager()->getFogMode() == Ogre::FOG_LINEAR) {
        outStream << "    float dist = oUVMisc.z;\n";
        outStream << "    float fog_val =  clamp((dist - fogParams.y) * fogParams.w, 0.0, 1.0);\n";
        outStream << "    gl_FragColor.rgb = mix(gl_FragColor.rgb, fogColour, fog_val);\n";
      } else if (terrain->getSceneManager()->getFogMode() == Ogre::FOG_EXP) {
        outStream << "    float dist = oUVMisc.z;\n";
        outStream << "    float exponent = dist * fogParams.x;\n";
        outStream << "    float fog_val = 1.0 - clamp(1.0 / exp(exponent), 0.0, 1.0);\n";
        outStream << "    gl_FragColor.rgb = mix(gl_FragColor.rgb, fogColour, fog_val);\n";
      } else if (terrain->getSceneManager()->getFogMode() == Ogre::FOG_EXP2) {
        outStream << "    float dist = oUVMisc.z;\n";
        outStream << "    float exponent = dist * fogParams.x;\n";
        outStream << "    float fog_val = 1.0 - clamp(1.0 /exp(exponent * exponent), 0.0, 1.0);\n";
        outStream << "    gl_FragColor.rgb = mix(gl_FragColor.rgb, fogColour, fog_val);\n";
      } else {
        //Fog none, do nothing
      }
    } else {
      outStream << "    gl_FragColor.rgb = mix(gl_FragColor.rgb, fogColour, fogVal);\n";
    }
  }

  // Final return
  outStream << "}\n";
}
//---------------------------------------------------------------------
void ShaderHelperGLSL::generateFpDynamicShadowsHelpers(const SM2Profile *prof, const Ogre::Terrain *terrain,
                                                       TechniqueType tt, Ogre::StringStream &outStream) {
  if (prof->getReceiveDynamicShadowsPSSM()) {
    Ogre::uint numTextures = prof->getReceiveDynamicShadowsPSSM()->getSplitCount();

    if (prof->getReceiveDynamicShadowsDepth()) {
      outStream <<
                "float calcPSSMDepthShadow(";
    } else {
      outStream <<
                "float calcPSSMSimpleShadow(";
    }

    outStream << "\n    ";
    for (Ogre::uint i = 0; i < numTextures; ++i)
      outStream << "sampler2D shadowMap" << i << ", ";
    outStream << "\n    ";
    for (Ogre::uint i = 0; i < numTextures; ++i)
      outStream << "vec4 lsPos" << i << ", ";
    if (prof->getReceiveDynamicShadowsDepth()) {
      outStream << "\n    ";
      for (Ogre::uint i = 0; i < numTextures; ++i)
        outStream << "float invShadowmapSize" << i << ", ";
    }
    outStream << "\n"
                 "    vec4 pssmSplitPoints, float camDepth) \n"
                 "    { \n"
                 "    float shadow; \n"
                 "    // calculate shadow \n";

    for (Ogre::uint i = 0; i < numTextures; ++i) {
      if (!i)
        outStream << "    if (camDepth <= pssmSplitPoints." << ShaderHelper::getChannel(i) << ") \n";
      else if (i < numTextures - 1)
        outStream << "    else if (camDepth <= pssmSplitPoints." << ShaderHelper::getChannel(i) << ") \n";
      else
        outStream << "    else \n";

      outStream <<
                "    { \n";
      if (prof->getReceiveDynamicShadowsDepth()) {
        outStream <<
                  "        shadow = calcDepthShadow(shadowMap" << i << ", lsPos" << i << ", invShadowmapSize" << i
                  << "); \n";
      } else {
        outStream <<
                  "        shadow = calcSimpleShadow(shadowMap" << i << ", lsPos" << i << "); \n";
      }
      outStream << "    } \n";
    }

    outStream <<
              "    return shadow; \n"
              "} \n\n\n";
  }
}
//---------------------------------------------------------------------
Ogre::uint ShaderHelperGLSL::generateVpDynamicShadowsParams(Ogre::uint texCoord, const SM2Profile *prof,
                                                            const Ogre::Terrain *terrain, TechniqueType tt,
                                                            Ogre::StringStream &outStream) {
  // out semantics & params
  Ogre::uint numTextures = 1;
  if (prof->getReceiveDynamicShadowsPSSM()) {
    numTextures = prof->getReceiveDynamicShadowsPSSM()->getSplitCount();
  }
  for (Ogre::uint i = 0; i < numTextures; ++i) {
    outStream <<
              "    varying vec4 oLightSpacePos" << i << ";\n" <<
              "    uniform mat4 texViewProjMatrix" << i << ";\n";
  }

  return texCoord;
}
//---------------------------------------------------------------------
void ShaderHelperGLSL::generateVpDynamicShadows(const SM2Profile *prof, const Ogre::Terrain *terrain,
                                                TechniqueType tt, Ogre::StringStream &outStream) {
  Ogre::uint numTextures = 1;
  if (prof->getReceiveDynamicShadowsPSSM()) {
    numTextures = prof->getReceiveDynamicShadowsPSSM()->getSplitCount();
  }

  // Calculate the position of vertex in light space
  for (Ogre::uint i = 0; i < numTextures; ++i) {
    outStream <<
              "    oLightSpacePos" << i << " = texViewProjMatrix" << i << " * worldPos; \n";
  }

//  if (prof->getReceiveDynamicShadowsPSSM()) {
//    outStream <<
//              "    // pass cam depth\n"
//              "    oUVMisc.z = gl_Position.z;\n";
//  }
}
//---------------------------------------------------------------------
void ShaderHelperGLSL::generateFpDynamicShadowsParams(Ogre::uint *texCoord, Ogre::uint *sampler,
                                                      const SM2Profile *prof, const Ogre::Terrain *terrain,
                                                      TechniqueType tt, Ogre::StringStream &outStream) {
  if (tt == HIGH_LOD)
    mShadowSamplerStartHi = *sampler;
  else if (tt == LOW_LOD)
    mShadowSamplerStartLo = *sampler;

  // in semantics & params
  Ogre::uint numTextures = 1;
  if (prof->getReceiveDynamicShadowsPSSM()) {
    numTextures = prof->getReceiveDynamicShadowsPSSM()->getSplitCount();
    outStream <<
              "uniform vec4 pssmSplitPoints;\n";
  }
  for (Ogre::uint i = 0; i < numTextures; ++i) {
    outStream <<
              "varying vec4 oLightSpacePos" << i << ";\n" <<
              "uniform sampler2D shadowMap" << i << ";\n";
    *sampler = *sampler + 1;
    *texCoord = *texCoord + 1;
    if (prof->getReceiveDynamicShadowsDepth()) {
      outStream <<
                "uniform float inverseShadowmapSize" << i << ";\n";
    }
  }
}
//---------------------------------------------------------------------
void ShaderHelperGLSL::generateFpDynamicShadows(const SM2Profile *prof, const Ogre::Terrain *terrain,
                                                TechniqueType tt, Ogre::StringStream &outStream) {
  if (prof->getReceiveDynamicShadowsPSSM()) {
    Ogre::uint numTextures = prof->getReceiveDynamicShadowsPSSM()->getSplitCount();
    outStream <<
              "    float camDepth = oUVMisc.z;\n";

    if (prof->getReceiveDynamicShadowsDepth()) {
      outStream <<
                "    float rtshadow = calcPSSMDepthShadow(";
    } else {
      outStream <<
                "    float rtshadow = calcPSSMSimpleShadow(";
    }
    for (Ogre::uint i = 0; i < numTextures; ++i)
      outStream << "shadowMap" << i << ", ";
    outStream << "\n        ";

    for (Ogre::uint i = 0; i < numTextures; ++i)
      outStream << "oLightSpacePos" << i << ", ";
    if (prof->getReceiveDynamicShadowsDepth()) {
      outStream << "\n        ";
      for (Ogre::uint i = 0; i < numTextures; ++i)
        outStream << "inverseShadowmapSize" << i << ", ";
    }
    outStream << "\n" <<
              "        pssmSplitPoints, camDepth);\n";
  } else {
    if (prof->getReceiveDynamicShadowsDepth()) {
      outStream <<
                "    float rtshadow = calcDepthShadow(shadowMap0, oLightSpacePos0, inverseShadowmapSize0);";
    } else {
      outStream <<
                "    float rtshadow = calcSimpleShadow(shadowMap0, oLightSpacePos0);";
    }
  }

  outStream <<
            "    shadow = min(shadow, rtshadow);\n";
}
//---------------------------------------------------------------------
ShaderHelperCg::ShaderHelperCg() : ShaderHelper(false) {
  mSM4Available = Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("ps_4_0");
}
//---------------------------------------------------------------------
Ogre::HighLevelGpuProgramPtr
ShaderHelperCg::createVertexProgram(
    const SM2Profile *prof, const Ogre::Terrain *terrain, TechniqueType tt) {
  Ogre::HighLevelGpuProgramManager &mgr = Ogre::HighLevelGpuProgramManager::getSingleton();
  Ogre::String progName = getVertexProgramName(prof, terrain, tt);

  Ogre::String lang = mgr.isLanguageSupported("hlsl") ? "hlsl" : "cg";

  Ogre::HighLevelGpuProgramPtr ret = mgr.getByName(progName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  if (!ret) {
    ret = mgr.createProgram(progName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                            lang, Ogre::GPT_VERTEX_PROGRAM);
  } else {
    ret->unload();
  }

  if (lang == "hlsl") {
    ret->setParameter("enable_backwards_compatibility", "true");
    ret->setParameter("target", "vs_4_0 vs_3_0 vs_2_0");
  } else {
    ret->setParameter("profiles", "vs_4_0 vs_3_0 vs_2_0 arbvp1");
  }

  ret->setParameter("entry_point", "main_vp");

  return ret;

}
//---------------------------------------------------------------------
Ogre::HighLevelGpuProgramPtr
ShaderHelperCg::createFragmentProgram(
    const SM2Profile *prof, const Ogre::Terrain *terrain, TechniqueType tt) {
  Ogre::HighLevelGpuProgramManager &mgr = Ogre::HighLevelGpuProgramManager::getSingleton();
  Ogre::String progName = getFragmentProgramName(prof, terrain, tt);

  Ogre::String lang = mgr.isLanguageSupported("hlsl") ? "hlsl" : "cg";

  Ogre::HighLevelGpuProgramPtr ret = mgr.getByName(progName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  if (!ret) {
    ret = mgr.createProgram(progName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                            lang, Ogre::GPT_FRAGMENT_PROGRAM);
  } else {
    ret->unload();
  }

  if (lang == "hlsl") {
    ret->setParameter("enable_backwards_compatibility", "true");
    ret->setParameter("target", "ps_4_0 ps_3_0 ps_2_b");
  } else {
    if (prof->isLayerNormalMappingEnabled() || prof->isLayerParallaxMappingEnabled())
      ret->setParameter("profiles", "ps_4_0 ps_3_0 ps_2_x fp40 arbfp1");
    else
      ret->setParameter("profiles", "ps_4_0 ps_3_0 ps_2_0 fp30 arbfp1");
  }
  ret->setParameter("entry_point", "main_fp");

  return ret;

}
//---------------------------------------------------------------------
void ShaderHelperCg::generateVpHeader(
    const SM2Profile *prof, const Ogre::Terrain *terrain, TechniqueType tt, Ogre::StringStream &outStream) {
  outStream <<
            "void main_vp(\n";
  bool compression = terrain->_getUseVertexCompression() && tt != RENDER_COMPOSITE_MAP;
  if (compression) {
    const char *idx2 = mSM4Available ? "int2" : "float2";
    outStream <<
              idx2 << " posIndex : POSITION,\n"
                      "float height  : TEXCOORD0,\n";
  } else {
    outStream <<
              "float4 pos : POSITION,\n"
              "float2 uv  : TEXCOORD0,\n";

  }
  if (tt != RENDER_COMPOSITE_MAP)
    outStream << "float2 delta  : TEXCOORD1,\n"; // lodDelta, lodThreshold

  outStream <<
            "uniform float4x4 worldMatrix,\n"
            "uniform float4x4 viewProjMatrix,\n"
            "uniform float2   lodMorph,\n"; // morph amount, morph LOD target

  if (compression) {
    outStream <<
              "uniform float4x4   posIndexToObjectSpace,\n"
              "uniform float    baseUVScale,\n";
  }
  // uv multipliers
  Ogre::uint maxLayers = prof->getMaxLayers(terrain);
  Ogre::uint numLayers = std::min(maxLayers, static_cast<Ogre::uint>(terrain->getLayerCount()));
  Ogre::uint numUVMultipliers = (numLayers / 4);
  if (numLayers % 4)
    ++numUVMultipliers;
  for (Ogre::uint i = 0; i < numUVMultipliers; ++i)
    outStream << "uniform float4 uvMul_" << i << ", \n";

  outStream <<
            "out float4 oPos : POSITION,\n"
            "out float4 oPosObj : TEXCOORD0 \n";

  Ogre::uint texCoordSet = 1;
  outStream <<
            ", out float4 oUVMisc : TEXCOORD" << texCoordSet++ << " // xy = uv, z = camDepth\n";

  // layer UV's premultiplied, packed as xy/zw
  Ogre::uint numUVSets = numLayers / 2;
  if (numLayers % 2)
    ++numUVSets;
  if (tt != LOW_LOD) {
    for (Ogre::uint i = 0; i < numUVSets; ++i) {
      outStream <<
                ", out float4 oUV" << i << " : TEXCOORD" << texCoordSet++ << "\n";
    }
  }

  if (prof->getParent()->getDebugLevel() && tt != RENDER_COMPOSITE_MAP) {
    outStream << ", out float2 lodInfo : TEXCOORD" << texCoordSet++ << "\n";
  }

  bool fog = terrain->getSceneManager()->getFogMode() != Ogre::FOG_NONE && tt != RENDER_COMPOSITE_MAP;
  if (fog) {
    outStream <<
              ", uniform float4 fogParams\n"
              ", out float fogVal : COLOR\n";
  }

  if (prof->isShadowingEnabled(tt, terrain)) {
    texCoordSet = generateVpDynamicShadowsParams(texCoordSet, prof, terrain, tt, outStream);
  }

  // check we haven't exceeded texture coordinates
  if (texCoordSet > 8) {
    OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS,
                "Requested options require too many texture coordinate sets! Try reducing the number of layers.");
  }

  outStream <<
            ")\n"
            "{\n";
  if (compression) {
    outStream <<
              "   float4 pos;\n"
              "   pos = mul(posIndexToObjectSpace, float4(posIndex, height, 1));\n"
              "   float2 uv = float2(posIndex.x * baseUVScale, 1.0 - (posIndex.y * baseUVScale));\n";
  }
  outStream <<
            "   float4 worldPos = mul(worldMatrix, pos);\n"
            "   oPosObj = pos;\n";

  if (tt != RENDER_COMPOSITE_MAP) {
    // determine whether to apply the LOD morph to this vertex
    // we store the deltas against all vertices so we only want to apply
    // the morph to the ones which would disappear. The target LOD which is
    // being morphed to is stored in lodMorph.y, and the LOD at which
    // the vertex should be morphed is stored in uv.w. If we subtract
    // the former from the latter, and arrange to only morph if the
    // result is negative (it will only be -1 in fact, since after that
    // the vertex will never be indexed), we will achieve our aim.
    // sign(vertexLOD - targetLOD) == -1 is to morph
    outStream <<
              "   float toMorph = -min(0, sign(delta.y - lodMorph.y));\n";
    // this will either be 1 (morph) or 0 (don't morph)
    if (prof->getParent()->getDebugLevel()) {
      // x == LOD level (-1 since value is target level, we want to display actual)
      outStream << "lodInfo.x = (lodMorph.y - 1) / " << terrain->getNumLodLevels() << ";\n";
      // y == LOD morph
      outStream << "lodInfo.y = toMorph * lodMorph.x;\n";
    }

    // morph
    switch (terrain->getAlignment()) {
      case Ogre::Terrain::ALIGN_X_Y:outStream << "  worldPos.z += delta.x * toMorph * lodMorph.x;\n";
        break;
      case Ogre::Terrain::ALIGN_X_Z:outStream << "  worldPos.y += delta.x * toMorph * lodMorph.x;\n";
        break;
      case Ogre::Terrain::ALIGN_Y_Z:outStream << "  worldPos.x += delta.x * toMorph * lodMorph.x;\n";
        break;
    };
  }


  // generate UVs
  if (tt != LOW_LOD) {
    for (Ogre::uint i = 0; i < numUVSets; ++i) {
      Ogre::uint layer = i * 2;
      Ogre::uint uvMulIdx = layer / 4;

      outStream <<
                "   oUV" << i << ".xy = " << " uv.xy * uvMul_" << uvMulIdx << "." << getChannel(layer) << ";\n";
      outStream <<
                "   oUV" << i << ".zw = " << " uv.xy * uvMul_" << uvMulIdx << "." << getChannel(layer + 1) << ";\n";

    }

  }

}

//---------------------------------------------------------------------
void ShaderHelperCg::generateFpHeader(
    const SM2Profile *prof, const Ogre::Terrain *terrain, TechniqueType tt, Ogre::StringStream &outStream) {

  // Main header
  outStream << "#include <HLSL_SM4Support.hlsl>\n";
  outStream << "#include <TerrainHelpers.cg>\n";

  if (prof->isShadowingEnabled(tt, terrain))
    generateFpDynamicShadowsHelpers(prof, terrain, tt, outStream);

  // UV's premultiplied, packed as xy/zw
  Ogre::uint maxLayers = prof->getMaxLayers(terrain);
  Ogre::uint numBlendTextures = std::min(terrain->getBlendTextureCount(maxLayers), terrain->getBlendTextureCount());
  Ogre::uint numLayers = std::min(maxLayers, static_cast<Ogre::uint>(terrain->getLayerCount()));

  Ogre::uint currentSamplerIdx = 0;
  if (tt == LOW_LOD) {
    // single composite map covers all the others below
    outStream << Ogre::StringUtil::format("SAMPLER2D(compositeMap, %d);\n", currentSamplerIdx++);
  } else {
    outStream << Ogre::StringUtil::format("SAMPLER2D(globalNormal, %d);\n", currentSamplerIdx++);

    if (terrain->getGlobalColourMapEnabled() && prof->isGlobalColourMapEnabled()) {
      outStream << Ogre::StringUtil::format("SAMPLER2D(globalColourMap, %d);\n", currentSamplerIdx++);
    }
    if (prof->isLightmapEnabled()) {
      outStream << Ogre::StringUtil::format("SAMPLER2D(lightMap, %d);\n", currentSamplerIdx++);
    }
    // Blend textures - sampler definitions
    for (Ogre::uint i = 0; i < numBlendTextures; ++i) {
      outStream << Ogre::StringUtil::format("SAMPLER2D(blendTex%d, %d);\n", i, currentSamplerIdx++);
    }

    // Layer textures - sampler definitions & UV multipliers
    for (Ogre::uint i = 0; i < numLayers; ++i) {
      outStream << Ogre::StringUtil::format("SAMPLER2D(difftex%d, %d);\n", i, currentSamplerIdx++);
      outStream << Ogre::StringUtil::format("SAMPLER2D(normtex%d, %d);\n", i, currentSamplerIdx++);
    }
  }

  if (prof->isShadowingEnabled(tt, terrain)) {
    Ogre::uint numTextures = 1;
    if (prof->getReceiveDynamicShadowsPSSM())
      numTextures = prof->getReceiveDynamicShadowsPSSM()->getSplitCount();
    Ogre::uint sampler = currentSamplerIdx;
    for (Ogre::uint i = 0; i < numTextures; ++i) {
      outStream << Ogre::StringUtil::format("SAMPLER2D(shadowMap%d, %d);\n", i, sampler++);
    }
  }

  outStream <<
            "float4 main_fp(\n"
            "float4 vertexPos : POSITION,\n"
            "float4 position : TEXCOORD0,\n";

  Ogre::uint texCoordSet = 1;
  outStream <<
            "float4 uvMisc : TEXCOORD" << texCoordSet++ << ",\n";

  Ogre::uint numUVSets = numLayers / 2;
  if (numLayers % 2)
    ++numUVSets;
  if (tt != LOW_LOD) {
    for (Ogre::uint i = 0; i < numUVSets; ++i) {
      outStream <<
                "float4 layerUV" << i << " : TEXCOORD" << texCoordSet++ << ", \n";
    }

  }
  if (prof->getParent()->getDebugLevel() && tt != RENDER_COMPOSITE_MAP) {
    outStream << "float2 lodInfo : TEXCOORD" << texCoordSet++ << ", \n";
  }

  bool fog = terrain->getSceneManager()->getFogMode() != Ogre::FOG_NONE && tt != RENDER_COMPOSITE_MAP;
  if (fog) {
    outStream <<
              "uniform float3 fogColour, \n"
              "float fogVal : COLOR,\n";
  }

  outStream <<
            // Only 1 light supported in this version
            // deferred shading profile / generator later, ok? :)
            "uniform float3 ambient,\n"
            "uniform float4 lightPosObjSpace,\n"
            "uniform float3 lightDiffuseColour,\n"
            "uniform float3 lightSpecularColour,\n"
            "uniform float3 eyePosObjSpace,\n"
            // pack scale, bias and specular
            "uniform float4 scaleBiasSpecular\n";

  if (prof->isShadowingEnabled(tt, terrain)) {
    generateFpDynamicShadowsParams(&texCoordSet, &currentSamplerIdx, prof, terrain, tt, outStream);
  }

  // check we haven't exceeded samplers
  if (currentSamplerIdx > 16) {
    OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS,
                "Requested options require too many texture samplers! Try reducing the number of layers.");
  }

  outStream <<
            ") : COLOR\n"
            "{\n"
            "   float4 outputCol;\n"
            "   float shadow = 1.0;\n"
            "   float2 uv = uvMisc.xy;\n"
            // base colour
            "   outputCol = float4(0,0,0,1);\n";

  if (tt != LOW_LOD) {
    outStream <<
              // global normal
              "   float3 normal = expand(tex2D(globalNormal, uv)).rgb;\n";

  }

  outStream <<
            "   float3 lightDir = \n"
            "       lightPosObjSpace.xyz -  (position.xyz * lightPosObjSpace.w);\n"
            "   float3 eyeDir = eyePosObjSpace - position.xyz;\n"

            // set up accumulation areas
            "   float3 diffuse = float3(0,0,0);\n"
            "   float specular = 0;\n";

  if (tt == LOW_LOD) {
    // we just do a single calculation from composite map
    outStream <<
              "   float4 composite = tex2D(compositeMap, uv);\n"
              "   diffuse = composite.rgb;\n";
    // TODO - specular; we'll need normals for this!
  } else {
    // set up the blend values
    for (Ogre::uint i = 0; i < numBlendTextures; ++i) {
      outStream << "  float4 blendTexVal" << i << " = tex2D(blendTex" << i << ", uv);\n";
    }

    if (prof->isLayerNormalMappingEnabled()) {
      // derive the tangent space basis
      // we do this in the pixel shader because we don't have per-vertex normals
      // because of the LOD, we use a normal map
      // tangent is always +x or -z in object space depending on alignment
      switch (terrain->getAlignment()) {
        case Ogre::Terrain::ALIGN_X_Y:
        case Ogre::Terrain::ALIGN_X_Z:outStream << "  float3 tangent = float3(1, 0, 0);\n";
          break;
        case Ogre::Terrain::ALIGN_Y_Z:outStream << "  float3 tangent = float3(0, 0, -1);\n";
          break;
      };

      outStream << "  float3 binormal = normalize(cross(tangent, normal));\n";
      // note, now we need to re-cross to derive tangent again because it wasn't orthonormal
      outStream << "  tangent = normalize(cross(normal, binormal));\n";
      // derive final matrix
      outStream << "  float3x3 TBN = float3x3(tangent, binormal, normal);\n";

      // set up lighting result placeholders for interpolation
      outStream << " float4 litRes, litResLayer;\n";
      outStream << "  float3 TSlightDir, TSeyeDir, TShalfAngle, TSnormal;\n";
      if (prof->isLayerParallaxMappingEnabled())
        outStream << "  float displacement;\n";
      // move
      outStream << "  TSlightDir = normalize(mul(TBN, lightDir));\n";
      outStream << "  TSeyeDir = normalize(mul(TBN, eyeDir));\n";

    } else {
      // simple per-pixel lighting with no normal mapping
      outStream << "  lightDir = normalize(lightDir);\n";
      outStream << "  eyeDir = normalize(eyeDir);\n";
      outStream << "  float3 halfAngle = normalize(lightDir + eyeDir);\n";
      outStream << "  float4 litRes = lit(dot(lightDir, normal), dot(halfAngle, normal), scaleBiasSpecular.z);\n";

    }
  }

}
//---------------------------------------------------------------------
void ShaderHelperCg::generateVpLayer(
    const SM2Profile *prof,
    const Ogre::Terrain *terrain,
    TechniqueType tt,
    Ogre::uint layer,
    Ogre::StringStream &outStream) {
  // nothing to do
}
//---------------------------------------------------------------------
void ShaderHelperCg::generateFpLayer(
    const SM2Profile *prof,
    const Ogre::Terrain *terrain,
    TechniqueType tt,
    Ogre::uint layer,
    Ogre::StringStream &outStream) {
  Ogre::uint uvIdx = layer / 2;
  Ogre::String uvChannels = (layer % 2) ? ".zw" : ".xy";
  Ogre::uint blendIdx = (layer - 1) / 4;
  Ogre::String blendChannel = getChannel(layer - 1);
  Ogre::String blendWeightStr = Ogre::String("blendTexVal") + Ogre::StringConverter::toString(blendIdx) +
      "." + blendChannel;

  // generate early-out conditional
  /* Disable - causing some issues even when trying to force the use of texldd
  if (layer && prof->_isSM3Available())
      outStream << "  if (" << blendWeightStr << " > 0.0003)\n  { \n";
  */

  // generate UV
  outStream << "  float2 uv" << layer << " = layerUV" << uvIdx << uvChannels << ";\n";

  // calculate lighting here if normal mapping
  if (prof->isLayerNormalMappingEnabled()) {
    if (prof->isLayerParallaxMappingEnabled() && tt != RENDER_COMPOSITE_MAP) {
      // modify UV - note we have to sample an extra time
      outStream << "  displacement = tex2D(normtex" << layer << ", uv" << layer << ").a\n"
                                                                                   "       * scaleBiasSpecular.x + scaleBiasSpecular.y;\n";
      outStream << "  uv" << layer << " += TSeyeDir.xy * displacement;\n";
    }

    // access TS normal map
    outStream << "  TSnormal = expand(tex2D(normtex" << layer << ", uv" << layer << ")).rgb;\n";
    outStream << "  TShalfAngle = normalize(TSlightDir + TSeyeDir);\n";
    outStream << "  litResLayer = lit(dot(TSlightDir, TSnormal), dot(TShalfAngle, TSnormal), scaleBiasSpecular.z);\n";
    if (!layer)
      outStream << "  litRes = litResLayer;\n";
    else
      outStream << "  litRes = lerp(litRes, litResLayer, " << blendWeightStr << ");\n";

  }

  // sample diffuse texture
  outStream << "  float4 diffuseSpecTex" << layer
            << " = tex2D(difftex" << layer << ", uv" << layer << ");\n";

  // apply to common
  if (!layer) {
    outStream << "  diffuse = diffuseSpecTex0.rgb;\n";
    if (prof->isLayerSpecularMappingEnabled())
      outStream << "  specular = diffuseSpecTex0.a;\n";
  } else {
    outStream << "  diffuse = lerp(diffuse, diffuseSpecTex" << layer
              << ".rgb, " << blendWeightStr << ");\n";
    if (prof->isLayerSpecularMappingEnabled())
      outStream << "  specular = lerp(specular, diffuseSpecTex" << layer
                << ".a, " << blendWeightStr << ");\n";

  }

  // End early-out
  /* Disable - causing some issues even when trying to force the use of texldd
  if (layer && prof->_isSM3Available())
      outStream << "  } // early-out blend value\n";
  */
}
//---------------------------------------------------------------------
void ShaderHelperCg::generateVpFooter(
    const SM2Profile *prof, const Ogre::Terrain *terrain, TechniqueType tt, Ogre::StringStream &outStream) {

  outStream <<
            "   oPos = mul(viewProjMatrix, worldPos);\n"
            "   oUVMisc.xy = uv.xy;\n";

  bool fog = terrain->getSceneManager()->getFogMode() != Ogre::FOG_NONE && tt != RENDER_COMPOSITE_MAP;
  if (fog) {
    if (terrain->getSceneManager()->getFogMode() == Ogre::FOG_LINEAR) {
      outStream <<
                "   fogVal = saturate((oPos.z - fogParams.y) * fogParams.w);\n";
    } else {
      outStream <<
                "   fogVal = 1 - saturate(1 / (exp(oPos.z * fogParams.x)));\n";
    }
  }

  if (prof->isShadowingEnabled(tt, terrain))
    generateVpDynamicShadows(prof, terrain, tt, outStream);

  outStream <<
            "}\n";

}
//---------------------------------------------------------------------
void ShaderHelperCg::generateFpFooter(
    const SM2Profile *prof, const Ogre::Terrain *terrain, TechniqueType tt, Ogre::StringStream &outStream) {

  if (tt == LOW_LOD) {
    if (prof->isShadowingEnabled(tt, terrain)) {
      generateFpDynamicShadows(prof, terrain, tt, outStream);
      outStream <<
                "   outputCol.rgb = diffuse * rtshadow;\n";
    } else {
      outStream <<
                "   outputCol.rgb = diffuse;\n";
    }
  } else {
    if (terrain->getGlobalColourMapEnabled() && prof->isGlobalColourMapEnabled()) {
      // sample colour map and apply to diffuse
      outStream << "  diffuse *= tex2D(globalColourMap, uv).rgb;\n";
    }
    if (prof->isLightmapEnabled()) {
      // sample lightmap
      outStream << "  shadow = tex2D(lightMap, uv).r;\n";
    }

    if (prof->isShadowingEnabled(tt, terrain)) {
      generateFpDynamicShadows(prof, terrain, tt, outStream);
    }

    // diffuse lighting
    outStream << "  outputCol.rgb += ambient.rgb * diffuse + litRes.y * lightDiffuseColour * diffuse * shadow;\n";

    // specular default
    if (!prof->isLayerSpecularMappingEnabled())
      outStream << "  specular = 1.0;\n";

    if (tt == RENDER_COMPOSITE_MAP) {
      // Lighting embedded in alpha
      outStream <<
                "   outputCol.a = shadow;\n";

    } else {
      // Apply specular
      outStream << "  outputCol.rgb += litRes.z * lightSpecularColour * specular * shadow;\n";

      if (prof->getParent()->getDebugLevel()) {
        outStream << "  outputCol.rg += lodInfo.xy;\n";
      }
    }
  }

  bool fog = terrain->getSceneManager()->getFogMode() != Ogre::FOG_NONE && tt != RENDER_COMPOSITE_MAP;
  if (fog) {
    outStream << "  outputCol.rgb = lerp(outputCol.rgb, fogColour, fogVal);\n";
  }

  // Final return
  outStream << "  return outputCol;\n"
            << "}\n";

}
//---------------------------------------------------------------------
void ShaderHelperCg::generateFpDynamicShadowsHelpers(
    const SM2Profile *prof, const Ogre::Terrain *terrain, TechniqueType tt, Ogre::StringStream &outStream) {
  if (prof->getReceiveDynamicShadowsPSSM()) {
    Ogre::uint numTextures = prof->getReceiveDynamicShadowsPSSM()->getSplitCount();

    if (prof->getReceiveDynamicShadowsDepth()) {
      outStream <<
                "float calcPSSMDepthShadow(";
    } else {
      outStream <<
                "float calcPSSMSimpleShadow(";
    }

    outStream << "\n    ";
    for (Ogre::uint i = 0; i < numTextures; ++i)
      outStream << "sampler2D shadowMap" << i << ", ";
    outStream << "\n    ";
    for (Ogre::uint i = 0; i < numTextures; ++i)
      outStream << "float4 lsPos" << i << ", ";
    if (prof->getReceiveDynamicShadowsDepth()) {
      outStream << "\n    ";
      for (Ogre::uint i = 0; i < numTextures; ++i)
        outStream << "float invShadowmapSize" << i << ", ";
    }
    outStream << "\n"
                 "   float4 pssmSplitPoints, float camDepth) \n"
                 "{ \n"
                 "   float shadow; \n"
                 "   // calculate shadow \n";

    for (Ogre::uint i = 0; i < numTextures; ++i) {
      if (!i)
        outStream << "  if (camDepth <= pssmSplitPoints." << ShaderHelper::getChannel(i) << ") \n";
      else if (i < numTextures - 1)
        outStream << "  else if (camDepth <= pssmSplitPoints." << ShaderHelper::getChannel(i) << ") \n";
      else
        outStream << "  else \n";

      outStream <<
                "   { \n";
      if (prof->getReceiveDynamicShadowsDepth()) {
        outStream <<
                  "       shadow = calcDepthShadow(shadowMap" << i << ", lsPos" << i << ", invShadowmapSize" << i
                  << "); \n";
      } else {
        outStream <<
                  "       shadow = calcSimpleShadow(shadowMap" << i << ", lsPos" << i << "); \n";
      }
      outStream <<
                "   } \n";

    }

    outStream <<
              "   return shadow; \n"
              "} \n\n\n";
  }

}
//---------------------------------------------------------------------
Ogre::uint ShaderHelperCg::generateVpDynamicShadowsParams(
    Ogre::uint texCoord,
    const SM2Profile *prof,
    const Ogre::Terrain *terrain,
    TechniqueType tt,
    Ogre::StringStream &outStream) {
  // out semantics & params
  Ogre::uint numTextures = 1;
  if (prof->getReceiveDynamicShadowsPSSM()) {
    numTextures = prof->getReceiveDynamicShadowsPSSM()->getSplitCount();
  }
  for (Ogre::uint i = 0; i < numTextures; ++i) {
    outStream <<
              ", out float4 oLightSpacePos" << i << " : TEXCOORD" << texCoord++ << " \n" <<
              ", uniform float4x4 texViewProjMatrix" << i << " \n";
  }

  return texCoord;

}
//---------------------------------------------------------------------
void ShaderHelperCg::generateVpDynamicShadows(
    const SM2Profile *prof, const Ogre::Terrain *terrain, TechniqueType tt, Ogre::StringStream &outStream) {
  Ogre::uint numTextures = 1;
  if (prof->getReceiveDynamicShadowsPSSM()) {
    numTextures = prof->getReceiveDynamicShadowsPSSM()->getSplitCount();
  }

  // Calculate the position of vertex in light space
  for (Ogre::uint i = 0; i < numTextures; ++i) {
    outStream <<
              "   oLightSpacePos" << i << " = mul(texViewProjMatrix" << i << ", worldPos); \n";
  }

  if (prof->getReceiveDynamicShadowsPSSM()) {
    outStream <<
              "   // pass cam depth\n"
              "   oUVMisc.z = oPos.z;\n";
  }

}
//---------------------------------------------------------------------
void ShaderHelperCg::generateFpDynamicShadowsParams(
    Ogre::uint *texCoord,
    Ogre::uint *sampler,
    const SM2Profile *prof,
    const Ogre::Terrain *terrain,
    TechniqueType tt,
    Ogre::StringStream &outStream) {
  if (tt == HIGH_LOD)
    mShadowSamplerStartHi = *sampler;
  else if (tt == LOW_LOD)
    mShadowSamplerStartLo = *sampler;

  // in semantics & params
  Ogre::uint numTextures = 1;
  if (prof->getReceiveDynamicShadowsPSSM()) {
    numTextures = prof->getReceiveDynamicShadowsPSSM()->getSplitCount();
    outStream <<
              ", uniform float4 pssmSplitPoints \n";
  }
  for (Ogre::uint i = 0; i < numTextures; ++i) {
    outStream <<
              ", float4 lightSpacePos" << i << " : TEXCOORD" << *texCoord << " \n";
    *sampler = *sampler + 1;
    *texCoord = *texCoord + 1;
    if (prof->getReceiveDynamicShadowsDepth()) {
      outStream <<
                ", uniform float inverseShadowmapSize" << i << " \n";
    }
  }

}
//---------------------------------------------------------------------
void ShaderHelperCg::generateFpDynamicShadows(
    const SM2Profile *prof, const Ogre::Terrain *terrain, TechniqueType tt, Ogre::StringStream &outStream) {
  if (prof->getReceiveDynamicShadowsPSSM()) {
    Ogre::uint numTextures = prof->getReceiveDynamicShadowsPSSM()->getSplitCount();
    outStream <<
              "   float camDepth = uvMisc.z;\n";

    if (prof->getReceiveDynamicShadowsDepth()) {
      outStream <<
                "   float rtshadow = calcPSSMDepthShadow(";
    } else {
      outStream <<
                "   float rtshadow = calcPSSMSimpleShadow(";
    }
    for (Ogre::uint i = 0; i < numTextures; ++i)
      outStream << "shadowMap" << i << ", ";
    outStream << "\n        ";

    for (Ogre::uint i = 0; i < numTextures; ++i)
      outStream << "lightSpacePos" << i << ", ";
    if (prof->getReceiveDynamicShadowsDepth()) {
      outStream << "\n        ";
      for (Ogre::uint i = 0; i < numTextures; ++i)
        outStream << "inverseShadowmapSize" << i << ", ";
    }
    outStream << "\n" <<
              "       pssmSplitPoints, camDepth);\n";

  } else {
    if (prof->getReceiveDynamicShadowsDepth()) {
      outStream <<
                "   float rtshadow = calcDepthShadow(shadowMap0, lightSpacePos0, inverseShadowmapSize0);";
    } else {
      outStream <<
                "   float rtshadow = calcSimpleShadow(shadowMap0, lightSpacePos0);";
    }
  }

  outStream <<
            "   shadow = min(shadow, rtshadow);\n";

}
}
