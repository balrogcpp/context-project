// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Compositor.h"
#include "Config.h"
#include "PbrShaderUtils.h"

using namespace std;

namespace Glue {

class GBufferSchemeHandler : public Ogre::MaterialManager::Listener {
 public:
  GBufferSchemeHandler() {
    ref_mat_ = Ogre::MaterialManager::getSingleton().getByName("gbuffer");

    if (!ref_mat_) {
      throw Exception("No available materials for Compositor::GBufferSchemeHandler");
    }

    ref_mat_->load();
  }

  Ogre::Technique *handleSchemeNotFound(unsigned short schemeIndex, const Ogre::String &schemeName,
                                        Ogre::Material *originalMaterial, unsigned short lodIndex,
                                        const Ogre::Renderable *rend) override {
    Ogre::Technique *gBufferTech = originalMaterial->createTechnique();

    gBufferTech->setSchemeName(schemeName);
    Ogre::Pass *gbufPass = gBufferTech->createPass();
    *gbufPass = *ref_mat_->getTechnique(0)->getPass(0);

    return gBufferTech;
  }

  void Update(Ogre::Matrix4 mvp_prev, float time) {
    for (auto &it : gpu_fp_params_) {
      it->setNamedConstant("uFrameTime", time);
    }

    for (auto &it : gpu_vp_params_) {
      it->setNamedConstant("cWorldViewProjPrev", mvp_prev);
    }
  }

 protected:
  Ogre::MaterialPtr ref_mat_;
  vector<Ogre::GpuProgramParametersSharedPtr> gpu_fp_params_;
  vector<Ogre::GpuProgramParametersSharedPtr> gpu_vp_params_;
};

Compositor::Compositor() {
  effects["bloom"] = false;
  effects["ssao"] = false;
  effects["mblur"] = false;

  compositor_manager = Ogre::CompositorManager::getSingletonPtr();
  scene = Ogre::Root::getSingleton().getSceneManager("Default");
  camera = scene->getCamera("Default");
  viewport = camera->getViewport();

  EnableEffect("ssao", config->Get<bool>("enable_ssao"));
  EnableEffect("bloom", config->Get<bool>("enable_bloom"));
  EnableEffect("mblur", config->Get<bool>("enable_mblur"));
  SetUp();
}

Compositor::~Compositor() {}

void Compositor::Update(float time) {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  static bool gl3 = Ogre::Root::getSingleton().getRenderSystem()->getName() != "OpenGL ES 2.x Rendering Subsystem";
  static bool mblur = effects["mblur"];

  if (gl3 && mblur) {
    Pbr::Update(time);
  }
#endif
}

void Compositor::EnableEffect(const std::string &name, bool enable) { effects[name] = enable; }

void Compositor::Cleanup() { Pbr::Cleanup(); }

void Compositor::Pause() {}

void Compositor::Resume() {}

void Compositor::InitGbuffer() {
  if (!gbuff_handler) {
    gbuff_handler = make_unique<GBufferSchemeHandler>();
    Ogre::MaterialManager::getSingleton().addListener(gbuff_handler.get(), "GBuffer");
  }

  if (compositor_manager->addCompositor(viewport, "GBuffer")) {
    compositor_manager->setCompositorEnabled(viewport, "GBuffer", true);
  } else {
    Ogre::LogManager::getSingleton().logMessage("Failed to add GBuffer compositor\n");
  }
}

void Compositor::AddCompositorEnabled(const std::string &name) {
  if (compositor_manager->addCompositor(viewport, name)) {
    compositor_manager->setCompositorEnabled(viewport, name, true);
  } else {
    throw Exception(string("Failed to add ") + name + " compositor");
  }
}

void Compositor::AddCompositorDisabled(const std::string &name) {
  if (compositor_manager->addCompositor(viewport, name)) {
    compositor_manager->setCompositorEnabled(viewport, name, false);
  } else {
    throw Exception(string("Failed to add ") + name + " compositor");
  }
}

void Compositor::EnableCompositor(const std::string &name) {
  compositor_manager->setCompositorEnabled(viewport, name, true);
}

void Compositor::InitMRT() {
  if (compositor_manager->addCompositor(viewport, "MRT")) {
    compositor_manager->setCompositorEnabled(viewport, "MRT", false);
  } else {
    Ogre::LogManager::getSingleton().logMessage("Context core:: Failed to add MRT compositor\n");
  }

  auto *compositor_chain = compositor_manager->getCompositorChain(viewport);
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID

  if (config->Get<bool>("window_fullscreen")) {
    auto *main_compositor = compositor_chain->getCompositor("MRT");
    auto *td = main_compositor->getTechnique()->getTextureDefinition("mrt");
    td->width = config->Get<int>("window_width");
    td->height = config->Get<int>("window_high");
  }

#else

  auto *main_compositor = compositor_chain->getCompositor("MRT");
  auto *td = main_compositor->getTechnique()->getTextureDefinition("mrt");
  td->height = viewport->getActualHeight() * 0.75;
  td->width = viewport->getActualWidth() * 0.75;

#endif

  compositor_manager->setCompositorEnabled(viewport, "MRT", true);
}

void Compositor::InitOutput() {
  string output_compositor;
  string mblur_compositor;

  output_compositor = "Output";
  mblur_compositor = "MBlur";

  AddCompositorDisabled(output_compositor);

  if (effects["ssao"]) {
    auto &material_manager = Ogre::MaterialManager::getSingleton();
    auto material = material_manager.getByName(output_compositor);
    auto *pass = material->getTechnique(0)->getPass(0);
    auto fs_params = pass->getFragmentProgramParameters();

    auto texture = pass->getTextureUnitState("SSAO");
    texture->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
    texture->setCompositorReference("SSAO", "ssao");
    fs_params->setNamedConstant("uSSAOEnable", 1.0f);
  }

  if (effects["bloom"]) {
    auto &material_manager = Ogre::MaterialManager::getSingleton();
    auto material = material_manager.getByName(output_compositor);
    auto *pass = material->getTechnique(0)->getPass(0);
    auto fs_params = pass->getFragmentProgramParameters();

    auto texture = pass->getTextureUnitState("Bloom");
    texture->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
    texture->setCompositorReference("Bloom", "bloom");
    fs_params->setNamedConstant("uBloomEnable", 1.0f);
  }

  EnableCompositor(output_compositor);

  auto &material_manager = Ogre::MaterialManager::getSingleton();
  auto material = material_manager.getByName(mblur_compositor);
  auto *pass = material->getTechnique(0)->getPass(0);
  auto fs_params = pass->getFragmentProgramParameters();

  if (effects["mblur"]) {
    fs_params->setNamedConstant("uMotionBlurEnable", 1.0f);
  }

  AddCompositorEnabled("MBlur");
}

void Compositor::SetUp() {
  InitMRT();

  if (effects["ssao"]) {
    AddCompositorEnabled("SSAO");
    AddCompositorEnabled("BoxFilter/SSAO");
  }

  if (effects["bloom"]) {
    AddCompositorEnabled("Bloom");
    AddCompositorEnabled("FilterX/Bloom");
    AddCompositorEnabled("FilterY/Bloom");
  }

  InitOutput();
}

}  // namespace glue
