//MIT License
//
//Copyright (c) 2021 Andrew Vasiliev
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

#include "pcheader.h"
#include "PbrShaderUtils.h"
#include "ComponentLocator.h"

using namespace std;

namespace xio {
//----------------------------------------------------------------------------------------------------------------------
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

	material->getTechnique(0)->setShadowCasterMaterial(new_caster);
  } else {
	string caster_name = "PSSM/shadow_caster/" + material_name;
	Ogre::MaterialPtr new_caster = Ogre::MaterialManager::getSingleton().getByName(caster_name);

	if (!new_caster) {
	  auto caster_material = Ogre::MaterialManager::getSingleton().getByName("PSSM/shadow_caster");

	  new_caster = caster_material->clone(caster_name);

	  auto *pass = new_caster->getTechnique(0)->getPass(0);
	  pass->setCullingMode(material->getTechnique(0)->getPass(0)->getCullingMode());
	  pass->setManualCullingMode(material->getTechnique(0)->getPass(0)->getManualCullingMode());
	}

	material->getTechnique(0)->setShadowCasterMaterial(new_caster);
  }
}

static std::vector <Ogre::GpuProgramParametersSharedPtr> gpu_fp_params_;
static std::vector <Ogre::GpuProgramParametersSharedPtr> gpu_vp_params_;
static Ogre::Matrix4 mvp_;
static Ogre::Matrix4 mvp_prev_;
static Ogre::Camera *camera_;

//----------------------------------------------------------------------------------------------------------------------
void Pbr::Cleanup() {
	gpu_fp_params_.clear();
	gpu_fp_params_.shrink_to_fit();
	gpu_vp_params_.clear();
	gpu_vp_params_.shrink_to_fit();
}

//----------------------------------------------------------------------------------------------------------------------
void Pbr::Update(float time) {
  if (GetConf().Get<bool>("compositor_use_motion")) {
	camera_ = Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default");
	mvp_prev_ = mvp_;
	mvp_ = camera_->getProjectionMatrixWithRSDepth()*camera_->getViewMatrix();

	for_each(gpu_fp_params_.begin(),
			 gpu_fp_params_.end(),
			 [time](auto &it) { it->setNamedConstant("uFrameTime", time); });

	for_each(gpu_vp_params_.begin(),
			 gpu_vp_params_.end(),
			 [](auto &it) { it->setNamedConstant("cWorldViewProjPrev", mvp_prev_); });
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Pbr::UpdatePbrParams(const Ogre::MaterialPtr &material) {
  for (int i = 0; i < material->getNumTechniques(); i++) {

	if (!material->getTechnique(i)->getPass(0)->hasVertexProgram()
		|| !material->getTechnique(i)->getPass(0)->hasFragmentProgram()) {
	  return;
	}

	auto vert_params = material->getTechnique(i)->getPass(0)->getVertexProgramParameters();
	vert_params->setIgnoreMissingParams(true);

	if (vert_params->getConstantDefinitions().map.count("uMVPMatrix") == 0) {
	  return;
	}

	vert_params->setNamedAutoConstant("uMVPMatrix", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
	vert_params->setNamedAutoConstant("uModelMatrix", Ogre::GpuProgramParameters::ACT_WORLD_MATRIX);
	vert_params->setNamedAutoConstant("uCameraPosition", Ogre::GpuProgramParameters::ACT_CAMERA_POSITION);

	vert_params->setNamedConstant("uFadeRange", 50.0f);
	vert_params->setNamedConstant("uWindRange", 50.0f);
	vert_params->setNamedConstantFromTime("uTime", 1.0f);

	auto frag_params = material->getTechnique(i)->getPass(0)->getFragmentProgramParameters();
	frag_params->setIgnoreMissingParams(true);

	frag_params->setNamedAutoConstant("uAlphaRejection", Ogre::GpuProgramParameters::ACT_SURFACE_ALPHA_REJECTION_VALUE);
	frag_params->setNamedAutoConstant("uSurfaceAmbientColour", Ogre::GpuProgramParameters::ACT_SURFACE_AMBIENT_COLOUR);
	frag_params->setNamedAutoConstant("uSurfaceDiffuseColour", Ogre::GpuProgramParameters::ACT_SURFACE_DIFFUSE_COLOUR);
	frag_params->setNamedAutoConstant("uSurfaceSpecularColour",Ogre::GpuProgramParameters::ACT_SURFACE_SPECULAR_COLOUR);
	frag_params->setNamedAutoConstant("uSurfaceShininessColour", Ogre::GpuProgramParameters::ACT_SURFACE_SHININESS);
	frag_params->setNamedAutoConstant("uSurfaceEmissiveColour",Ogre::GpuProgramParameters::ACT_SURFACE_EMISSIVE_COLOUR);
	frag_params->setNamedAutoConstant("uAmbientLightColour", Ogre::GpuProgramParameters::ACT_AMBIENT_LIGHT_COLOUR);
	frag_params->setNamedAutoConstant("uLightCount", Ogre::GpuProgramParameters::ACT_LIGHT_COUNT);
	frag_params->setNamedAutoConstant("uLightPositionArray",Ogre::GpuProgramParameters::ACT_LIGHT_POSITION_ARRAY,OGRE_MAX_SIMULTANEOUS_LIGHTS);
	frag_params->setNamedAutoConstant("uLightDirectionArray",Ogre::GpuProgramParameters::ACT_LIGHT_DIRECTION_ARRAY,OGRE_MAX_SIMULTANEOUS_LIGHTS);
	frag_params->setNamedAutoConstant("uLightDiffuseScaledColourArray",Ogre::GpuProgramParameters::ACT_LIGHT_DIFFUSE_COLOUR_POWER_SCALED_ARRAY,OGRE_MAX_SIMULTANEOUS_LIGHTS);
	frag_params->setNamedAutoConstant("uLightAttenuationArray",Ogre::GpuProgramParameters::ACT_LIGHT_ATTENUATION_ARRAY,OGRE_MAX_SIMULTANEOUS_LIGHTS);
	frag_params->setNamedAutoConstant("uLightSpotParamsArray",Ogre::GpuProgramParameters::ACT_SPOTLIGHT_PARAMS_ARRAY,OGRE_MAX_SIMULTANEOUS_LIGHTS);
	frag_params->setNamedAutoConstant("uFogColour", Ogre::GpuProgramParameters::ACT_FOG_COLOUR);
	frag_params->setNamedAutoConstant("uFogParams", Ogre::GpuProgramParameters::ACT_FOG_PARAMS);
	frag_params->setNamedAutoConstant("uCameraPosition", Ogre::GpuProgramParameters::ACT_CAMERA_POSITION);
	frag_params->setNamedAutoConstant("cNearClipDistance", Ogre::GpuProgramParameters::ACT_NEAR_CLIP_DISTANCE);
	frag_params->setNamedAutoConstant("cFarClipDistance", Ogre::GpuProgramParameters::ACT_FAR_CLIP_DISTANCE);

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID || OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
  frag_params->setNamedConstant("uLOD", 1.0f);
#else
  frag_params->setNamedConstant("uLOD", 0.0f);
#endif

	gpu_vp_params_.push_back(vert_params);
	gpu_fp_params_.push_back(frag_params);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Pbr::UpdatePbrShadowReceiver(const Ogre::MaterialPtr &material) {
  if (Ogre::Root::getSingleton().getSceneManager("Default")->getShadowTechnique() == Ogre::SHADOWTYPE_NONE)
    return;

  for (int i = 0; i < material->getNumTechniques(); i++) {

	if (!material->getReceiveShadows() || !material->getTechnique(i)->getPass(0)->hasVertexProgram()
		|| !material->getTechnique(i)->getPass(0)->hasFragmentProgram()) {
	  return;
	}

	auto *scene = Ogre::Root::getSingleton().getSceneManager("Default");
	auto *pssm = dynamic_cast<Ogre::PSSMShadowCameraSetup *>(scene->getShadowCameraSetup().get());

	auto *technique = material->getTechnique(i);
	auto *pass = technique->getPass(0);
	auto vert_params = pass->getVertexProgramParameters();
	vert_params->setIgnoreMissingParams(true);

	if (vert_params->getConstantDefinitions().map.count("uTexWorldViewProjMatrixArray") == 0) {
	  return;
	}

	vert_params->setNamedConstant("uShadowTextureCount", OGRE_MAX_SIMULTANEOUS_SHADOW_TEXTURES);
	vert_params->setNamedAutoConstant("uTexWorldViewProjMatrixArray",Ogre::GpuProgramParameters::ACT_TEXTURE_WORLDVIEWPROJ_MATRIX_ARRAY,OGRE_MAX_SIMULTANEOUS_SHADOW_TEXTURES);

	auto frag_params = material->getTechnique(i)->getPass(0)->getFragmentProgramParameters();
	frag_params->setIgnoreMissingParams(true);

	Ogre::Vector4 splitPoints;
	const Ogre::PSSMShadowCameraSetup::SplitPointList &splitPointList = pssm->getSplitPoints();

	for (int j = 0; j < pssm->getSplitCount(); j++) {
	  splitPoints[j] = splitPointList[j + 1];
	}

	splitPoints.w = scene->getShadowFarDistance();

	if (frag_params->getConstantDefinitions().map.count("pssmSplitPoints") == 0) {
	  return;
	}

	frag_params->setNamedConstant("pssmSplitPoints", splitPoints);
	frag_params->setNamedAutoConstant("uShadowColour", Ogre::GpuProgramParameters::ACT_SHADOW_COLOUR);
	frag_params->setNamedAutoConstant("uLightCastsShadowsArray",Ogre::GpuProgramParameters::ACT_LIGHT_CASTS_SHADOWS_ARRAY,OGRE_MAX_SIMULTANEOUS_SHADOW_TEXTURES);
	frag_params->setNamedConstant("uShadowFilterSize", 0.004f);
	frag_params->setNamedConstant("uShadowFilterIterations", 16);

	int texture_count = pass->getNumTextureUnitStates();

	for (int j = 0; j < OGRE_MAX_SIMULTANEOUS_SHADOW_TEXTURES; j++) {
	  if (pass->getTextureUnitState("shadowMap" + to_string(j)))
		continue;

	  Ogre::TextureUnitState *tu = pass->createTextureUnitState();
	  tu->setName("shadowMap" + to_string(j));
	  tu->setContentType(Ogre::TextureUnitState::CONTENT_SHADOW);
	  tu->setTextureAddressingMode(Ogre::TextureUnitState::TAM_BORDER);
	  tu->setTextureBorderColour(Ogre::ColourValue::White);
	  tu->setTextureFiltering(Ogre::TFO_NONE);
	  frag_params->setNamedConstant("shadowMap" + to_string(j), texture_count + j);
	}
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Pbr::UpdatePbrIbl(const Ogre::MaterialPtr &material, bool active) {
  auto ibl_texture = material->getTechnique(0)->getPass(0)->getTextureUnitState("IBL_Specular");

  if (active) {
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
void Pbr::UpdatePbrParams(const string &material) {
  auto material_ptr = Ogre::MaterialManager::getSingleton().getByName(material);
  if (material_ptr)
  	UpdatePbrParams(material_ptr);
}

//----------------------------------------------------------------------------------------------------------------------
void Pbr::UpdatePbrIbl(const string &material, bool realtime) {
  auto material_ptr = Ogre::MaterialManager::getSingleton().getByName(material);
  UpdatePbrIbl(material_ptr, realtime);
}

//----------------------------------------------------------------------------------------------------------------------
void Pbr::UpdatePbrShadowReceiver(const string &material) {
  auto material_ptr = Ogre::MaterialManager::getSingleton().getByName(material);
  UpdatePbrShadowReceiver(material_ptr);
}

//----------------------------------------------------------------------------------------------------------------------
void Pbr::UpdatePbrShadowCaster(const string &material) {
  auto material_ptr = Ogre::MaterialManager::getSingleton().getByName(material);
  UpdatePbrShadowCaster(material_ptr);
}

} //namespace
