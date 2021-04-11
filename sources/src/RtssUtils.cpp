//MIT License
//
//Copyright (c) 2021 Andrew Vasiliev
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

#include "pcheader.h"
#include "RtssUtils.h"

#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM

#include <RTShaderSystem/OgreRTShaderSystem.h>
#include <RTShaderSystem/OgreShaderGenerator.h>

using namespace std;

namespace xio {

void InitRtss() {
  if (!Ogre::RTShader::ShaderGenerator::initialize()) {
	throw Exception("RTSS failed to initialize");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CreateRtssShaders(const string &cache_path) {
  auto *scene_ = Ogre::Root::getSingleton().getSceneManager("Default");
  auto *camera_ = scene_->getCamera("Default");
  auto *viewport_ = camera_->getViewport();

  viewport_->setMaterialScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

  auto *shader_generator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
  shader_generator->addSceneManager(scene_);
  viewport_->setMaterialScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

//  if (!filesystem::exists(cache_path) && !cache_path.empty())
//	filesystem::create_directories(cache_path);

  shader_generator->setShaderCachePath(cache_path);

  Ogre::MaterialManager::getSingleton().addListener(new ShaderResolver(shader_generator));
}

//----------------------------------------------------------------------------------------------------------------------
void InitPssm(const vector<float> &split_points) {
  Ogre::RTShader::ShaderGenerator &rtShaderGen = Ogre::RTShader::ShaderGenerator::getSingleton();
  auto subRenderState = rtShaderGen.createSubRenderState<Ogre::RTShader::IntegratedPSSM3>();
  Ogre::RTShader::RenderState *schemRenderState = rtShaderGen.getRenderState(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
  subRenderState->setSplitPoints(split_points);
//  schemRenderState->addTemplateSubRenderState(subRenderState);
}

//----------------------------------------------------------------------------------------------------------------------
void InitInstansing() {
  auto *scene_ = Ogre::Root::getSingleton().getSceneManager("Default");
  auto *camera_ = scene_->getCamera("Default");
  auto *viewport_ = camera_->getViewport();
  Ogre::RTShader::ShaderGenerator &rtShaderGen = Ogre::RTShader::ShaderGenerator::getSingleton();
  viewport_->setMaterialScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
  Ogre::RTShader::RenderState
	  *schemRenderState = rtShaderGen.getRenderState(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
  Ogre::RTShader::SubRenderState *subRenderState = rtShaderGen.createSubRenderState<Ogre::RTShader::IntegratedPSSM3>();
  schemRenderState->addTemplateSubRenderState(subRenderState);

  //Add the hardware skinning to the shader generator default render state
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

//----------------------------------------------------------------------------------------------------------------------
ShaderResolver::ShaderResolver(Ogre::RTShader::ShaderGenerator *shader_generator) {
  shader_generator_ = shader_generator;
}

//----------------------------------------------------------------------------------------------------------------------
bool ShaderResolver::FixMaterial(const string &material_name) {
  static vector<string> material_list_;

  if (find(material_list_.begin(), material_list_.end(), material_name)==material_list_.end())
	material_list_.push_back(material_name);
  else
	return false;

  auto &mShaderGenerator = Ogre::RTShader::ShaderGenerator::getSingleton();
  auto originalMaterial = Ogre::MaterialManager::getSingleton().getByName(material_name);

  if (!originalMaterial) {
	originalMaterial = Ogre::MaterialManager::getSingleton().getByName(material_name, Ogre::RGN_INTERNAL);
  }

  bool verbose = false;

  if (!originalMaterial) {
	if (verbose)
	  cout << "Requested to fix material " << material_name << " but it does not exist\n";

	return false;
  } else {
	if (verbose)
	  cout << "Fixing Material " << material_name << " !\n";
  }

  // Init shader generated technique for this material.
  bool techniqueCreated = mShaderGenerator.createShaderBasedTechnique(
	  *originalMaterial,
	  Ogre::MaterialManager::DEFAULT_SCHEME_NAME,
	  Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME
  );

  // Case technique registration succeeded.
  if (techniqueCreated) {
	// Force creating the shaders for the generated technique.
	mShaderGenerator.validateMaterial(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME,
									  originalMaterial->getName(),
									  originalMaterial->getGroup());
  } else {
	originalMaterial->getTechnique(0)->setSchemeName(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
Ogre::Technique *ShaderResolver::handleSchemeNotFound(unsigned short scheme_index,
													  const string &scheme_name,
													  Ogre::Material *original_material,
													  unsigned short lod_index,
													  const Ogre::Renderable *renderable) {
//  static vector<string> material_list_;
//  string material_name = original_material->getName();
//  if (find(material_list_.begin(), material_list_.end(), material_name)==material_list_.end()) {
//	material_list_.push_back(material_name);
//  } else {
//	return nullptr;
//  }

  if (scheme_name!=Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME) {
	return nullptr;
  }


  // Init shader generated technique for this material.
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
	  if (it->getSchemeName()==scheme_name) {
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
  if (technique->getSchemeName()==Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME) {
	Ogre::Material *mat = technique->getParent();
	shader_generator_->validateMaterialIlluminationPasses(technique->getSchemeName(),
														  mat->getName(), mat->getGroup());
	return true;
  } else {
	return false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool ShaderResolver::beforeIlluminationPassesCleared(Ogre::Technique *technique) {
  if (technique->getSchemeName()==Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME) {
	Ogre::Material *mat = technique->getParent();
	shader_generator_->invalidateMaterialIlluminationPasses(technique->getSchemeName(),
															mat->getName(),
															mat->getGroup());
	return true;
  } else {
	return false;
  }
}

} //namespace

#endif
