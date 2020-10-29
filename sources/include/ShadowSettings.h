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
#include "NoCopy.h"
#include <OgreRoot.h>
#include <OgreMaterialManager.h>
#include <OgreShadowCameraSetupPSSM.h>
#include <vector>

namespace xio {
class ShadowSettings : public NoCopy {
 public:
  ShadowSettings() {}
  virtual ~ShadowSettings() {}
//----------------------------------------------------------------------------------------------------------------------
  void UpdateParams(bool enable, float far_distance, int tex_size, int tex_format = 16) {
    auto *scene = Ogre::Root::getSingleton().getSceneManager("Default");
    if (!enable) {
      scene->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
      return;
    }

    scene->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
    Ogre::PixelFormat texture_type;
    if (tex_format == 32)
      texture_type = Ogre::PixelFormat::PF_DEPTH32;
    else if (tex_format == 16)
      texture_type = Ogre::PixelFormat::PF_DEPTH16;
    else
      throw Exception("Unknown texture format, aborting;");

    scene->setShadowTextureSettings(tex_size, tex_count_, texture_type);
    scene->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, 3);
    scene->setShadowTextureCountPerLightType(Ogre::Light::LT_SPOTLIGHT, 3);
    scene->setShadowTextureCountPerLightType(Ogre::Light::LT_POINT, 6);
    scene->setShadowTextureSelfShadow(true);
    scene->setShadowCasterRenderBackFaces(true);
    scene->setShadowFarDistance(far_distance);
    auto passCaterMaterial = Ogre::MaterialManager::getSingleton().getByName("PSSM/NoAlpha/shadow_caster");
    scene->setShadowTextureCasterMaterial(passCaterMaterial);

    pssm_ = std::make_shared<Ogre::PSSMShadowCameraSetup>();
    pssm_->calculateSplitPoints(split_count_, 0.1, scene->getShadowFarDistance());
    split_points_ = pssm_->getSplitPoints();
    pssm_->setSplitPadding(1.0);
    pssm_->setOptimalAdjustFactor(0, 0.0);
    pssm_->setOptimalAdjustFactor(1, 0.0);
    pssm_->setOptimalAdjustFactor(2, 0.0);
    scene->setShadowCameraSetup(pssm_);
  }
//----------------------------------------------------------------------------------------------------------------------
  void UpdateSplits(float padding, const std::vector<float> &pssm_factor) {
    pssm_->setSplitPadding(padding);
    auto *scene = Ogre::Root::getSingleton().getSceneManager("Default");
    pssm_->calculateSplitPoints(split_count_, 0.1, scene->getShadowFarDistance());
    for (size_t i = 0; i < pssm_factor.size(); i++) {
      pssm_->setOptimalAdjustFactor(i, pssm_factor[i]);
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  void SetManualSplits(const std::vector<float> &split_points) {
    split_points_ = split_points;
    pssm_->setSplitPoints(split_points);
  }
//----------------------------------------------------------------------------------------------------------------------
  const std::vector<float> &GetSplitPoints() {
    return split_points_;
  }
//----------------------------------------------------------------------------------------------------------------------
  Ogre::PSSMShadowCameraSetup* GetPssmSetup() {
    return pssm_.get();
  }

 private:
  int16_t split_count_ = 3;
  int16_t tex_count_ = 3;
  std::vector<float> split_points_;
  std::shared_ptr<Ogre::PSSMShadowCameraSetup> pssm_;
};
}