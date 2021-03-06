//MIT License
//
//Copyright (c) 2021 Andrei Vasilev
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
#include "Compositor.h"
#include <iostream>

using namespace std;

namespace xio {

class GBufferSchemeHandler : public Ogre::MaterialManager::Listener {
 public:

  GBufferSchemeHandler() {
	ref_mat_ = Ogre::MaterialManager::getSingleton().getByName("gbuffer");
	ref_mat_alpha_ = Ogre::MaterialManager::getSingleton().getByName("gbuffer_alpha");
	ref_mat_empty_ = Ogre::MaterialManager::getSingleton().getByName("gbuffer_empty");

	if (!ref_mat_ || !ref_mat_alpha_ || !ref_mat_empty_)
		throw Exception("No available materials for Compositor::GBufferSchemeHandler");

	ref_mat_->load();
	ref_mat_alpha_->load();
	ref_mat_empty_->load();
  }

//----------------------------------------------------------------------------------------------------------------------
  Ogre::Technique *handleSchemeNotFound(unsigned short schemeIndex,
										const Ogre::String &schemeName,
										Ogre::Material *originalMaterial,
										unsigned short lodIndex,
										const Ogre::Renderable *rend) final {
	Ogre::Technique *gBufferTech = originalMaterial->createTechnique();

	auto *pass = originalMaterial->getTechnique(0)->getPass(0);
	int alpha_rejection = static_cast<int>(pass->getAlphaRejectValue());
	bool has_alpha = false;

	if (pass->getNumTextureUnitStates() > 0) {
	  auto tu = pass->getTextureUnitState(0);

	  if (tu) {
		has_alpha = tu->_getTexturePtr()->hasAlpha();
	  }
	}

	if (pass->getNumTextureUnitStates() > 0 && has_alpha && alpha_rejection > 0) {
	  gBufferTech->setSchemeName(schemeName);
	  Ogre::Pass *gbufPass = gBufferTech->createPass();
	  *gbufPass = *ref_mat_alpha_->getTechnique(0)->getPass(0);

	  auto texture_albedo = pass->getTextureUnitState(0);
	  if (texture_albedo) {
		string texture_name = pass->getTextureUnitState(0)->getTextureName();
		auto *AlphaTexPtr = gBufferTech->getPass(0)->getTextureUnitState("BaseColor");

		if (AlphaTexPtr) {
		  AlphaTexPtr->setContentType(Ogre::TextureUnitState::CONTENT_NAMED);
		  AlphaTexPtr->setTextureName(texture_name);
		}
	  }
	} else {
	  gBufferTech->setSchemeName(schemeName);
	  Ogre::Pass *gbufPass = gBufferTech->createPass();
	  *gbufPass = *ref_mat_->getTechnique(0)->getPass(0);
	}

	gpu_vp_params_.push_back(gBufferTech->getPass(0)->getVertexProgramParameters());
	gpu_fp_params_.push_back(gBufferTech->getPass(0)->getFragmentProgramParameters());
	gBufferTech->getPass(0)->getVertexProgramParameters()->setIgnoreMissingParams(true);
	gBufferTech->getPass(0)->getFragmentProgramParameters()->setIgnoreMissingParams(true);

	return gBufferTech;
  }

//----------------------------------------------------------------------------------------------------------------------
  void Update(Ogre::Matrix4 mvp_prev, float time) {
	for (auto &it : gpu_fp_params_) {
	  it->setNamedConstant("uFrameTime", time);
	}

	for (auto &it : gpu_vp_params_) {
	  it->setNamedConstant("cWorldViewProjPrev", mvp_prev);
	}
  }

 private:
  Ogre::MaterialPtr ref_mat_;
  Ogre::MaterialPtr ref_mat_alpha_;
  Ogre::MaterialPtr ref_mat_empty_;
  vector <Ogre::GpuProgramParametersSharedPtr> gpu_fp_params_;
  vector <Ogre::GpuProgramParametersSharedPtr> gpu_vp_params_;
};

//----------------------------------------------------------------------------------------------------------------------
Compositor::Compositor() {
  effects_["bloom"] = false;
  effects_["ssao"] = false;
  effects_["motion"] = false;
  scene_ = Ogre::Root::getSingleton().getSceneManager("Default");
  camera_ = scene_->getCamera("Default");
  viewport_ = camera_->getViewport();
}

//----------------------------------------------------------------------------------------------------------------------
Compositor::~Compositor() {

}

//----------------------------------------------------------------------------------------------------------------------
void Compositor::Update(float time) {
  if (effects_["motion"]) {
	mvp_prev_ = mvp_;
	mvp_ = camera_->getProjectionMatrixWithRSDepth()*camera_->getViewMatrix();
	gbuff_handler_->Update(mvp_prev_, time);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Compositor::Init() {
  auto &compositor_manager = Ogre::CompositorManager::getSingleton();

  if (compositor_manager.addCompositor(viewport_, "Main"))
	compositor_manager.setCompositorEnabled(viewport_, "Main", false);
  else
	Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Main compositor\n");

  auto *compositor_chain = compositor_manager.getCompositorChain(viewport_);
  bool fullscreen = conf_->Get<bool>("window_fullscreen");

  if (fullscreen) {
	auto *main_compositor = compositor_chain->getCompositor("Main");
	auto *td = main_compositor->getTechnique()->getTextureDefinition("scene");
	td->width = conf_->Get<int>("window_width");
	td->height = conf_->Get<int>("window_high");
  }

  int new_width = conf_->Get<int>("window_width");
  int new_height = conf_->Get<int>("window_high");
  bool resolution_changed = new_width!=viewport_->getActualWidth() || new_height!=viewport_->getActualHeight();

  compositor_manager.setCompositorEnabled(viewport_, "Main", true);

  if (effects_["ssao"] || effects_["motion"] || !gbuff_handler_) {
	  gbuff_handler_ = make_unique <GBufferSchemeHandler>();
	Ogre::MaterialManager::getSingleton().addListener(gbuff_handler_.get(), "GBuffer");
  }

  if (effects_["ssao"] || effects_["motion"]) {
	if (compositor_manager.addCompositor(viewport_, "GBuffer"))
	  compositor_manager.setCompositorEnabled(viewport_, "GBuffer", false);
	else
	  Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add ShadowReceiver compositor\n");

	if (fullscreen) {
	  auto *gbuf_compositor = compositor_chain->getCompositor("GBuffer");
	  auto *td = gbuf_compositor->getTechnique()->getTextureDefinition("mrt");
	  td->width = new_width/2;
	  td->height = new_height/2;
	}

	compositor_manager.setCompositorEnabled(viewport_, "GBuffer", true);
  }

  if (effects_["bloom"]) {
	if (compositor_manager.addCompositor(viewport_, "Bloom"))
	  compositor_manager.setCompositorEnabled(viewport_, "Bloom", false);
	else
	  Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Bloom compositor\n");

	if (fullscreen) {
	  auto *bloom_compositor = compositor_chain->getCompositor("Bloom");
	  auto *td = bloom_compositor->getTechnique()->getTextureDefinition("bloom");
	  td->width = new_width/4;
	  td->height = new_height/4;
	}

	compositor_manager.setCompositorEnabled(viewport_, "Bloom", true);

	if (compositor_manager.addCompositor(viewport_, "FilterY/Bloom"))
	  compositor_manager.setCompositorEnabled(viewport_, "FilterY/Bloom", true);
	else
	  Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Filter compositor\n");

	if (compositor_manager.addCompositor(viewport_, "FilterX/Bloom"))
	  compositor_manager.setCompositorEnabled(viewport_, "FilterX/Bloom", true);
	else
	  Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Filter compositor\n");
  }

  if (effects_["ssao"]) {
	if (compositor_manager.addCompositor(viewport_, "Ssao"))
	  compositor_manager.setCompositorEnabled(viewport_, "Ssao", true);
	else
	  Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Filter compositor\n");

	if (compositor_manager.addCompositor(viewport_, "Filter/Ssao"))
	  compositor_manager.setCompositorEnabled(viewport_, "Filter/Ssao", true);
	else
	  Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Filter compositor\n");
  }

  if (effects_["ssao"] && !effects_["bloom"]) {
	if (compositor_manager.addCompositor(viewport_, "Modulate/Ssao"))
	  compositor_manager.setCompositorEnabled(viewport_, "Modulate/Ssao", true);
	else
	  Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Modulate compositor\n");
  } else if (effects_["bloom"] && !effects_["ssao"]) {
	if (compositor_manager.addCompositor(viewport_, "Modulate/Bloom"))
	  compositor_manager.setCompositorEnabled(viewport_, "Modulate/Bloom", true);
	else
	  Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Modulate compositor\n");
  } else if (effects_["bloom"] && effects_["ssao"]) {
	if (compositor_manager.addCompositor(viewport_, "Modulate/SsaoBlur"))
	  compositor_manager.setCompositorEnabled(viewport_, "Modulate/SsaoBlur", true);
	else
	  Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Modulate compositor\n");
  }

  if (effects_["motion"]) {
	if (compositor_manager.addCompositor(viewport_, "MotionBlur"))
	  compositor_manager.setCompositorEnabled(viewport_, "MotionBlur", true);
	else
	  Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Modulate compositor\n");
  }

  if (compositor_manager.addCompositor(viewport_, "Output"))
	compositor_manager.setCompositorEnabled(viewport_, "Output", true);
  else
	Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add ShadowReceiver compositor\n");
}

} //namespace
