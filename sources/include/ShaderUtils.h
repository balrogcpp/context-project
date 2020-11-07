//MIT License
//
//Copyright (c) 2020 Andrey Vasiliev
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#pragma once
#include <Ogre.h>
#include <string>
#include <vector>
#include <memory>

namespace xio {
//----------------------------------------------------------------------------------------------------------------------
inline void UpdatePbrShadowCaster(const Ogre::MaterialPtr &material) {
  auto *pass = material->getTechnique(0)->getPass(0);
  int alpha_rejection = static_cast<int>(pass->getAlphaRejectValue());
  bool transparency_casts_shadows = material->getTransparencyCastsShadows();
  int num_textures = pass->getNumTextureUnitStates();
  std::string material_name = material->getName();

  if (num_textures > 0 && alpha_rejection > 0 && transparency_casts_shadows) {
    auto caster_material = Ogre::MaterialManager::getSingleton().getByName("PSSM/shadow_caster_alpha");
    std::string caster_name = "PSSM/shadow_caster_alpha/" + material_name;
    Ogre::MaterialPtr new_caster = Ogre::MaterialManager::getSingleton().getByName(caster_name);

    if (!new_caster) {
      new_caster = caster_material->clone(caster_name);
      auto texture_albedo = pass->getTextureUnitState("Albedo");
      if (texture_albedo) {
        std::string texture_name = pass->getTextureUnitState("Albedo")->getTextureName();

        auto *texPtr3 = new_caster->getTechnique(0)->getPass(0)->getTextureUnitState("BaseColor");

        if (texPtr3) {
          texPtr3->setContentType(Ogre::TextureUnitState::CONTENT_NAMED);
          texPtr3->setTextureAddressingMode(Ogre::TextureAddressingMode::TAM_CLAMP);
          texPtr3->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_POINT);
          texPtr3->setTextureName(texture_name);
        }
      }
    }

    material->getTechnique(0)->setShadowCasterMaterial(new_caster);
  } else {
    std::string caster_name = "PSSM/shadow_caster/" + material_name;
    Ogre::MaterialPtr new_caster = Ogre::MaterialManager::getSingleton().getByName(caster_name);

    if (!new_caster) {
      auto caster_material = Ogre::MaterialManager::getSingleton().getByName("PSSM/shadow_caster");
      new_caster = caster_material->clone(caster_name);
    }

    material->getTechnique(0)->setShadowCasterMaterial(new_caster);
  }
}

//----------------------------------------------------------------------------------------------------------------------
inline void UpdatePbrParams(const Ogre::MaterialPtr &material) {
  const int MAX_LIGHT_COUNT = 10;

  if (!material->getTechnique(0)->getPass(0)->hasVertexProgram() || !material->getTechnique(0)->getPass(0)->hasFragmentProgram()) {
    return;
  }

  auto vert_params = material->getTechnique(0)->getPass(0)->getVertexProgramParameters();
  vert_params->setIgnoreMissingParams(true);

  if (vert_params->getConstantDefinitions().map.count("uMVPMatrix") == 0) {
    return;
  }

  vert_params->setNamedAutoConstant("uMVPMatrix", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
  vert_params->setNamedAutoConstant("uModelMatrix", Ogre::GpuProgramParameters::ACT_WORLD_MATRIX);
  vert_params->setNamedAutoConstant("uCameraPosition", Ogre::GpuProgramParameters::ACT_CAMERA_POSITION);

  vert_params->setNamedConstant("fadeRange", 1.0f / 50.0f);
  vert_params->setNamedConstant("windRange", 10.0f);
  vert_params->setNamedConstantFromTime("uTime", 1.0);

  auto frag_params = material->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
  frag_params->setIgnoreMissingParams(true);

  frag_params->setNamedAutoConstant("uAlphaRejection", Ogre::GpuProgramParameters::ACT_SURFACE_ALPHA_REJECTION_VALUE);
  frag_params->setNamedAutoConstant("uSurfaceAmbientColour", Ogre::GpuProgramParameters::ACT_SURFACE_AMBIENT_COLOUR);
  frag_params->setNamedAutoConstant("uSurfaceDiffuseColour", Ogre::GpuProgramParameters::ACT_SURFACE_DIFFUSE_COLOUR);
  frag_params->setNamedAutoConstant("uSurfaceSpecularColour",Ogre::GpuProgramParameters::ACT_SURFACE_SPECULAR_COLOUR);
  frag_params->setNamedAutoConstant("uSurfaceShininessColour", Ogre::GpuProgramParameters::ACT_SURFACE_SHININESS);
  frag_params->setNamedAutoConstant("uSurfaceEmissiveColour",Ogre::GpuProgramParameters::ACT_SURFACE_EMISSIVE_COLOUR);
  frag_params->setNamedAutoConstant("uAmbientLightColour", Ogre::GpuProgramParameters::ACT_AMBIENT_LIGHT_COLOUR);
  frag_params->setNamedAutoConstant("uLightCount", Ogre::GpuProgramParameters::ACT_LIGHT_COUNT);
  frag_params->setNamedAutoConstant("uLightPositionArray", Ogre::GpuProgramParameters::ACT_LIGHT_POSITION_ARRAY, MAX_LIGHT_COUNT);
  frag_params->setNamedAutoConstant("uLightDirectionArray", Ogre::GpuProgramParameters::ACT_LIGHT_DIRECTION_ARRAY, MAX_LIGHT_COUNT);
  frag_params->setNamedAutoConstant("uLightDiffuseScaledColourArray", Ogre::GpuProgramParameters::ACT_LIGHT_DIFFUSE_COLOUR_POWER_SCALED_ARRAY, MAX_LIGHT_COUNT);
  frag_params->setNamedAutoConstant("uLightAttenuationArray", Ogre::GpuProgramParameters::ACT_LIGHT_ATTENUATION_ARRAY, MAX_LIGHT_COUNT);
  frag_params->setNamedAutoConstant("uLightSpotParamsArray", Ogre::GpuProgramParameters::ACT_SPOTLIGHT_PARAMS_ARRAY, MAX_LIGHT_COUNT);
  frag_params->setNamedAutoConstant("uFogColour", Ogre::GpuProgramParameters::ACT_FOG_COLOUR);
  frag_params->setNamedAutoConstant("uFogParams", Ogre::GpuProgramParameters::ACT_FOG_PARAMS);
  frag_params->setNamedAutoConstant("uCameraPosition", Ogre::GpuProgramParameters::ACT_CAMERA_POSITION);

  frag_params->setNamedConstant("uLOD", 0.5f);
  frag_params->setNamedConstant("uShadowFilterSize", 0.004f);
  frag_params->setNamedConstant("uShadowFilterIterations", 16);
}

//----------------------------------------------------------------------------------------------------------------------
inline void UpdatePbrShadowReceiver(const Ogre::MaterialPtr &material) {
  const int PSSM_SPLIT_COUNT = 3;
  const int MAX_SHADOWS = 5;
  const int MAX_LIGHT_COUNT = 10;

  if (!material->getReceiveShadows() || !material->getTechnique(0)->getPass(0)->hasVertexProgram() || !material->getTechnique(0)->getPass(0)->hasFragmentProgram()) {
    return;
  }

  auto *pssm = dynamic_cast<Ogre::PSSMShadowCameraSetup *>(Ogre::Root::getSingleton().getSceneManager("Default")->getShadowCameraSetup().get());

  auto *technique = material->getTechnique(0);
  auto *pass = technique->getPass(0);
  auto vert_params = pass->getVertexProgramParameters();
  vert_params->setIgnoreMissingParams(true);

  if (vert_params->getConstantDefinitions().map.count("uTexWorldViewProjMatrixArray") == 0) {
    return;
  }

  vert_params->setNamedAutoConstant( "uLightCount", Ogre::GpuProgramParameters::ACT_LIGHT_COUNT);
  vert_params->setNamedAutoConstant("uTexWorldViewProjMatrixArray",Ogre::GpuProgramParameters::ACT_TEXTURE_WORLDVIEWPROJ_MATRIX_ARRAY, MAX_SHADOWS);
  vert_params->setNamedAutoConstant("uLightCastsShadowsArray",Ogre::GpuProgramParameters::ACT_LIGHT_CASTS_SHADOWS_ARRAY, MAX_LIGHT_COUNT);

  auto frag_params = material->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
  frag_params->setIgnoreMissingParams(true);

  Ogre::Vector4 splitPoints;
  const Ogre::PSSMShadowCameraSetup::SplitPointList &splitPointList = pssm->getSplitPoints();

  for (int i = 0; i < PSSM_SPLIT_COUNT; i++) {
    splitPoints[i] = splitPointList[i + 1];
  }

  if (frag_params->getConstantDefinitions().map.count("pssmSplitPoints") == 0) {
    return;
  }

  frag_params->setNamedConstant("pssmSplitPoints", splitPoints);

  frag_params->setNamedAutoConstant("uShadowColour", Ogre::GpuProgramParameters::ACT_SHADOW_COLOUR);
  frag_params->setNamedAutoConstant("uLightCastsShadowsArray",Ogre::GpuProgramParameters::ACT_LIGHT_CASTS_SHADOWS_ARRAY, MAX_LIGHT_COUNT);

  frag_params->setNamedConstant("uShadowFilterSize", 0.004f);
  frag_params->setNamedConstant("uShadowFilterIterations", 16);

  int texture_count = pass->getNumTextureUnitStates();

  for (int i = 0; i < MAX_SHADOWS; i++) {
    if (pass->getTextureUnitState("shadowMap" + std::to_string(i)))
      continue;

    Ogre::TextureUnitState *tu = pass->createTextureUnitState();
    tu->setName("shadowMap" + std::to_string(i));
    tu->setContentType(Ogre::TextureUnitState::CONTENT_SHADOW);
    tu->setTextureAddressingMode(Ogre::TextureUnitState::TAM_BORDER);
    tu->setTextureBorderColour(Ogre::ColourValue::White);
    tu->setTextureFiltering(Ogre::TFO_NONE);
    frag_params->setNamedConstant("shadowMap" + std::to_string(i), texture_count + i);
  }
}

//----------------------------------------------------------------------------------------------------------------------
inline void UpdatePbrIbl(const Ogre::MaterialPtr &material, bool realtime) {
  auto ibl_texture = material->getTechnique(0)->getPass(0)->getTextureUnitState("IBL_Specular");

  if (realtime) {
    auto cubemap = Ogre::TextureManager::getSingleton().getByName("dyncubemap", Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);

    if (ibl_texture)
      ibl_texture->setTexture(cubemap);
  } else {
    auto skybox = Ogre::MaterialManager::getSingleton().getByName("SkyBox");
    if (!skybox)
      return;
    auto cubemap = skybox->getTechnique(0)->getPass(0)->getTextureUnitState("CubeMap");
    if (!cubemap)
      return;

    if (ibl_texture)
      ibl_texture->setTexture(cubemap->_getTexturePtr());
  }
}

//----------------------------------------------------------------------------------------------------------------------
inline void UpdatePbrParams(const std::string &material) {
  auto material_ptr = Ogre::MaterialManager::getSingleton().getByName(material);
  UpdatePbrParams(material_ptr);
}

//----------------------------------------------------------------------------------------------------------------------
inline void UpdatePbrIbl(const std::string &material, bool realtime) {
  auto material_ptr = Ogre::MaterialManager::getSingleton().getByName(material);
  UpdatePbrIbl(material_ptr, realtime);
}

//----------------------------------------------------------------------------------------------------------------------
inline void UpdatePbrShadowReceiver(const std::string &material) {
  auto material_ptr = Ogre::MaterialManager::getSingleton().getByName(material);
  UpdatePbrShadowReceiver(material_ptr);
}

//----------------------------------------------------------------------------------------------------------------------
inline void UpdatePbrShadowCaster(const std::string &material) {
  auto material_ptr = Ogre::MaterialManager::getSingleton().getByName(material);
  UpdatePbrShadowCaster(material_ptr);
}
}