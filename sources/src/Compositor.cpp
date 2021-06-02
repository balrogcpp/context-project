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

#include "Compositor.h"

#include "Config.h"
#include "PbrShaderUtils.h"
#include "pcheader.h"

using namespace std;

namespace xio {

//----------------------------------------------------------------------------------------------------------------------
class GBufferSchemeHandler : public Ogre::MaterialManager::Listener {
 public:
  GBufferSchemeHandler() {
    ref_mat_ = Ogre::MaterialManager::getSingleton().getByName("gbuffer");

    if (!ref_mat_) {
      throw Exception(
          "No available materials for Compositor::GBufferSchemeHandler");
    }

    ref_mat_->load();
  }

  //----------------------------------------------------------------------------------------------------------------------
  Ogre::Technique *handleSchemeNotFound(unsigned short schemeIndex,
                                        const Ogre::String &schemeName,
                                        Ogre::Material *originalMaterial,
                                        unsigned short lodIndex,
                                        const Ogre::Renderable *rend) override {
    Ogre::Technique *gBufferTech = originalMaterial->createTechnique();

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
  effects_["mblur"] = false;

  compositor_manager_ = Ogre::CompositorManager::getSingletonPtr();
  scene_ = Ogre::Root::getSingleton().getSceneManager("Default");
  camera_ = scene_->getCamera("Default");
  viewport_ = camera_->getViewport();

  EnableEffect("ssao", conf_->Get<bool>("enable_ssao"));
  EnableEffect("bloom", conf_->Get<bool>("enable_bloom"));
  EnableEffect("mblur", conf_->Get<bool>("enable_mblur"));
  Init();
}

//----------------------------------------------------------------------------------------------------------------------
Compositor::~Compositor() {}

//----------------------------------------------------------------------------------------------------------------------
void Compositor::Update(float time) {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  static bool gl3 = Ogre::Root::getSingleton().getRenderSystem()->getName() !=
                    "OpenGL ES 2.x Rendering Subsystem";
  static bool mblur = effects_["mblur"];

  if (gl3 && mblur) {
    Pbr::Update(time);
  }
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void Compositor::EnableEffect(const std::string &name, bool enable) {
  effects_[name] = enable;
}

//----------------------------------------------------------------------------------------------------------------------
void Compositor::Cleanup() { Pbr::Cleanup(); }

//----------------------------------------------------------------------------------------------------------------------
void Compositor::Pause() {}

//----------------------------------------------------------------------------------------------------------------------
void Compositor::Resume() {}

//----------------------------------------------------------------------------------------------------------------------
void Compositor::InitGbuffer_() {
  if (!gbuff_handler_) {
    gbuff_handler_ = make_unique<GBufferSchemeHandler>();
    Ogre::MaterialManager::getSingleton().addListener(gbuff_handler_.get(),
                                                      "GBuffer");
  }

  if (compositor_manager_->addCompositor(viewport_.get(), "GBuffer")) {
    compositor_manager_->setCompositorEnabled(viewport_.get(), "GBuffer", true);
  } else {
    Ogre::LogManager::getSingleton().logMessage(
        "Failed to add GBuffer compositor\n");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Compositor::AddCompositorEnabled_(const std::string &name) {
  if (compositor_manager_->addCompositor(viewport_.get(), name)) {
    compositor_manager_->setCompositorEnabled(viewport_.get(), name, true);
  } else {
    throw Exception(string("Failed to add ") + name + " compositor");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Compositor::AddCompositorDisabled_(const std::string &name) {
  if (compositor_manager_->addCompositor(viewport_.get(), name)) {
    compositor_manager_->setCompositorEnabled(viewport_.get(), name, false);
  } else {
    throw Exception(string("Failed to add ") + name + " compositor");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Compositor::EnableCompositor_(const std::string &name) {
  compositor_manager_->setCompositorEnabled(viewport_.get(), name, true);
}

//----------------------------------------------------------------------------------------------------------------------
void Compositor::InitMRT_() {
  if (compositor_manager_->addCompositor(viewport_.get(), "MRT")) {
    compositor_manager_->setCompositorEnabled(viewport_.get(), "MRT", false);
  } else {
    Ogre::LogManager::getSingleton().logMessage(
        "Context core:: Failed to add MRT compositor\n");
  }

  auto *compositor_chain =
      compositor_manager_->getCompositorChain(viewport_.get());
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID

  if (conf_->Get<bool>("window_fullscreen")) {
    auto *main_compositor = compositor_chain->getCompositor("MRT");
    auto *td = main_compositor->getTechnique()->getTextureDefinition("mrt");
    td->width = conf_->Get<int>("window_width");
    td->height = conf_->Get<int>("window_high");
  }

#else

  auto *main_compositor = compositor_chain->getCompositor("MRT");
  auto *td = main_compositor->getTechnique()->getTextureDefinition("mrt");
  td->height = viewport_->getActualHeight() * 0.75;
  td->width = viewport_->getActualWidth() * 0.75;

#endif

  compositor_manager_->setCompositorEnabled(viewport_.get(), "MRT", true);
}

//----------------------------------------------------------------------------------------------------------------------
void Compositor::InitOutput_() {
  string output_compositor;
  string mblur_compositor;

  output_compositor = "Output";
  mblur_compositor = "MBlur";

  AddCompositorDisabled_(output_compositor);

  if (effects_["ssao"]) {
    auto &material_manager = Ogre::MaterialManager::getSingleton();
    auto material = material_manager.getByName(output_compositor);
    auto *pass = material->getTechnique(0)->getPass(0);
    auto fs_params = pass->getFragmentProgramParameters();

    auto texture = pass->getTextureUnitState("SSAO");
    texture->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
    texture->setCompositorReference("SSAO", "ssao");
    fs_params->setNamedConstant("uSSAOEnable", 1.0f);
  }

  if (effects_["bloom"]) {
    auto &material_manager = Ogre::MaterialManager::getSingleton();
    auto material = material_manager.getByName(output_compositor);
    auto *pass = material->getTechnique(0)->getPass(0);
    auto fs_params = pass->getFragmentProgramParameters();

    auto texture = pass->getTextureUnitState("Bloom");
    texture->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
    texture->setCompositorReference("Bloom", "bloom");
    fs_params->setNamedConstant("uBloomEnable", 1.0f);
  }

  EnableCompositor_(output_compositor);

  auto &material_manager = Ogre::MaterialManager::getSingleton();
  auto material = material_manager.getByName(mblur_compositor);
  auto *pass = material->getTechnique(0)->getPass(0);
  auto fs_params = pass->getFragmentProgramParameters();

  if (effects_["mblur"]) {
    fs_params->setNamedConstant("uMotionBlurEnable", 1.0f);
  }

  AddCompositorEnabled_("MBlur");
}

//----------------------------------------------------------------------------------------------------------------------
void Compositor::Init() {
  InitMRT_();

  if (effects_["ssao"]) {
    AddCompositorEnabled_("SSAO");
    AddCompositorEnabled_("BoxFilter/SSAO");
  }

  if (effects_["bloom"]) {
    AddCompositorEnabled_("Bloom");
    AddCompositorEnabled_("FilterX/Bloom");
    AddCompositorEnabled_("FilterY/Bloom");
  }

  InitOutput_();
}

}  // namespace xio
