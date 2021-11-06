// This source file is part of context-project
// Created by Andrew Vasiliev

#pragma once
#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
#include <RTShaderSystem/OgreShaderGenerator.h>

#include "Exception.h"

namespace glue {

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

}  // namespace glue

#endif
