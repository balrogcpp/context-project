// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
#include "Exception.h"
#include <RTShaderSystem/OgreShaderGenerator.h>

namespace Glue {

void InitRtss();

void CreateRtssShaders(const std::string &cache_path = "");

void InitPssm(const std::vector<float> &split_points);

void InitInstansing();

class ShaderResolver final : public Ogre::MaterialManager::Listener {
 public:
  ShaderResolver(Ogre::RTShader::ShaderGenerator *ShaderGeneratorPtr);

  static bool FixMaterial(const std::string &material_name);

  Ogre::Technique *handleSchemeNotFound(unsigned short SchemeIndex, const std::string &SchemeName,
                                        Ogre::Material *OriginalMaterialPtr, unsigned short LodIndex,
                                        const Ogre::Renderable *OgreRenderable);

  bool afterIlluminationPassesCreated(Ogre::Technique *OgreTechnique);

  bool beforeIlluminationPassesCleared(Ogre::Technique *OgreTechnique);

 protected:
  Ogre::RTShader::ShaderGenerator *ShaderGeneratorPtr = nullptr;
};

}  // namespace Glue

#endif
