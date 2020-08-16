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

#include "pcheader.hpp"

#include "ShaderResolver.h"
#include "ContextManager.h"
#include "ConfigManager.h"

#include <OgreTechnique.h>
#include <Ogre.h>
#include <iostream>

#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM

namespace Context {
std::vector<std::string> ShaderResolver::material_list_;

//----------------------------------------------------------------------------------------------------------------------
ShaderResolver::ShaderResolver(Ogre::RTShader::ShaderGenerator *shader_generator) {
  shader_generator_ = shader_generator;
}
//----------------------------------------------------------------------------------------------------------------------
bool ShaderResolver::FixMaterial(const std::string &material_name) {
  if (ConfigManager::GetSingleton().GetBool("rtss_enable")) {
    if (std::find(material_list_.begin(), material_list_.end(), material_name) == material_list_.end()) {
      material_list_.push_back(material_name);
    } else {
      return false;
    }

    auto &mShaderGenerator = Ogre::RTShader::ShaderGenerator::getSingleton();
    auto originalMaterial = Ogre::MaterialManager::getSingleton().getByName(material_name,
                                                                            Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
    auto schemeName = Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME;

    if (!originalMaterial) {
      originalMaterial = Ogre::MaterialManager::getSingleton().getByName(material_name,
                                                                         Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME);
    }

    bool verbose = ConfigManager::GetSingleton().GetBool("global_verbose_enable");

    if (!originalMaterial) {
      if (verbose)
        std::cout << "Requested to fix material " << material_name << " but it does not exist\n";

      return false;
    } else {
      if (verbose)
        std::cout << "Fixing Material " << material_name << " !\n";
    }

    // Create shader generated technique for this material.
    bool techniqueCreated = mShaderGenerator.createShaderBasedTechnique(
        *originalMaterial,
        Ogre::MaterialManager::DEFAULT_SCHEME_NAME,
        schemeName
    );

    // Case technique registration succeeded.
    if (techniqueCreated) {
      // Force creating the shaders for the generated technique.
      mShaderGenerator.validateMaterial(schemeName, originalMaterial->getName(), originalMaterial->getGroup());
    } else {
      originalMaterial->getTechnique(0)->setSchemeName(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
    }

    return true;
  } else {
    return false;
  }
}
//----------------------------------------------------------------------------------------------------------------------
bool ShaderResolver::ProtectMaterial(const std::string &material_name) {
  auto originalMaterial = Ogre::MaterialManager::getSingleton().getByName(material_name);
  Ogre::Viewport *ogreViewport = ContextManager::GetSingleton().GetOgreViewport();
  originalMaterial->getTechnique(0)->setSchemeName(ogreViewport->getMaterialScheme());

  return true;
}
//----------------------------------------------------------------------------------------------------------------------
Ogre::Technique *ShaderResolver::handleSchemeNotFound(unsigned short scheme_index,
                                                      const std::string &scheme_name,
                                                      Ogre::Material *original_material,
                                                      unsigned short lod_index,
                                                      const Ogre::Renderable *renderable) {
  if (scheme_name != Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME) {
    return nullptr;
  }

  std::string material_name = original_material->getName();
  if (std::find(material_list_.begin(), material_list_.end(), material_name) == material_list_.end()) {
    material_list_.push_back(material_name);
  } else {
    return nullptr;
  }

  if (ConfigManager::GetSingleton().GetBool("global_verbose_enable")) {
    std::cout << "Fixing Material " << original_material->getName() << " !\n";
  }

  // Case this is the default shader generator scheme.

  // Create shader generated technique for this material.
  bool techniqueCreated = shader_generator_->createShaderBasedTechnique(
      *original_material,
      Ogre::MaterialManager::DEFAULT_SCHEME_NAME,
      scheme_name);

  // Case technique registration succeeded.
  if (techniqueCreated) {
    // Force creating the shaders for the generated technique.
    shader_generator_->validateMaterial(scheme_name, original_material->getName(), original_material->getGroup());

    // Grab the generated technique.
    for (const auto &it : original_material->getTechniques()) {
      if (it->getSchemeName() == scheme_name) {
        return it;
      }
    }
  } else {
    original_material->getTechnique(0)->setSchemeName(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
  }

  return nullptr;
}
//----------------------------------------------------------------------------------------------------------------------
bool ShaderResolver::afterIlluminationPassesCreated(Ogre::Technique *technique) {
  if (technique->getSchemeName() == Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME) {
    Ogre::Material *mat = technique->getParent();
    shader_generator_->validateMaterialIlluminationPasses(technique->getSchemeName(),
                                                          mat->getName(), mat->getGroup());
    return true;
  }
  return false;
}
//----------------------------------------------------------------------------------------------------------------------
bool ShaderResolver::beforeIlluminationPassesCleared(Ogre::Technique *technique) {
  if (technique->getSchemeName() == Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME) {
    Ogre::Material *mat = technique->getParent();
    shader_generator_->invalidateMaterialIlluminationPasses(technique->getSchemeName(),
                                                            mat->getName(), mat->getGroup());
    return true;
  }
  return false;
}

} //namespace Context

#endif
