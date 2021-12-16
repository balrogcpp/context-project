// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "PBRUtils.h"
#include "Components/ComponentLocator.h"

using namespace std;

namespace Glue {

void PBR::UpdatePbrShadowCaster(const Ogre::MaterialPtr &material) {
  auto *pass = material->getTechnique(0)->getPass(0);
  int alpha_rejection = static_cast<int>(pass->getAlphaRejectValue());
  bool transparency_casts_shadows = material->getTransparencyCastsShadows();
  int num_textures = pass->getNumTextureUnitStates();
  string material_name = material->getName();

  if (num_textures > 0 && alpha_rejection > 0 && transparency_casts_shadows) {
    auto caster_material = Ogre::MaterialManager::getSingleton().getByName("PSSM/shadow_caster_alpha");
    string caster_name = "PSSM/shadow_caster_alpha/" + material_name;
    Ogre::MaterialPtr new_caster = Ogre::MaterialManager::getSingleton().getByName(caster_name);

    if (!new_caster) {
      new_caster = caster_material->clone(caster_name);

      auto texture_albedo = pass->getTextureUnitState("Albedo");

      if (texture_albedo) {
        string texture_name = pass->getTextureUnitState("Albedo")->getTextureName();

        auto *texPtr3 = new_caster->getTechnique(0)->getPass(0)->getTextureUnitState("BaseColor");

        if (texPtr3) {
          texPtr3->setContentType(Ogre::TextureUnitState::CONTENT_NAMED);
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
//  else {
//    string caster_name = "PSSM/shadow_caster/" + material_name;
//    Ogre::MaterialPtr new_caster = Ogre::MaterialManager::getSingleton().getByName(caster_name);
//
//    if (!new_caster) {
//      auto caster_material = Ogre::MaterialManager::getSingleton().getByName("PSSM/shadow_caster");
//
//      new_caster = caster_material->clone(caster_name);
//
//      auto *new_pass = new_caster->getTechnique(0)->getPass(0);
//      new_pass->setCullingMode(pass->getCullingMode());
//      new_pass->setManualCullingMode(pass->getManualCullingMode());
//    }
//
//    material->getTechnique(0)->setShadowCasterMaterial(new_caster);
//  }
}

static std::vector<Ogre::GpuProgramParametersSharedPtr> gpu_fp_params_;
static std::vector<Ogre::GpuProgramParametersSharedPtr> gpu_vp_params_;

void PBR::Cleanup() {
  gpu_fp_params_.clear();
  gpu_fp_params_.shrink_to_fit();
  gpu_vp_params_.clear();
  gpu_vp_params_.shrink_to_fit();
}

void PBR::Update(float time) {
  static Ogre::Camera *camera_ = Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default");
  static Ogre::Matrix4 mvp_;
  static Ogre::Matrix4 mvp_prev_;

  mvp_prev_ = mvp_;
  mvp_ = camera_->getProjectionMatrixWithRSDepth() * camera_->getViewMatrix();

//  for (auto &it : gpu_fp_params_) it->setNamedConstant("uFrameTime", time);

  for (auto &it : gpu_vp_params_) it->setNamedConstant("cWorldViewProjPrev", mvp_prev_);
}  // namespace Glue

void PBR::UpdatePbrParams(const Ogre::MaterialPtr &material) {
  using GPU = Ogre::GpuProgramParameters::AutoConstantType;
  const int MAX_SHADOWS = OGRE_MAX_SIMULTANEOUS_SHADOW_TEXTURES;
  const int MAX_LIGHTS = OGRE_MAX_SIMULTANEOUS_LIGHTS;

  for (int i = 0; i < material->getNumTechniques(); i++) {
    if (!material->getTechnique(i)->getPass(0)->hasVertexProgram() ||
        !material->getTechnique(i)->getPass(0)->hasFragmentProgram()) {
      return;
    }

    gpu_vp_params_.push_back(material->getTechnique(i)->getPass(0)->getVertexProgramParameters());
    gpu_fp_params_.push_back(material->getTechnique(i)->getPass(0)->getFragmentProgramParameters());
  }
}

void PBR::UpdatePbrIbl(const Ogre::MaterialPtr &material, bool active) {
  auto ibl_texture = material->getTechnique(0)->getPass(0)->getTextureUnitState("IBL_Specular");

  if (active) {
    auto cubemap = Ogre::TextureManager::getSingleton().getByName("dyncubemap", Ogre::RGN_AUTODETECT);
    if (ibl_texture) {
      ibl_texture->setTexture(cubemap);
    }
  } else {
    auto skybox = Ogre::MaterialManager::getSingleton().getByName("SkyBox");
    if (!skybox) return;
    auto cubemap = skybox->getTechnique(0)->getPass(0)->getTextureUnitState("CubeMap");
    if (!cubemap) return;

    if (ibl_texture) ibl_texture->setTexture(cubemap->_getTexturePtr());
  }
}

void PBR::UpdatePbrParams(const string &material) {
  auto material_ptr = Ogre::MaterialManager::getSingleton().getByName(material);
  if (material_ptr) {
    UpdatePbrParams(material_ptr);
  }
}

void PBR::UpdatePbrIbl(const string &material, bool realtime) {
  auto material_ptr = Ogre::MaterialManager::getSingleton().getByName(material);
  UpdatePbrIbl(material_ptr, realtime);
}

void PBR::UpdatePbrShadowCaster(const string &material) {
  auto material_ptr = Ogre::MaterialManager::getSingleton().getByName(material);
  UpdatePbrShadowCaster(material_ptr);
}

}  // namespace Glue
