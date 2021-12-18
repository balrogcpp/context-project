// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "RTSSUtils.h"
#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
#include <RTShaderSystem/OgreRTShaderSystem.h>
#include <RTShaderSystem/OgreShaderGenerator.h>

using namespace std;

namespace Glue {

void InitRtss() {
  if (!Ogre::RTShader::ShaderGenerator::initialize()) {
    throw Exception("RTSS failed to initialize");
  }
}

void CreateRtssShaders(const string &cache_path) {
  auto *scene_ = Ogre::Root::getSingleton().getSceneManager("Default");
  auto *camera_ = scene_->getCamera("Default");
  auto *viewport_ = camera_->getViewport();

  viewport_->setMaterialScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

  auto *shader_generator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
  shader_generator->addSceneManager(scene_);
  viewport_->setMaterialScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

  shader_generator->setShaderCachePath(cache_path);

  Ogre::MaterialManager::getSingleton().addListener(new ShaderResolver(shader_generator));
}

void InitPssm(const vector<float> &split_points) {
  Ogre::RTShader::ShaderGenerator &rtShaderGen = Ogre::RTShader::ShaderGenerator::getSingleton();
  auto subRenderState = rtShaderGen.createSubRenderState<Ogre::RTShader::IntegratedPSSM3>();
  Ogre::RTShader::RenderState *schemRenderState =
      rtShaderGen.getRenderState(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
  subRenderState->setSplitPoints(split_points);
  schemRenderState->addTemplateSubRenderState(subRenderState);
}

void InitInstansing() {
  auto *scene_ = Ogre::Root::getSingleton().getSceneManager("Default");
  auto *camera_ = scene_->getCamera("Default");
  auto *viewport_ = camera_->getViewport();
  Ogre::RTShader::ShaderGenerator &rtShaderGen = Ogre::RTShader::ShaderGenerator::getSingleton();
  viewport_->setMaterialScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
  Ogre::RTShader::RenderState *schemRenderState =
      rtShaderGen.getRenderState(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
  Ogre::RTShader::SubRenderState *subRenderState = rtShaderGen.createSubRenderState<Ogre::RTShader::IntegratedPSSM3>();
  schemRenderState->addTemplateSubRenderState(subRenderState);

  // Add the hardware skinning to the shader generator default render state
  subRenderState = rtShaderGen.createSubRenderState<Ogre::RTShader::HardwareSkinning>();
  schemRenderState->addTemplateSubRenderState(subRenderState);

  // increase max bone count for higher efficiency
  Ogre::RTShader::HardwareSkinningFactory::getSingleton().setMaxCalculableBoneCount(80);

  // re-generate shaders to include new SRSs
  rtShaderGen.invalidateScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
  rtShaderGen.validateScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

  // update scheme for FFP supporting rendersystems
  Ogre::MaterialManager::getSingleton().setActiveScheme(viewport_->getMaterialScheme());
}

ShaderResolver::ShaderResolver(Ogre::RTShader::ShaderGenerator *ShaderGeneratorPtr) {
  this->ShaderGeneratorPtr = ShaderGeneratorPtr;
}

bool ShaderResolver::FixMaterial(const string &material_name) {
  auto &mShaderGenerator = Ogre::RTShader::ShaderGenerator::getSingleton();
  auto originalMaterial = Ogre::MaterialManager::getSingleton().getByName(material_name);

  if (!originalMaterial) {
    originalMaterial = Ogre::MaterialManager::getSingleton().getByName(material_name, Ogre::RGN_INTERNAL);
  }

  bool verbose = false;

  if (!originalMaterial) {
    return false;
  }

  // SetUp shader generated technique for this material.
  bool techniqueCreated =
      mShaderGenerator.createShaderBasedTechnique(*originalMaterial, Ogre::MaterialManager::DEFAULT_SCHEME_NAME,
                                                  Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

  // Case technique registration succeeded.
  if (techniqueCreated) {
    // Force creating the shaders for the generated technique.
    mShaderGenerator.validateMaterial(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME, originalMaterial->getName(),
                                      originalMaterial->getGroup());
  } else {
    originalMaterial->getTechnique(0)->setSchemeName(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
  }

  return true;
}

Ogre::Technique *ShaderResolver::handleSchemeNotFound(unsigned short SchemeIndex, const string &SchemeName,
                                                      Ogre::Material *OriginalMaterialPtr, unsigned short LodIndex,
                                                      const Ogre::Renderable *OgreRenderable) {
  if (SchemeName != Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME) {
    return nullptr;
  }

  // SetUp shader generated technique for this material.
  bool techniqueCreated = ShaderGeneratorPtr->createShaderBasedTechnique(
      *OriginalMaterialPtr, Ogre::MaterialManager::DEFAULT_SCHEME_NAME, SchemeName);

  // Case technique registration succeeded.
  if (techniqueCreated) {
    // Force creating the shaders for the generated technique.
    ShaderGeneratorPtr->validateMaterial(SchemeName, OriginalMaterialPtr->getName(), OriginalMaterialPtr->getGroup());

    // Grab the generated technique.
    for (const auto &it : OriginalMaterialPtr->getTechniques()) {
      if (it->getSchemeName() == SchemeName) {
        return it;
      }
    }
  } else {
    OriginalMaterialPtr->getTechnique(0)->setSchemeName(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
  }

  return nullptr;
}

bool ShaderResolver::afterIlluminationPassesCreated(Ogre::Technique *OgreTechnique) {
  if (OgreTechnique->getSchemeName() == Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME) {
    Ogre::Material *mat = OgreTechnique->getParent();
    ShaderGeneratorPtr->validateMaterialIlluminationPasses(OgreTechnique->getSchemeName(), mat->getName(), mat->getGroup());
    return true;
  } else {
    return false;
  }
}

bool ShaderResolver::beforeIlluminationPassesCleared(Ogre::Technique *OgreTechnique) {
  if (OgreTechnique->getSchemeName() == Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME) {
    Ogre::Material *mat = OgreTechnique->getParent();
    ShaderGeneratorPtr->invalidateMaterialIlluminationPasses(OgreTechnique->getSchemeName(), mat->getName(), mat->getGroup());
    return true;
  } else {
    return false;
  }
}

}  // namespace Glue

#endif // OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
