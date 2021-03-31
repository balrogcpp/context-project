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
#include "Compositor.h"
#include "PbrShaderUtils.h"

using namespace std;

namespace xio {

class GBufferSchemeHandler : public Ogre::MaterialManager::Listener {
 public:

  GBufferSchemeHandler() {
	ref_mat_ = Ogre::MaterialManager::getSingleton().getByName("gbuffer");

	if (!ref_mat_)
	  throw Exception("No available materials for Compositor::GBufferSchemeHandler");

	ref_mat_->load();
  }

//----------------------------------------------------------------------------------------------------------------------
  Ogre::Technique *handleSchemeNotFound(unsigned short schemeIndex,
										const Ogre::String &schemeName,
										Ogre::Material *originalMaterial,
										unsigned short lodIndex,
										const Ogre::Renderable *rend) override {
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

	gBufferTech->setSchemeName(schemeName);
	Ogre::Pass *gbufPass = gBufferTech->createPass();
	*gbufPass = *ref_mat_->getTechnique(0)->getPass(0);

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
  vector<Ogre::GpuProgramParametersSharedPtr> gpu_fp_params_;
  vector<Ogre::GpuProgramParametersSharedPtr> gpu_vp_params_;
};

//----------------------------------------------------------------------------------------------------------------------
Compositor::Compositor() {
  effects_["bloom"] = false;
  effects_["ssao"] = false;
  effects_["motion"] = false;

  compositor_manager_ = Ogre::CompositorManager::getSingletonPtr();
  scene_ = Ogre::Root::getSingleton().getSceneManager("Default");
  camera_ = scene_->getCamera("Default");
  viewport_ = camera_->getViewport();
}

//----------------------------------------------------------------------------------------------------------------------
Compositor::~Compositor() {

}

//----------------------------------------------------------------------------------------------------------------------
void Compositor::Update(float time) {
#if OGRE_PLATFORM!=OGRE_PLATFORM_ANDROID
  if (Ogre::Root::getSingleton().getRenderSystem()->getName()!="OpenGL ES 2.x Rendering Subsystem")
  	Pbr::Update(time);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void Compositor::EnableEffect(const std::string &name, bool enable) {
  effects_[name] = enable;
}

//----------------------------------------------------------------------------------------------------------------------
void Compositor::Cleanup() {
  Pbr::Cleanup();
}

//----------------------------------------------------------------------------------------------------------------------
void Compositor::Pause() {

}

//----------------------------------------------------------------------------------------------------------------------
void Compositor::Resume() {

}

//----------------------------------------------------------------------------------------------------------------------
void Compositor::InitGbuffer_() {
    if (!gbuff_handler_) {
	gbuff_handler_ = make_unique<GBufferSchemeHandler>();
	Ogre::MaterialManager::getSingleton().addListener(gbuff_handler_.get(), "GBuffer");
  }

  if (compositor_manager_->addCompositor(viewport_.get(), "GBuffer"))
	compositor_manager_->setCompositorEnabled(viewport_.get(), "GBuffer", true);
  else
	Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add GBuffer compositor\n");
}

//----------------------------------------------------------------------------------------------------------------------
void Compositor::AddCompositor_(const std::string &name) {
  if (compositor_manager_->addCompositor(viewport_.get(), name))
	compositor_manager_->setCompositorEnabled(viewport_.get(), name, true);
  else
    throw Exception(string("Failed to add ") + name + " compositor");
}

//----------------------------------------------------------------------------------------------------------------------
void Compositor::InitMRT_() {
  if (compositor_manager_->addCompositor(viewport_.get(), "MRT"))
	compositor_manager_->setCompositorEnabled(viewport_.get(), "MRT", false);
  else
	Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add MRT compositor\n");

  auto *compositor_chain = compositor_manager_->getCompositorChain(viewport_.get());
  bool fullscreen = conf_->Get<bool>("window_fullscreen");

  if (fullscreen) {
	auto *main_compositor = compositor_chain->getCompositor("MRT");
	auto *td = main_compositor->getTechnique()->getTextureDefinition("mrt");
	td->width = conf_->Get<int>("window_width");
	td->height = conf_->Get<int>("window_high");
  }

  compositor_manager_->setCompositorEnabled(viewport_.get(), "MRT", true);
}

//----------------------------------------------------------------------------------------------------------------------
void Compositor::InitOutput_() {
  if (effects_["ssao"]) {
	if (compositor_manager_->addCompositor(viewport_.get(), "OutputSSAO"))
	  compositor_manager_->setCompositorEnabled(viewport_.get(), "OutputSSAO", false);
	else
	  Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add OutputSSAO compositor\n");

	auto params = Ogre::MaterialManager::getSingleton().getByName("OutputSSAO")->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
	params->setNamedConstant("uMotionBlueEnable", 1.0f);
	compositor_manager_->setCompositorEnabled(viewport_.get(), "OutputSSAO", true);
  } else {

	if (compositor_manager_->addCompositor(viewport_.get(), "Output"))
	  compositor_manager_->setCompositorEnabled(viewport_.get(), "Output", false);

	else
	  Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add Output compositor\n");

	auto params = Ogre::MaterialManager::getSingleton().getByName("Output")->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
	params->setNamedConstant("uMotionBlueEnable", 1.0f);
	compositor_manager_->setCompositorEnabled(viewport_.get(), "Output", true);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Compositor::Init() {
  if (Ogre::Root::getSingleton().getRenderSystem()->getName()=="OpenGL ES 2.x Rendering Subsystem")
	return;
#if OGRE_PLATFORM==OGRE_PLATFORM_ANDROID
  return;
#endif

  InitMRT_();

  if (effects_["ssao"]) {
    AddCompositor_("SSAO");
    AddCompositor_("BoxFilter/SSAO");
  }

  InitOutput_();
}

} //namespace
