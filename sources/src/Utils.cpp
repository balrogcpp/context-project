/*
MIT License

Copyright (c) 2020 Andrey Vasiliev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "pcheader.h"
#include "Utils.h"
#include "Render.h"

using namespace Ogre;
using namespace xio;

namespace xio {
//----------------------------------------------------------------------------------------------------------------------
void UpdatePbrShadowCaster(MaterialPtr material) {
  std::string material_name = material->getName();
  static std::vector<std::string> material_list;

  if (std::find(material_list.begin(), material_list.end(), material_name) != material_list.end()) {
    return;
  } else {
    material_list.push_back(material_name);
  }

  auto *pass = material->getTechnique(0)->getPass(0);
  int alpha_rejection = static_cast<int>(pass->getAlphaRejectValue());

  if (material->getTechnique(0)->getPass(0)->getNumTextureUnitStates() > 0 && alpha_rejection > 0) {
    auto caster_material = MaterialManager::getSingleton().getByName("PSSM/shadow_caster");
    auto new_caster = caster_material->clone("PSSM/shadow_caster" + std::to_string(material_list.size()));
    material->getTechnique(0)->setShadowCasterMaterial(new_caster);

    auto texture_albedo = pass->getTextureUnitState("Albedo");
    if (texture_albedo) {
      std::string texture_name = pass->getTextureUnitState("Albedo")->getTextureName();

      auto *texPtr3 = new_caster->getTechnique(0)->getPass(0)->getTextureUnitState("BaseColor");

      if (texPtr3) {
        texPtr3->setContentType(TextureUnitState::CONTENT_NAMED);
        texPtr3->setTextureAddressingMode(TextureAddressingMode::TAM_CLAMP);
        texPtr3->setTextureFiltering(FO_LINEAR, FO_LINEAR, FO_POINT);
        texPtr3->setTextureName(texture_name);
      }
    }
  } else {
    auto caster_material = MaterialManager::getSingleton().getByName("PSSM/NoAlpha/shadow_caster");
    auto new_caster = caster_material->clone("PSSM/NoAlpha/shadow_caster" + std::to_string(material_list.size()));
    material->getTechnique(0)->setShadowCasterMaterial(new_caster);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void UpdatePbrParams(MaterialPtr material) {
  std::string material_name = material->getName();
  bool registered = false;
  static std::vector<std::string> material_list;

  if (std::find(material_list.begin(), material_list.end(), material_name) != material_list.end()) {
    registered = true;
//    return;
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

    AddGpuConstParameterAuto(vert_params, "uMVPMatrix", GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
    AddGpuConstParameterAuto(vert_params, "uModelMatrix", GpuProgramParameters::ACT_WORLD_MATRIX);
    AddGpuConstParameterAuto(vert_params, "uLightCount", GpuProgramParameters::ACT_LIGHT_COUNT);

    AddGpuConstParameter(vert_params, "fadeRange", 1 / (100.0f * 2.0f));

    if (constants.map.count("uTime") > 0) {
      vert_params->setNamedConstantFromTime("uTime", 1);
    }
  }

  if (material->getTechnique(0)->getPass(0)->hasFragmentProgram()) {
    auto frag_params = material->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
    auto pass = material->getTechnique(0)->getPass(0);

    AddGpuConstParameterAuto(frag_params,
                             "uAlphaRejection",
                             GpuProgramParameters::ACT_SURFACE_ALPHA_REJECTION_VALUE);
    AddGpuConstParameterAuto(frag_params,
                             "uSurfaceAmbientColour",
                             GpuProgramParameters::ACT_SURFACE_AMBIENT_COLOUR);
    AddGpuConstParameterAuto(frag_params,
                             "uSurfaceDiffuseColour",
                             GpuProgramParameters::ACT_SURFACE_DIFFUSE_COLOUR);
    AddGpuConstParameterAuto(frag_params,
                             "uSurfaceSpecularColour",
                             GpuProgramParameters::ACT_SURFACE_SPECULAR_COLOUR);
    AddGpuConstParameterAuto(frag_params, "uSurfaceShininessColour", GpuProgramParameters::ACT_SURFACE_SHININESS);
    AddGpuConstParameterAuto(frag_params,
                             "uSurfaceEmissiveColour",
                             GpuProgramParameters::ACT_SURFACE_EMISSIVE_COLOUR);
    AddGpuConstParameterAuto(frag_params, "uAmbientLightColour", GpuProgramParameters::ACT_AMBIENT_LIGHT_COLOUR);
    AddGpuConstParameterAuto(frag_params, "uLightCount", GpuProgramParameters::ACT_LIGHT_COUNT);
    AddGpuConstParameterAuto(frag_params,
                             "uLightPositionArray",
                             GpuProgramParameters::ACT_LIGHT_POSITION_ARRAY,
                             light_count);
    AddGpuConstParameterAuto(frag_params,
                             "uLightDirectionArray",
                             GpuProgramParameters::ACT_LIGHT_DIRECTION_ARRAY,
                             light_count);
    AddGpuConstParameterAuto(frag_params,
                             "uLightDiffuseScaledColourArray",
                             GpuProgramParameters::ACT_LIGHT_DIFFUSE_COLOUR_POWER_SCALED_ARRAY,
                             light_count);
    AddGpuConstParameterAuto(frag_params,
                             "uLightAttenuationArray",
                             GpuProgramParameters::ACT_LIGHT_ATTENUATION_ARRAY,
                             light_count);
    AddGpuConstParameterAuto(frag_params,
                             "uLightSpotParamsArray",
                             GpuProgramParameters::ACT_SPOTLIGHT_PARAMS_ARRAY,
                             light_count);
    AddGpuConstParameterAuto(frag_params, "uFogColour", GpuProgramParameters::ACT_FOG_COLOUR);
    AddGpuConstParameterAuto(frag_params, "uFogParams", GpuProgramParameters::ACT_FOG_PARAMS);
    AddGpuConstParameterAuto(frag_params, "uCameraPosition", GpuProgramParameters::ACT_CAMERA_POSITION);

    auto ibl_texture = pass->getTextureUnitState("IBL_Specular");
    const bool realtime_cubemap = false;
//    if (ibl_texture) {
//      if (realtime_cubemap) {
//        ibl_texture->setTexture(CubeMapCamera::Instance().GetDyncubemap());
//      } else {
//        std::string skybox_cubemap =
//            MaterialManager::getSingleton().getByName("SkyBox")->getTechnique(0)->getPass(0)->getTextureUnitState(
//                "CubeMap")->getTextureName();
//        ibl_texture->setTextureName(skybox_cubemap);
//      }
//    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void UpdatePbrShadowReceiver(MaterialPtr material) {
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
//    return;
  } else {
    shadowed_list.push_back(material_name);
  }

  auto *pssm =
      dynamic_cast<PSSMShadowCameraSetup *>(Ogre::Root::getSingleton().getSceneManager("Default")->getShadowCameraSetup().get());

  if (material->getTechnique(0)->getPass(0)->hasVertexProgram()) {
    auto vert_params = material->getTechnique(0)->getPass(0)->getVertexProgramParameters();

    auto &constants = vert_params->getConstantDefinitions();
    if (constants.map.count("uTexWorldViewProjMatrixArray") == 0) {
      return;
    }

    AddGpuConstParameterAuto(vert_params,
                             "uTexWorldViewProjMatrixArray",
                             GpuProgramParameters::ACT_TEXTURE_WORLDVIEWPROJ_MATRIX_ARRAY,
                             pssm_split_count);
    AddGpuConstParameterAuto(vert_params,
                             "uLightCastsShadowsArray",
                             GpuProgramParameters::ACT_LIGHT_CASTS_SHADOWS_ARRAY,
                             light_count);
  }

  if (material->getTechnique(0)->getPass(0)->hasFragmentProgram()) {
    auto frag_params = material->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
    auto pass = material->getTechnique(0)->getPass(0);

    uint numTextures = 3;
    Vector4 splitPoints;
    const PSSMShadowCameraSetup::SplitPointList &splitPointList = pssm->getSplitPoints();
    for (int j = 1; j < numTextures; ++j) {
      splitPoints[j - 1] = splitPointList[j];
    }
    splitPoints[numTextures - 1] = Ogre::Root::getSingleton().getSceneManager("Default")->getShadowFarDistance();

    const int light_count = 5;
    auto &constants = frag_params->getConstantDefinitions();
    if (constants.map.count("pssmSplitPoints") != 0) {
      frag_params->setNamedConstant("pssmSplitPoints", splitPoints);
      AddGpuConstParameterAuto(frag_params, "uShadowColour", GpuProgramParameters::ACT_SHADOW_COLOUR);
      AddGpuConstParameterAuto(frag_params, "uLightCastsShadowsArray",
                               GpuProgramParameters::ACT_LIGHT_CASTS_SHADOWS_ARRAY,
                               light_count);

      int texture_count = pass->getNumTextureUnitStates();

      if (!registered) {
        for (int k = 0; k < 3; k++) {
          TextureUnitState *tu = pass->createTextureUnitState();
          tu->setContentType(TextureUnitState::CONTENT_SHADOW);
          tu->setTextureAddressingMode(TextureUnitState::TAM_BORDER);
          tu->setTextureBorderColour(ColourValue::White);
          tu->setTextureFiltering(FO_LINEAR, FO_LINEAR, FO_POINT);
          frag_params->setNamedConstant("shadowMap" + std::to_string(k), texture_count + k);
//            frag_params->setNamedAutoConstant("inverseShadowmapSize" + std::to_string(k),
//                                    GpuProgramParameters::ACT_INVERSE_TEXTURE_SIZE,
//                                    texture_count + k);
        }
      }
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void UpdateForestParams(MaterialPtr material) {
}
//----------------------------------------------------------------------------------------------------------------------
void UpdatePbrParams(const std::string &material) {
  UpdatePbrParams(MaterialManager::getSingleton().getByName(material));
}
//----------------------------------------------------------------------------------------------------------------------
void UpdatePbrShadowReceiver(const std::string &material) {
  UpdatePbrShadowReceiver(MaterialManager::getSingleton().getByName(material));
}
//----------------------------------------------------------------------------------------------------------------------
void UpdatePbrShadowCaster(const std::string &material) {
  UpdatePbrShadowCaster(MaterialManager::getSingleton().getByName(material));
}
//----------------------------------------------------------------------------------------------------------------------
void UpdateForestParams(const std::string &material) {
  UpdateForestParams(MaterialManager::getSingleton().getByName(material));
}
//----------------------------------------------------------------------------------------------------------------------
void EnsureHasTangents(Ogre::MeshPtr mesh_ptr) {
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
//----------------------------------------------------------------------------------------------------------------------
bool HasNoTangentsAndCanGenerate(Ogre::VertexDeclaration *vertex_declaration) {
  bool hasTangents = false;
  bool hasUVs = false;
  const Ogre::VertexDeclaration::VertexElementList &elementList = vertex_declaration->getElements();
  auto itor = elementList.begin();
  auto end = elementList.end();

  while (itor != end && !hasTangents) {
    const Ogre::VertexElement &vertexElem = *itor;
    if (vertexElem.getSemantic() == Ogre::VES_TANGENT)
      hasTangents = true;
    if (vertexElem.getSemantic() == Ogre::VES_TEXTURE_COORDINATES)
      hasUVs = true;

    ++itor;
  }

  return !hasTangents && hasUVs;
}
}