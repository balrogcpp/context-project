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
#include "PbrShaderUtils.h"
#include "Exception.h"

namespace xio {
class ShadowSettings : public NoCopy {
 public:
  ShadowSettings();

  virtual ~ShadowSettings();

  void UpdateParams();

  void UpdateParams(bool enable, float far_distance, int tex_size, int tex_format = 16);

  void UpdateSplits(float padding, const std::vector<float> &pssm_factor);

  void SetManualSplits(const std::vector<float> &split_points);

  const std::vector<float> &GetSplitPoints();

  const Ogre::PSSMShadowCameraSetup &GetPssmSetup();

 private:
  int16_t pssm_split_count_ = 3;
  int16_t tex_count_ = OGRE_MAX_SIMULTANEOUS_SHADOW_TEXTURES;
  std::vector<float> split_points_;
  std::shared_ptr<Ogre::PSSMShadowCameraSetup> pssm_;
  Ogre::Camera *camera_ = nullptr;
  Ogre::SceneManager *scene_ = nullptr;
  bool shadow_enable_ = true;
  float far_distance_ = 400.0;
  int16_t tex_size_ = 2048;
  int tex_format_ = 16;
};
}