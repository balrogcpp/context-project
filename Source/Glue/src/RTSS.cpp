// This source file is part of Glue Engine. Created by Andrey Vasiliev

#include "PCHeader.h"
#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
#include "RTSS.h"
#include <RTShaderSystem/OgreRTShaderSystem.h>
#include <RTShaderSystem/OgreShaderGenerator.h>

using namespace std;
using namespace Ogre;

namespace Glue {

void InitRTSS() {
  if (!RTShader::ShaderGenerator::initialize()) throw Exception("RTSS failed to initialize");
}

static unique_ptr<ShaderResolver> ResolverPtr;

void InitRTSSRuntime(string CachePath) {
  auto *OgreScene = Root::getSingleton().getSceneManager("Default");
  auto *OgreViewport = OgreScene->getCamera("Default")->getViewport();
  auto *ShaderGenerator = RTShader::ShaderGenerator::getSingletonPtr();

  OgreViewport->setMaterialScheme(MSN_SHADERGEN);
  ShaderGenerator->addSceneManager(OgreScene);
  OgreViewport->setMaterialScheme(MSN_SHADERGEN);
  ShaderGenerator->setShaderCachePath(CachePath);
  ResolverPtr = make_unique<ShaderResolver>(ShaderGenerator);
  MaterialManager::getSingleton().addListener(ResolverPtr.get());
}

void ClearRTSSRuntime() {
  if (ResolverPtr) {
    MaterialManager::getSingleton().removeListener(ResolverPtr.get());
    ResolverPtr.reset();
  }
}

void InitRTSSPSSM(const vector<float> &SplitPoints) {
  const auto DSN = MSN_SHADERGEN;
  auto &rtShaderGen = RTShader::ShaderGenerator::getSingleton();
  auto *schemRenderState = rtShaderGen.getRenderState(DSN);

  auto subRenderState = rtShaderGen.createSubRenderState<RTShader::IntegratedPSSM3>();
  subRenderState->setSplitPoints(SplitPoints);
  schemRenderState->addTemplateSubRenderState(subRenderState);
}

void InitRTSSInstansing() {
  auto *scene_ = Root::getSingleton().getSceneManager("Default");
  auto *camera_ = scene_->getCamera("Default");
  auto *viewport_ = camera_->getViewport();
  const auto DSN = MSN_SHADERGEN;

  RTShader::ShaderGenerator &rtShaderGen = RTShader::ShaderGenerator::getSingleton();
  viewport_->setMaterialScheme(DSN);
  RTShader::RenderState *schemRenderState = rtShaderGen.getRenderState(DSN);
  RTShader::SubRenderState *subRenderState = rtShaderGen.createSubRenderState<RTShader::IntegratedPSSM3>();
  schemRenderState->addTemplateSubRenderState(subRenderState);

  // Add the hardware skinning to the shader generator default render state
  subRenderState = rtShaderGen.createSubRenderState<RTShader::HardwareSkinning>();
  schemRenderState->addTemplateSubRenderState(subRenderState);

  // increase max bone count for higher efficiency
  RTShader::HardwareSkinningFactory::getSingleton().setMaxCalculableBoneCount(80);

  // re-generate shaders to include new SRSs
  rtShaderGen.invalidateScheme(DSN);
  rtShaderGen.validateScheme(DSN);

  // update scheme for FFP supporting rendersystems
  MaterialManager::getSingleton().setActiveScheme(viewport_->getMaterialScheme());
}

ShaderResolver::ShaderResolver(RTShader::ShaderGenerator *ShaderGeneratorPtr) { this->ShaderGeneratorPtr = ShaderGeneratorPtr; }

bool ShaderResolver::FixMaterial(const string &material_name) {
  auto &mShaderGenerator = RTShader::ShaderGenerator::getSingleton();
  auto originalMaterial = MaterialManager::getSingleton().getByName(material_name);
  const auto DSNOld = MaterialManager::DEFAULT_SCHEME_NAME;
  const auto DSN = MSN_SHADERGEN;

  if (!originalMaterial) originalMaterial = MaterialManager::getSingleton().getByName(material_name, RGN_INTERNAL);

  if (!originalMaterial) return false;

  // SetUp shader generated technique for this material.
  bool techniqueCreated = mShaderGenerator.createShaderBasedTechnique(*originalMaterial, DSNOld, DSN);

  // Case technique registration succeeded.
  // Force creating the shaders for the generated technique.

  if (techniqueCreated)
    mShaderGenerator.validateMaterial(DSN, originalMaterial->getName(), originalMaterial->getGroup());
  else
    originalMaterial->getTechnique(0)->setSchemeName(DSN);

  return true;
}

Technique *ShaderResolver::handleSchemeNotFound(unsigned short SchemeIndex, const string &SchemeName, Material *OriginalMaterialPtr,
                                                unsigned short LodIndex, const Renderable *OgreRenderable) {
  const auto DSNOld = MaterialManager::DEFAULT_SCHEME_NAME;
  const auto DSN = MSN_SHADERGEN;

  if (SchemeName != DSN) return nullptr;

  // SetUp shader generated technique for this material.
  bool techniqueCreated = ShaderGeneratorPtr->createShaderBasedTechnique(*OriginalMaterialPtr, DSNOld, SchemeName);

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
    OriginalMaterialPtr->getTechnique(0)->setSchemeName(DSN);
  }

  return nullptr;
}

bool ShaderResolver::afterIlluminationPassesCreated(Technique *OgreTechnique) {
  const auto DSNOld = MaterialManager::DEFAULT_SCHEME_NAME;
  const auto DSN = MSN_SHADERGEN;

  if (OgreTechnique->getSchemeName() == DSN) {
    Material *mat = OgreTechnique->getParent();
    ShaderGeneratorPtr->validateMaterialIlluminationPasses(OgreTechnique->getSchemeName(), mat->getName(), mat->getGroup());
    return true;
  } else {
    return false;
  }
}

bool ShaderResolver::beforeIlluminationPassesCleared(Technique *OgreTechnique) {
  if (OgreTechnique->getSchemeName() == MSN_SHADERGEN) {
    Material *mat = OgreTechnique->getParent();
    ShaderGeneratorPtr->invalidateMaterialIlluminationPasses(OgreTechnique->getSchemeName(), mat->getName(), mat->getGroup());
    return true;
  } else {
    return false;
  }
}

}  // namespace Glue

#endif  // OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
