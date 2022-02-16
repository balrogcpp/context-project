// This source file is part of "glue project". Created by Andrey Vasiliev

#include "PCHeader.h"
#include "Components/Compositor.h"
#include "Config.h"
#include "ShaderHelpers.h"

using namespace std;
using namespace Ogre;

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

  Ogre::Technique *handleSchemeNotFound(unsigned short schemeIndex, const Ogre::String &schemeName, Ogre::Material *originalMaterial,
                                        unsigned short lodIndex, const Ogre::Renderable *rend) override {
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
  std::vector<Ogre::GpuProgramParametersSharedPtr> gpu_fp_params_;
  std::vector<Ogre::GpuProgramParametersSharedPtr> gpu_vp_params_;
};

Compositor::Compositor() {
  EffectsList["bloom"] = false;
  EffectsList["ssao"] = false;
  EffectsList["mblur"] = false;

  OgreCompositorManager = Ogre::CompositorManager::getSingletonPtr();
  OgreSceneManager = Ogre::Root::getSingleton().getSceneManager("Default");
  OgreCamera = OgreSceneManager->getCamera("Default");
  OgreViewport = OgreCamera->getViewport();
  OgreCompositorChain = OgreCompositorManager->getCompositorChain(OgreViewport);

  EnableEffect("ssao", Config::GetInstance().GetBool("enable_ssao"));
  EnableEffect("bloom", Config::GetInstance().GetBool("enable_bloom"));
  EnableEffect("mblur", Config::GetInstance().GetBool("enable_mblur"));

  SetUp();
}

Compositor::~Compositor() {}

void Compositor::OnUpdate(float time) {}

void Compositor::EnableEffect(const std::string &name, bool enable) { EffectsList[name] = enable; }

void Compositor::OnClean() {}

void Compositor::OnPause() {}

void Compositor::OnResume() {}

void Compositor::InitGBuffer() {
  if (!GBufferHandler) {
    GBufferHandler = make_unique<GBufferSchemeHandler>();
    Ogre::MaterialManager::getSingleton().addListener(GBufferHandler.get(), "GBuffer");
  }

  if (OgreCompositorManager->addCompositor(OgreViewport, "GBuffer")) {
    OgreCompositorManager->setCompositorEnabled(OgreViewport, "GBuffer", true);
  } else {
    Ogre::LogManager::getSingleton().logMessage("Failed to add GBuffer compositor\n");
  }
}

void Compositor::AddCompositorEnabled(const std::string &name) {
  if (OgreCompositorManager->addCompositor(OgreViewport, name)) {
    OgreCompositorManager->setCompositorEnabled(OgreViewport, name, true);
  } else {
    throw Exception(string("Failed to add ") + name + " compositor");
  }
}

void Compositor::AddCompositorDisabled(const std::string &name) {
  if (OgreCompositorManager->addCompositor(OgreViewport, name)) {
    OgreCompositorManager->setCompositorEnabled(OgreViewport, name, false);
  } else {
    throw Exception(string("Failed to add ") + name + " compositor");
  }
}

void Compositor::EnableCompositor(const std::string &name) { OgreCompositorManager->setCompositorEnabled(OgreViewport, name, true); }

void Compositor::InitMRT() {
  string MRT;
  if (UseMRT)
    MRT = "MRT";
  else
    MRT = "noMRT";

  if (OgreCompositorManager->addCompositor(OgreViewport, MRT)) {
    OgreCompositorManager->setCompositorEnabled(OgreViewport, MRT, false);
  } else {
    Throw("Failed to add MRT compositor");
  }

#ifdef MOBILE
  auto *MRTCompositor = OgreCompositorChain->getCompositor(MRT);
  auto *MRTTextureDefinition = MRTCompositor->getTechnique()->getTextureDefinition("mrt");
  MRTTextureDefinition->width = 1024;
  MRTTextureDefinition->height = 768;
#endif

  OgreCompositorManager->setCompositorEnabled(OgreViewport, MRT, true);
}

void Compositor::InitDummyOutput() { AddCompositorEnabled("OutputDummy"); }

void Compositor::InitOutput() {
  string OutputCompositor;

  if (EffectsList["mblur"])
    OutputCompositor = "Output";
  else
    OutputCompositor = "OutputFinal";

  const string MotionBlurCompositor = "MBlur";

  auto &material_manager = Ogre::MaterialManager::getSingleton();
  auto material = material_manager.getByName(OutputCompositor);
  auto *pass = material->getTechnique(0)->getPass(0);
  auto fs_params = pass->getFragmentProgramParameters();

  AddCompositorDisabled(OutputCompositor);

  if (EffectsList["ssao"]) {
    auto texture = pass->getTextureUnitState("SSAO");
    texture->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
    texture->setCompositorReference("SSAO", "ssao");
    fs_params->setNamedConstant("uSSAOEnable", 1.0f);
  }

  if (EffectsList["bloom"]) {
    auto texture = pass->getTextureUnitState("Bloom");
    texture->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
    texture->setCompositorReference("Bloom", "bloom");
    fs_params->setNamedConstant("uBloomEnable", 1.0f);
  }

  if (EffectsList["mblur"]) {
    auto material = material_manager.getByName(MotionBlurCompositor);
    auto *pass = material->getTechnique(0)->getPass(0);
    auto fs_params = pass->getFragmentProgramParameters();

    fs_params->setNamedConstant("uMotionBlurEnable", 1.0f);
  }

  EnableCompositor(OutputCompositor);
  if (EffectsList["mblur"]) AddCompositorEnabled("MBlur");
}

void Compositor::SetUp() {
  for (const auto &it : EffectsList) {
    if (it.second) {
      AnyEffectEnabled = true;
      break;
    }
  }

  if (Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL ES 2.x Rendering Subsystem" && !AnyEffectEnabled) UseMRT = false;

  InitMRT();

  // nothing to do without MTR
  if (!UseMRT) {
    InitDummyOutput();
    return;
  }

  if (EffectsList["ssao"]) {
    AddCompositorEnabled("SSAO");
    AddCompositorEnabled("BoxFilter/SSAO");
  }

  if (EffectsList["bloom"]) {
    AddCompositorEnabled("Bloom");
    AddCompositorEnabled("FilterX/Bloom");
    AddCompositorEnabled("FilterY/Bloom");
  }

  InitOutput();
}

}  // namespace Glue
