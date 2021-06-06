// MIT License
//
// Copyright (c) 2021 Andrew Vasiliev
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once
#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
#include <RTShaderSystem/OgreShaderGenerator.h>

#include "Exception.h"

namespace xio {

void InitRtss();

void CreateRtssShaders(const std::string &cache_path = "");

void InitPssm(const std::vector<float> &split_points);

void InitInstansing();

class ShaderResolver final : public Ogre::MaterialManager::Listener {
 public:
  ShaderResolver(Ogre::RTShader::ShaderGenerator *shader_generator);

  static bool FixMaterial(const std::string &material_name);

  Ogre::Technique *handleSchemeNotFound(unsigned short scheme_index, const std::string &scheme_name,
                                        Ogre::Material *original_material, unsigned short lod_index,
                                        const Ogre::Renderable *renderable);

  bool afterIlluminationPassesCreated(Ogre::Technique *technique);

  bool beforeIlluminationPassesCleared(Ogre::Technique *technique);

 private:
  Ogre::RTShader::ShaderGenerator *shader_generator_;
};

}  // namespace xio

#endif
