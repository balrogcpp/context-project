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

#include <OgreMaterialManager.h>

namespace Ogre::RTShader {
class ShaderGenerator;
}

namespace Context {

class ShaderResolver : public Ogre::MaterialManager::Listener {
 public:
  explicit ShaderResolver(Ogre::RTShader::ShaderGenerator *shader_generator);
  Ogre::Technique *handleSchemeNotFound(unsigned short scheme_index,
                                        const std::string &scheme_name,
                                        Ogre::Material *original_material, unsigned short lod_index,
                                        const Ogre::Renderable *renderable) final;

  bool afterIlluminationPassesCreated(Ogre::Technique *technique) final;
  bool beforeIlluminationPassesCleared(Ogre::Technique *technique) final;
  static bool FixMaterial(const std::string &material_name);
  static bool ProtectMaterial(const std::string &material_name);

 protected:
  Ogre::RTShader::ShaderGenerator *shader_generator_;
  static std::vector<std::string> material_list_;

 public:
  [[nodiscard]] Ogre::RTShader::ShaderGenerator *GetMShaderGenerator() const {
    return shader_generator_;
  }
  // The shader generator instance.
};
}
