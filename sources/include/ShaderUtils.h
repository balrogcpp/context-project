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
inline void AddGpuConstParameterAuto(const Ogre::GpuProgramParametersSharedPtr &parameters,
                                     const std::string &parameter_name,
                                     const Ogre::GpuProgramParameters::AutoConstantType value_name,
                                     const int info = 0) {
  const auto &constants = parameters->getConstantDefinitions();

  if (constants.map.count(parameter_name) > 0)
    parameters->setNamedAutoConstant(parameter_name, value_name, info);
}
//----------------------------------------------------------------------------------------------------------------------
inline void AddGpuConstParameter(const Ogre::GpuProgramParametersSharedPtr &parameters,
                                 const std::string &parameter_name,
                                 Ogre::Vector4 value) {
  const auto &constants = parameters->getConstantDefinitions();

  if (constants.map.count(parameter_name) > 0)
    parameters->setNamedConstant(parameter_name, value);
}
//----------------------------------------------------------------------------------------------------------------------
inline void AddGpuConstParameter(const Ogre::GpuProgramParametersSharedPtr &parameters,
                                 const std::string &parameter_name,
                                 Ogre::Vector3 value) {
  const auto &constants = parameters->getConstantDefinitions();

  if (constants.map.count(parameter_name) > 0)
    parameters->setNamedConstant(parameter_name, value);
}
//----------------------------------------------------------------------------------------------------------------------
inline void AddGpuConstParameter(const Ogre::GpuProgramParametersSharedPtr &parameters,
                                 const std::string &parameter_name,
                                 Ogre::Matrix4 value) {
  const auto &constants = parameters->getConstantDefinitions();

  if (constants.map.count(parameter_name) > 0)
    parameters->setNamedConstant(parameter_name, value);
}
//----------------------------------------------------------------------------------------------------------------------
inline void AddGpuConstParameter(const Ogre::GpuProgramParametersSharedPtr &parameters,
                                 const std::string &parameter_name,
                                 Ogre::Vector2 value) {
  const auto &constants = parameters->getConstantDefinitions();

  if (constants.map.count(parameter_name) > 0)
    parameters->setNamedConstant(parameter_name, value);
}
//----------------------------------------------------------------------------------------------------------------------
inline void AddGpuConstParameter(const Ogre::GpuProgramParametersSharedPtr &parameters,
                                 const std::string &parameter_name,
                                 float value) {
  const auto &constants = parameters->getConstantDefinitions();

  if (constants.map.count(parameter_name) > 0)
    parameters->setNamedConstant(parameter_name, value);
}
//----------------------------------------------------------------------------------------------------------------------
inline void AddGpuConstParameter(const Ogre::GpuProgramParametersSharedPtr &parameters,
                                 const std::string &parameter_name,
                                 int value) {
  const auto &constants = parameters->getConstantDefinitions();

  if (constants.map.count(parameter_name) > 0)
    parameters->setNamedConstant(parameter_name, value);
}
//----------------------------------------------------------------------------------------------------------------------
inline void AddGpuConstParameter(const Ogre::GpuProgramParametersSharedPtr &parameters,
                                 const std::string &parameter_name,
                                 unsigned int value) {
  const auto &constants = parameters->getConstantDefinitions();

  if (constants.map.count(parameter_name) > 0)
    parameters->setNamedConstant(parameter_name, value);
}

//----------------------------------------------------------------------------------------------------------------------
inline void UpdatePbrShadowCaster(Ogre::MaterialPtr material) {
  std::string material_name = material->getName();
  static std::vector<std::string> material_list;

  if (std::find(material_list.begin(), material_list.end(), material_name) != material_list.end()) {
    return;
  } else {
    material_list.push_back(material_name);
  }

  auto *pass = material->getTechnique(0)->getPass(0);
  int alpha_rejection = static_cast<int>(pass->getAlphaRejectValue());
  bool transparency_casts_shadows = material->getTransparencyCastsShadows();
  int num_textures = pass->getNumTextureUnitStates();

  if (num_textures > 0 && alpha_rejection > 0 && transparency_casts_shadows) {
    auto caster_material = Ogre::MaterialManager::getSingleton().getByName("PSSM/shadow_caster");
    auto new_caster = caster_material->clone("PSSM/shadow_caster" + std::to_string(material_list.size()));
    material->getTechnique(0)->setShadowCasterMaterial(new_caster);

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
  } else {
    auto caster_material = Ogre::MaterialManager::getSingleton().getByName("PSSM/NoAlpha/shadow_caster");
    auto new_caster = caster_material->clone("PSSM/NoAlpha/shadow_caster" + std::to_string(material_list.size()));
    material->getTechnique(0)->setShadowCasterMaterial(new_caster);
  }
}

//----------------------------------------------------------------------------------------------------------------------
inline void UpdatePbrParams(Ogre::MaterialPtr material) {
  std::string material_name = material->getName();
  bool registered = false;
  static std::vector<std::string> material_list;

  if (std::find(material_list.begin(), material_list.end(), material_name) != material_list.end()) {
    registered = true;
  } else {
    material_list.push_back(material_name);
  }

  const int light_count = 5;

  if (material->getTechnique(0)->getPass(0)->hasVertexProgram()) {
    auto vert_params = material->getTechnique(0)->getPass(0)->getVertexProgramParameters();

    auto &constants = vert_params->getConstantDefinitions();
    if (constants.map.count("uMVPMatrix") == 0) {
      return;
    }

    AddGpuConstParameterAuto(vert_params, "uMVPMatrix", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
    AddGpuConstParameterAuto(vert_params, "uModelMatrix", Ogre::GpuProgramParameters::ACT_WORLD_MATRIX);
    AddGpuConstParameterAuto(vert_params, "uCameraPosition", Ogre::GpuProgramParameters::ACT_CAMERA_POSITION);

    if (constants.map.count("fadeRange") > 0)
      AddGpuConstParameter(vert_params, "fadeRange", 1.0f / 200.0f);

    if (constants.map.count("uTime") > 0)
      vert_params->setNamedConstantFromTime("uTime", 1);
  }

  if (material->getTechnique(0)->getPass(0)->hasFragmentProgram()) {
    auto frag_params = material->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
    auto pass = material->getTechnique(0)->getPass(0);

    AddGpuConstParameterAuto(frag_params,
                             "uAlphaRejection",
                             Ogre::GpuProgramParameters::ACT_SURFACE_ALPHA_REJECTION_VALUE);
    AddGpuConstParameterAuto(frag_params,
                             "uSurfaceAmbientColour",
                             Ogre::GpuProgramParameters::ACT_SURFACE_AMBIENT_COLOUR);
    AddGpuConstParameterAuto(frag_params,
                             "uSurfaceDiffuseColour",
                             Ogre::GpuProgramParameters::ACT_SURFACE_DIFFUSE_COLOUR);
    AddGpuConstParameterAuto(frag_params,
                             "uSurfaceSpecularColour",
                             Ogre::GpuProgramParameters::ACT_SURFACE_SPECULAR_COLOUR);
    AddGpuConstParameterAuto(frag_params, "uSurfaceShininessColour", Ogre::GpuProgramParameters::ACT_SURFACE_SHININESS);
    AddGpuConstParameterAuto(frag_params,
                             "uSurfaceEmissiveColour",
                             Ogre::GpuProgramParameters::ACT_SURFACE_EMISSIVE_COLOUR);
    AddGpuConstParameterAuto(frag_params, "uAmbientLightColour", Ogre::GpuProgramParameters::ACT_AMBIENT_LIGHT_COLOUR);
    AddGpuConstParameterAuto(frag_params, "uLightCount", Ogre::GpuProgramParameters::ACT_LIGHT_COUNT);
    AddGpuConstParameterAuto(frag_params,
                             "uLightPositionArray",
                             Ogre::GpuProgramParameters::ACT_LIGHT_POSITION_ARRAY,
                             light_count);
    AddGpuConstParameterAuto(frag_params,
                             "uLightDirectionArray",
                             Ogre::GpuProgramParameters::ACT_LIGHT_DIRECTION_ARRAY,
                             light_count);
    AddGpuConstParameterAuto(frag_params,
                             "uLightDiffuseScaledColourArray",
                             Ogre::GpuProgramParameters::ACT_LIGHT_DIFFUSE_COLOUR_POWER_SCALED_ARRAY,
                             light_count);
    AddGpuConstParameterAuto(frag_params,
                             "uLightAttenuationArray",
                             Ogre::GpuProgramParameters::ACT_LIGHT_ATTENUATION_ARRAY,
                             light_count);
    AddGpuConstParameterAuto(frag_params,
                             "uLightSpotParamsArray",
                             Ogre::GpuProgramParameters::ACT_SPOTLIGHT_PARAMS_ARRAY,
                             light_count);
    AddGpuConstParameterAuto(frag_params, "uFogColour", Ogre::GpuProgramParameters::ACT_FOG_COLOUR);
    AddGpuConstParameterAuto(frag_params, "uFogParams", Ogre::GpuProgramParameters::ACT_FOG_PARAMS);
    AddGpuConstParameterAuto(frag_params, "uCameraPosition", Ogre::GpuProgramParameters::ACT_CAMERA_POSITION);
  }
}

//----------------------------------------------------------------------------------------------------------------------
inline void UpdatePbrShadowReceiver(Ogre::MaterialPtr material) {
  std::string material_name = material->getName();
  bool registered = false;
  const int pssm_split_count = 3;
  const int light_count = 5;
  const int shadows = pssm_split_count * light_count;
  static std::vector<std::string> shadowed_list;

  if (!material->getReceiveShadows()) {
    return;
  }

  if (std::find(shadowed_list.begin(), shadowed_list.end(), material_name) != shadowed_list.end()) {
    registered = true;
  } else {
    shadowed_list.push_back(material_name);
  }

  auto *pssm =
      dynamic_cast<Ogre::PSSMShadowCameraSetup *>(Ogre::Root::getSingleton().getSceneManager("Default")->getShadowCameraSetup().get());

  if (material->getTechnique(0)->getPass(0)->hasVertexProgram()) {
    auto vert_params = material->getTechnique(0)->getPass(0)->getVertexProgramParameters();

    auto &constants = vert_params->getConstantDefinitions();
    if (constants.map.count("uTexWorldViewProjMatrixArray") == 0) {
      return;
    }
    AddGpuConstParameterAuto(vert_params, "uLightCount", Ogre::GpuProgramParameters::ACT_LIGHT_COUNT);
    AddGpuConstParameterAuto(vert_params,
                             "uTexWorldViewProjMatrixArray",
                             Ogre::GpuProgramParameters::ACT_TEXTURE_WORLDVIEWPROJ_MATRIX_ARRAY,
                             pssm_split_count);
    AddGpuConstParameterAuto(vert_params,
                             "uLightCastsShadowsArray",
                             Ogre::GpuProgramParameters::ACT_LIGHT_CASTS_SHADOWS_ARRAY,
                             light_count);
  }

  if (material->getTechnique(0)->getPass(0)->hasFragmentProgram()) {
    auto frag_params = material->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
    auto pass = material->getTechnique(0)->getPass(0);

    uint numTextures = 3;
    Ogre::Vector4 splitPoints;
    const Ogre::PSSMShadowCameraSetup::SplitPointList &splitPointList = pssm->getSplitPoints();
    for (int j = 1; j < numTextures; ++j) {
      splitPoints[j - 1] = splitPointList[j];
    }
    splitPoints[numTextures - 1] = Ogre::Root::getSingleton().getSceneManager("Default")->getShadowFarDistance();

    const int light_count = 5;
    auto &constants = frag_params->getConstantDefinitions();
    if (constants.map.count("pssmSplitPoints") != 0) {
      frag_params->setNamedConstant("pssmSplitPoints", splitPoints);
      AddGpuConstParameterAuto(frag_params, "uShadowColour", Ogre::GpuProgramParameters::ACT_SHADOW_COLOUR);
      AddGpuConstParameterAuto(frag_params, "uLightCastsShadowsArray",
                               Ogre::GpuProgramParameters::ACT_LIGHT_CASTS_SHADOWS_ARRAY,
                               light_count);

      int texture_count = pass->getNumTextureUnitStates();

      if (!registered) {
        for (int k = 0; k < 3; k++) {
          Ogre::TextureUnitState *tu = pass->createTextureUnitState();
          tu->setContentType(Ogre::TextureUnitState::CONTENT_SHADOW);
          tu->setTextureAddressingMode(Ogre::TextureUnitState::TAM_BORDER);
          tu->setTextureBorderColour(Ogre::ColourValue::White);
          tu->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_POINT);
          frag_params->setNamedConstant("shadowMap" + std::to_string(k), texture_count + k);
        }
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
inline void UpdatePbrIbl(Ogre::MaterialPtr material) {
  std::string material_name = material->getName();
  bool registered = false;
  static std::vector<std::string> material_list;

  if (std::find(material_list.begin(), material_list.end(), material_name) != material_list.end()) {
    registered = true;
  } else {
    material_list.push_back(material_name);
  }

  auto ibl_texture = material->getTechnique(0)->getPass(0)->getTextureUnitState("IBL_Specular");
  auto ibl_texture2 = material->getTechnique(0)->getPass(0)->getTextureUnitState("IBL_Diffuse");
  const bool realtime_cubemap = true;
  if (realtime_cubemap) {
    if (ibl_texture) {
      ibl_texture->setTexture(Ogre::TextureManager::getSingleton().getByName("dyncubemap",
                                                                             Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME));

    }
    if (ibl_texture2) {
      ibl_texture2->setTexture(Ogre::TextureManager::getSingleton().getByName("dyncubemap",
                                                                              Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME));

    }
  } else {
    auto skybox = Ogre::MaterialManager::getSingleton().getByName("SkyBox");
    if (!skybox)
      return;
    auto cubemap = skybox->getTechnique(0)->getPass(0)->getTextureUnitState("CubeMap");
    if (!cubemap)
      return;
    if (ibl_texture)
      ibl_texture->setTextureName(cubemap->getName());
    if (ibl_texture2)
      ibl_texture2->setTextureName(cubemap->getName());
  }
}

//----------------------------------------------------------------------------------------------------------------------
inline void UpdatePbrParams(const std::string &material) {
  UpdatePbrParams(Ogre::MaterialManager::getSingleton().getByName(material));
}

//----------------------------------------------------------------------------------------------------------------------
inline void UpdatePbrIbl(const std::string &material) {
  UpdatePbrIbl(Ogre::MaterialManager::getSingleton().getByName(material));
}

//----------------------------------------------------------------------------------------------------------------------
inline void UpdatePbrShadowReceiver(const std::string &material) {
  UpdatePbrShadowReceiver(Ogre::MaterialManager::getSingleton().getByName(material));
}

//----------------------------------------------------------------------------------------------------------------------
inline void UpdatePbrShadowCaster(const std::string &material) {
  UpdatePbrShadowCaster(Ogre::MaterialManager::getSingleton().getByName(material));
}

//----------------------------------------------------------------------------------------------------------------------
inline bool HasNoTangentsAndCanGenerate(Ogre::VertexDeclaration *vertex_declaration) {
  bool hasTangents = false;
  bool hasUVs = false;
  auto &elementList = vertex_declaration->getElements();
  auto iter = elementList.begin();
  auto end = elementList.end();

  while (iter != end && !hasTangents) {
    const Ogre::VertexElement &vertexElem = *iter;
    if (vertexElem.getSemantic() == Ogre::VES_TANGENT)
      hasTangents = true;
    if (vertexElem.getSemantic() == Ogre::VES_TEXTURE_COORDINATES)
      hasUVs = true;

    ++iter;
  }

  return !hasTangents && hasUVs;
}

//----------------------------------------------------------------------------------------------------------------------
inline void EnsureHasTangents(Ogre::MeshPtr mesh_ptr) {
  bool generateTangents = false;
  if (mesh_ptr->sharedVertexData) {
    Ogre::VertexDeclaration *vertexDecl = mesh_ptr->sharedVertexData->vertexDeclaration;
    generateTangents |= HasNoTangentsAndCanGenerate(vertexDecl);
  }

  for (unsigned i = 0; i < mesh_ptr->getNumSubMeshes(); ++i) {
    Ogre::SubMesh *subMesh = mesh_ptr->getSubMesh(i);
    if (subMesh->vertexData) {
      Ogre::VertexDeclaration *vertexDecl = subMesh->vertexData->vertexDeclaration;
      generateTangents |= HasNoTangentsAndCanGenerate(vertexDecl);
    }
  }

  if (generateTangents) {
    mesh_ptr->buildTangentVectors();
  }
}
}