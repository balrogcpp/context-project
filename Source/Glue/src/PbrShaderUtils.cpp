// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "PbrShaderUtils.h"
#include "ComponentLocator.h"

using namespace std;

namespace Glue {

void Pbr::UpdatePbrShadowCaster(const Ogre::MaterialPtr &material) {
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

  } else {
    string caster_name = "PSSM/shadow_caster/" + material_name;
    Ogre::MaterialPtr new_caster = Ogre::MaterialManager::getSingleton().getByName(caster_name);

    if (!new_caster) {
      auto caster_material = Ogre::MaterialManager::getSingleton().getByName("PSSM/shadow_caster");

      new_caster = caster_material->clone(caster_name);

      auto *new_pass = new_caster->getTechnique(0)->getPass(0);
      new_pass->setCullingMode(pass->getCullingMode());
      new_pass->setManualCullingMode(pass->getManualCullingMode());
    }

    material->getTechnique(0)->setShadowCasterMaterial(new_caster);
  }
}

static std::vector<Ogre::GpuProgramParametersSharedPtr> gpu_fp_params_;
static std::vector<Ogre::GpuProgramParametersSharedPtr> gpu_vp_params_;
static Ogre::Matrix4 mvp_;
static Ogre::Matrix4 mvp_prev_;
static Ogre::Camera *camera_;

void Pbr::Cleanup() {
  gpu_fp_params_.clear();
  gpu_fp_params_.shrink_to_fit();
  gpu_vp_params_.clear();
  gpu_vp_params_.shrink_to_fit();
}

void Pbr::Update(float time) {
  camera_ = Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default");
  mvp_prev_ = mvp_;
  mvp_ = camera_->getProjectionMatrixWithRSDepth() * camera_->getViewMatrix();

  for_each(gpu_fp_params_.begin(), gpu_fp_params_.end(),
           [time](auto &it) { it->setNamedConstant("uFrameTime", time); });

  for_each(gpu_vp_params_.begin(), gpu_vp_params_.end(),
           [](auto &it) { it->setNamedConstant("cWorldViewProjPrev", mvp_prev_); });
}

void Pbr::UpdatePbrParams(const Ogre::MaterialPtr &material) {
  using GPU = Ogre::GpuProgramParameters::AutoConstantType;
  //  const int MAX_SHADOWS = OGRE_MAX_SIMULTANEOUS_SHADOW_TEXTURES;
  const int MAX_LIGHTS = OGRE_MAX_SIMULTANEOUS_LIGHTS;

  for (int i = 0; i < material->getNumTechniques(); i++) {
    if (!material->getTechnique(i)->getPass(0)->hasVertexProgram() ||
        !material->getTechnique(i)->getPass(0)->hasFragmentProgram()) {
      return;
    }

    auto vert_params = material->getTechnique(i)->getPass(0)->getVertexProgramParameters();
    vert_params->setIgnoreMissingParams(true);

    if (vert_params->getConstantDefinitions().map.count("uMVPMatrix") == 0) {
      return;
    }

    vert_params->setNamedAutoConstant("uMVPMatrix", GPU::ACT_WORLDVIEWPROJ_MATRIX);
    vert_params->setNamedAutoConstant("uModelMatrix", GPU::ACT_WORLD_MATRIX);
    vert_params->setNamedAutoConstant("uCameraPosition", GPU::ACT_CAMERA_POSITION);

    vert_params->setNamedConstant("uFadeRange", 50.0f);
    vert_params->setNamedConstant("uWindRange", 50.0f);
    vert_params->setNamedConstantFromTime("uTime", 1.0f);

    auto frag_params = material->getTechnique(i)->getPass(0)->getFragmentProgramParameters();
    frag_params->setIgnoreMissingParams(true);

    frag_params->setNamedAutoConstant("uAlphaRejection", GPU::ACT_SURFACE_ALPHA_REJECTION_VALUE);
    frag_params->setNamedAutoConstant("uSurfaceAmbientColour", GPU::ACT_SURFACE_AMBIENT_COLOUR);
    frag_params->setNamedAutoConstant("uSurfaceDiffuseColour", GPU::ACT_SURFACE_DIFFUSE_COLOUR);
    frag_params->setNamedAutoConstant("uSurfaceSpecularColour", GPU::ACT_SURFACE_SPECULAR_COLOUR);
    frag_params->setNamedAutoConstant("uSurfaceShininessColour", GPU::ACT_SURFACE_SHININESS);
    frag_params->setNamedAutoConstant("uSurfaceEmissiveColour", GPU::ACT_SURFACE_EMISSIVE_COLOUR);
    frag_params->setNamedAutoConstant("uAmbientLightColour", GPU::ACT_AMBIENT_LIGHT_COLOUR);
    frag_params->setNamedAutoConstant("uLightCount", GPU::ACT_LIGHT_COUNT);
    frag_params->setNamedAutoConstant("uLightPositionArray", GPU::ACT_LIGHT_POSITION_ARRAY, MAX_LIGHTS);
    frag_params->setNamedAutoConstant("uLightDirectionArray", GPU::ACT_LIGHT_DIRECTION_ARRAY, MAX_LIGHTS);
    frag_params->setNamedAutoConstant("uLightDiffuseScaledColourArray",
                                      GPU::ACT_LIGHT_DIFFUSE_COLOUR_POWER_SCALED_ARRAY, MAX_LIGHTS);
    frag_params->setNamedAutoConstant("uLightAttenuationArray", GPU::ACT_LIGHT_ATTENUATION_ARRAY, MAX_LIGHTS);
    frag_params->setNamedAutoConstant("uLightSpotParamsArray", GPU::ACT_SPOTLIGHT_PARAMS_ARRAY, MAX_LIGHTS);
    frag_params->setNamedAutoConstant("uFogColour", GPU::ACT_FOG_COLOUR);
    frag_params->setNamedAutoConstant("uFogParams", GPU::ACT_FOG_PARAMS);
    frag_params->setNamedAutoConstant("uCameraPosition", GPU::ACT_CAMERA_POSITION);
    frag_params->setNamedAutoConstant("cNearClipDistance", GPU::ACT_NEAR_CLIP_DISTANCE);
    frag_params->setNamedAutoConstant("cFarClipDistance", GPU::ACT_FAR_CLIP_DISTANCE);

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID || OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
    frag_params->setNamedConstant("uLOD", 1.0f);
#else
    frag_params->setNamedConstant("uLOD", 0.0f);
#endif

    gpu_vp_params_.push_back(vert_params);
    gpu_fp_params_.push_back(frag_params);
  }
}

void Pbr::UpdatePbrShadowReceiver(const Ogre::MaterialPtr &material) {
  if (Ogre::Root::getSingleton().getSceneManager("Default")->getShadowTechnique() == Ogre::SHADOWTYPE_NONE) return;

  using GPU = Ogre::GpuProgramParameters::AutoConstantType;
  const int MAX_SHADOWS = OGRE_MAX_SIMULTANEOUS_SHADOW_TEXTURES;

  for (int i = 0; i < material->getNumTechniques(); i++) {
    if (!material->getReceiveShadows() || !material->getTechnique(i)->getPass(0)->hasVertexProgram() ||
        !material->getTechnique(i)->getPass(0)->hasFragmentProgram()) {
      return;
    }

    auto *scene = Ogre::Root::getSingleton().getSceneManager("Default");
    auto *pssm = dynamic_cast<Ogre::PSSMShadowCameraSetup *>(scene->getShadowCameraSetup().get());

    auto *technique = material->getTechnique(i);
    auto *pass = technique->getPass(0);

    auto vp = pass->getVertexProgramParameters();
    vp->setIgnoreMissingParams(true);

    if (vp->getConstantDefinitions().map.count("uTexWorldViewProjMatrixArray") == 0) {
      return;
    }

    vp->setNamedConstant("uShadowTextureCount", MAX_SHADOWS);
    vp->setNamedAutoConstant("uTexWorldViewProjMatrixArray", GPU::ACT_TEXTURE_WORLDVIEWPROJ_MATRIX_ARRAY, MAX_SHADOWS);

    auto fp = material->getTechnique(i)->getPass(0)->getFragmentProgramParameters();
    fp->setIgnoreMissingParams(true);

    Ogre::Vector4 splitPoints;
    const Ogre::PSSMShadowCameraSetup::SplitPointList &splitPointList = pssm->getSplitPoints();

    for (unsigned j = 0; j < pssm->getSplitCount(); j++) {
      splitPoints[j] = splitPointList[j + 1];
    }

    splitPoints.w = scene->getShadowFarDistance();

    if (fp->getConstantDefinitions().map.count("pssmSplitPoints") == 0) {
      return;
    }

    fp->setNamedConstant("pssmSplitPoints", splitPoints);
    fp->setNamedAutoConstant("uShadowColour", GPU::ACT_SHADOW_COLOUR);
    fp->setNamedAutoConstant("uLightCastsShadowsArray", GPU::ACT_LIGHT_CASTS_SHADOWS_ARRAY, MAX_SHADOWS);
    fp->setNamedConstant("uShadowFilterSize", 4.0f);
    fp->setNamedConstant("uShadowFilterIterations", 16);
    // fp->setNamedConstant("uShadowDepthOffset", 0.0f);

    int texture_count = pass->getNumTextureUnitStates();

    for (int j = 0; j < MAX_SHADOWS; j++) {
      if (pass->getTextureUnitState("shadowMap" + to_string(j))) {
        continue;
      }

      Ogre::TextureUnitState *tu = pass->createTextureUnitState();
      tu->setName("shadowMap" + to_string(j));
      tu->setContentType(Ogre::TextureUnitState::CONTENT_SHADOW);
      tu->setTextureAddressingMode(Ogre::TextureUnitState::TAM_BORDER);
      tu->setTextureBorderColour(Ogre::ColourValue::White);
      // tu->setTextureFiltering(Ogre::TFO_NONE);
      fp->setNamedConstant("shadowMap" + to_string(j), texture_count + j);
      fp->setNamedAutoConstant("shadowTexel" + to_string(j), GPU::ACT_INVERSE_TEXTURE_SIZE, texture_count + j);
    }
  }
}

void Pbr::UpdatePbrIbl(const Ogre::MaterialPtr &material, bool active) {
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

void Pbr::UpdatePbrParams(const string &material) {
  auto material_ptr = Ogre::MaterialManager::getSingleton().getByName(material);
  if (material_ptr) {
    UpdatePbrParams(material_ptr);
  }
}

void Pbr::UpdatePbrIbl(const string &material, bool realtime) {
  auto material_ptr = Ogre::MaterialManager::getSingleton().getByName(material);
  UpdatePbrIbl(material_ptr, realtime);
}

void Pbr::UpdatePbrShadowReceiver(const string &material) {
  auto material_ptr = Ogre::MaterialManager::getSingleton().getByName(material);
  UpdatePbrShadowReceiver(material_ptr);
}

void Pbr::UpdatePbrShadowCaster(const string &material) {
  auto material_ptr = Ogre::MaterialManager::getSingleton().getByName(material);
  UpdatePbrShadowCaster(material_ptr);
}

}  // namespace Glue
