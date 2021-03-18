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
#include "ShadowSettings.h"
#include "Exception.h"

using namespace std;

namespace xio {
ShadowSettings::ShadowSettings()
	: pssm_split_count_(3),
	  tex_count_(OGRE_MAX_SIMULTANEOUS_SHADOW_TEXTURES),
	  camera_(nullptr),
	  scene_(nullptr),
	  shadow_enable_(true),
	  far_distance_(400),
	  tex_size_(1024),
	  tex_format_(16) {}

//----------------------------------------------------------------------------------------------------------------------
ShadowSettings::~ShadowSettings() {}

//----------------------------------------------------------------------------------------------------------------------
void ShadowSettings::UpdateParams() {
  UpdateParams(shadow_enable_, far_distance_, tex_size_, tex_format_);
}

//----------------------------------------------------------------------------------------------------------------------
void ShadowSettings::UpdateParams(bool enable, float far_distance, int tex_size, int tex_format) {
  shadow_enable_ = enable;
  far_distance_ = far_distance;
  tex_size_ = tex_size;
  tex_format_ = tex_format;

  scene_ = Ogre::Root::getSingleton().getSceneManager("Default");
  camera_ = scene_->getCamera("Default");

  scene_->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
  scene_->setShadowFarDistance(far_distance_);

  Ogre::PixelFormat texture_type;
  if (tex_format==32)
	texture_type = Ogre::PixelFormat::PF_DEPTH32;
  else if (tex_format==16)
	texture_type = Ogre::PixelFormat::PF_DEPTH16;
  else
	throw Exception("Unknown texture format, aborting;");

  scene_->setShadowTextureSize(tex_size_);
  scene_->setShadowTexturePixelFormat(texture_type);
  scene_->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, 3);
  scene_->setShadowTextureCountPerLightType(Ogre::Light::LT_SPOTLIGHT, 1);
  scene_->setShadowTextureCountPerLightType(Ogre::Light::LT_POINT, 0);

  scene_->setShadowTextureSelfShadow(true);
  scene_->setShadowCasterRenderBackFaces(true);
  scene_->setShadowFarDistance(far_distance);
  auto passCaterMaterial = Ogre::MaterialManager::getSingleton().getByName("PSSM/shadow_caster");
  scene_->setShadowTextureCasterMaterial(passCaterMaterial);

  pssm_ = make_shared<Ogre::PSSMShadowCameraSetup>();
  pssm_->calculateSplitPoints(pssm_split_count_, 0.1, scene_->getShadowFarDistance());
  split_points_ = pssm_->getSplitPoints();
  pssm_->setSplitPadding(1.0);

  for (int i = 0; i < pssm_split_count_; i++)
	pssm_->setOptimalAdjustFactor(i, 0.0);

  scene_->setShadowCameraSetup(pssm_);
  scene_->setShadowColour(Ogre::ColourValue::Black);

  if (!enable)
	scene_->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
}

//----------------------------------------------------------------------------------------------------------------------
void ShadowSettings::UpdateSplits(float padding, const vector<float> &pssm_factor) {
  pssm_->setSplitPadding(padding);
  pssm_->calculateSplitPoints(pssm_split_count_, 0.1, scene_->getShadowFarDistance());

  for (size_t i = 0; i < pssm_factor.size(); i++) {
	pssm_->setOptimalAdjustFactor(i, pssm_factor[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ShadowSettings::SetManualSplits(const vector<float> &split_points) {
  split_points_ = split_points;
  pssm_->setSplitPoints(split_points);
}

//----------------------------------------------------------------------------------------------------------------------
const vector<float> &ShadowSettings::GetSplitPoints() {
  return split_points_;
}

//----------------------------------------------------------------------------------------------------------------------
const Ogre::PSSMShadowCameraSetup &ShadowSettings::GetPssmSetup() {
  return *pssm_;
}

} //namespace
