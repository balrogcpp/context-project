// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "PBRUtils.h"
#include "Engine.h"

using namespace std;
using namespace Ogre;

namespace Glue {

void PBR::FixTransparentShadowCaster(const MaterialPtr &material) {
  auto *pass = material->getTechnique(0)->getPass(0);
  int alpha_rejection = static_cast<int>(pass->getAlphaRejectValue());
  bool transparency_casts_shadows = material->getTransparencyCastsShadows();
  int num_textures = pass->getNumTextureUnitStates();
  string material_name = material->getName();

  if (num_textures > 0 && alpha_rejection > 0 && transparency_casts_shadows) {
    auto caster_material = MaterialManager::getSingleton().getByName("PSSM/shadow_caster_alpha");
    string caster_name = "PSSM/shadow_caster_alpha/" + material_name;
    MaterialPtr new_caster = MaterialManager::getSingleton().getByName(caster_name);

    if (!new_caster) {
      new_caster = caster_material->clone(caster_name);

      auto texture_albedo = pass->getTextureUnitState("Albedo");

      if (texture_albedo) {
        string texture_name = pass->getTextureUnitState("Albedo")->getTextureName();

        auto *texPtr3 = new_caster->getTechnique(0)->getPass(0)->getTextureUnitState("BaseColor");

        if (texPtr3) {
          texPtr3->setContentType(TextureUnitState::CONTENT_NAMED);
          texPtr3->setTextureName(texture_name);
        }
      }
    }

    auto *new_pass = new_caster->getTechnique(0)->getPass(0);
    new_pass->setCullingMode(pass->getCullingMode());
    new_pass->setManualCullingMode(pass->getManualCullingMode());
    new_pass->setAlphaRejectValue(alpha_rejection);

    material->getTechnique(0)->setShadowCasterMaterial(new_caster);
  }
}

void PBR::FixTransparentShadowCaster(const string &material) {
  auto material_ptr = MaterialManager::getSingleton().getByName(material);
  FixTransparentShadowCaster(material_ptr);
}

void PBR::UpdatePbrParams(const MaterialPtr &material) {
  //using GPU = GpuProgramParameters::AutoConstantType;
  //const int MAX_SHADOWS = OGRE_MAX_SIMULTANEOUS_SHADOW_TEXTURES;
  //const int MAX_LIGHTS = OGRE_MAX_SIMULTANEOUS_LIGHTS;

  //for (int i = 0; i < material->getNumTechniques(); i++) {
  //  if (!material->getTechnique(i)->getPass(0)->hasVertexProgram() ||
  //      !material->getTechnique(i)->getPass(0)->hasFragmentProgram()) {
  //    return;
  //  }

  //  gpu_vp_params_.push_back(material->getTechnique(i)->getPass(0)->getVertexProgramParameters());
  //  gpu_fp_params_.push_back(material->getTechnique(i)->getPass(0)->getFragmentProgramParameters());
  //}
}

void PBR::UpdatePbrParams(const string &material) {
  //auto material_ptr = MaterialManager::getSingleton().getByName(material);
  //if (material_ptr) {
  //  UpdatePbrParams(material_ptr);
  //}
}

void PBR::UpdatePbrIbl(const MaterialPtr &material, bool active) {
  //auto ibl_texture = material->getTechnique(0)->getPass(0)->getTextureUnitState("IBL_Specular");

  //if (active) {
  //  auto cubemap = TextureManager::getSingleton().getByName("dyncubemap", RGN_AUTODETECT);
  //  if (ibl_texture) {
  //    ibl_texture->setTexture(cubemap);
  //  }
  //} else {
  //  auto skybox = MaterialManager::getSingleton().getByName("SkyBox");
  //  if (!skybox) return;
  //  auto cubemap = skybox->getTechnique(0)->getPass(0)->getTextureUnitState("CubeMap");
  //  if (!cubemap) return;

  //  if (ibl_texture) ibl_texture->setTexture(cubemap->_getTexturePtr());
  //}
}

void PBR::UpdatePbrIbl(const string &material, bool realtime) {
  //auto material_ptr = MaterialManager::getSingleton().getByName(material);
  //UpdatePbrIbl(material_ptr, realtime);
}

void PBR::Cleanup() {
  //gpu_fp_params_.clear();
  //gpu_fp_params_.shrink_to_fit();
  //gpu_vp_params_.clear();
  //gpu_vp_params_.shrink_to_fit();
}

void PBR::Update(float time) {
  //static Camera *CameraPtr = Root::getSingleton().getSceneManager("Default")->getCamera("Default");
  //static Matrix4 MVP;
  //static Matrix4 MVPprev;

  //MVPprev = MVP;
  //MVP = CameraPtr->getProjectionMatrixWithRSDepth() * CameraPtr->getViewMatrix();

  //for (auto &it : gpu_vp_params_) it->setNamedConstant("cWorldViewProjPrev", MVPprev);
}

}  // namespace Glue
