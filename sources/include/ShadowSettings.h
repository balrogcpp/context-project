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

#pragma once

#include "NoCopy.h"
#include <OgreRoot.h>
#include <OgreMaterialManager.h>
#include <OgreShadowCameraSetupPSSM.h>
#include <vector>

namespace Context {
class ShadowSettings : public NoCopy {
 public:
  ShadowSettings() {
    auto *scene = Ogre::Root::getSingleton().getSceneManager("Default");
    uint16_t tex_size = 1024;
    uint16_t tex_count = 3;
    float graphics_shadows_pssm_0_ = 0.0f;
    float graphics_shadows_pssm_1_ = 0.0f;
    float graphics_shadows_pssm_2_ = 0.0f;
    float graphics_shadows_far_distance_ = 500;
    float graphics_shadows_split_padding_ = 1.0f;
    const int pssm_splits_ = 3;

    scene->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
    Ogre::PixelFormat texture_type = Ogre::PF_DEPTH16;
    scene->setShadowTextureSettings(tex_size, tex_count, texture_type);
    scene->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, 3);
    scene->setShadowTextureCountPerLightType(Ogre::Light::LT_SPOTLIGHT, 3);
    scene->setShadowTextureCountPerLightType(Ogre::Light::LT_POINT, 6);
    scene->setShadowTextureSelfShadow(true);
    scene->setShadowCasterRenderBackFaces(true);
    scene->setShadowFarDistance(graphics_shadows_far_distance_);
    scene->setShadowDirectionalLightExtrusionDistance(graphics_shadows_far_distance_);
    auto passCaterMaterial = Ogre::MaterialManager::getSingleton().getByName("PSSM/shadow_caster");
    scene->setShadowTextureCasterMaterial(passCaterMaterial);
    auto pssm_setup_ = std::make_shared<Ogre::PSSMShadowCameraSetup>();
    pssm_setup_->calculateSplitPoints(pssm_splits_, 0.1, scene->getShadowFarDistance());
    split_points_ = pssm_setup_->getSplitPoints();
    pssm_setup_->setSplitPadding(graphics_shadows_split_padding_);
    pssm_setup_->setOptimalAdjustFactor(0, graphics_shadows_pssm_0_);
    pssm_setup_->setOptimalAdjustFactor(1, graphics_shadows_pssm_1_);
    pssm_setup_->setOptimalAdjustFactor(2, graphics_shadows_pssm_2_);
    scene->setShadowCameraSetup(pssm_setup_);
  }

  virtual ~ShadowSettings() {}

 private:
  std::vector<float> split_points_;
  std::shared_ptr<Ogre::PSSMShadowCameraSetup> pssm_;
};
}