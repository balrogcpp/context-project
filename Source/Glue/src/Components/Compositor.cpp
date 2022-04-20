// This source file is part of Glue Engine. Created by Andrey Vasiliev

#include "PCHeader.h"
#include "Components/Compositor.h"
#include "Config.h"
#include "ShaderHelpers.h"

using namespace std;
using namespace Ogre;

namespace Glue {

Compositor::Compositor() {
  OgreCompositorManager = Ogre::CompositorManager::getSingletonPtr();
  OgreSceneManager = Ogre::Root::getSingleton().getSceneManager("Default");
  OgreCamera = OgreSceneManager->getCamera("Default");
  OgreViewport = OgreCamera->getViewport();
  OgreCompositorChain = OgreCompositorManager->getCompositorChain(OgreViewport);

#ifdef MOBILE
  EnableEffect(FX_SSAO, false);
  EnableEffect(FX_BLOOM, false);
  EnableEffect(FX_BLUR, false);
#else
  EnableEffect(FX_SSAO, Config::GetInstance().GetBool("enable_ssao"));
  EnableEffect(FX_BLOOM, Config::GetInstance().GetBool("enable_bloom"));
  EnableEffect(FX_BLUR, Config::GetInstance().GetBool("enable_mblur"));
#endif
}

Compositor::~Compositor() {}

void Compositor::OnUpdate(float time) {}

void Compositor::EnableEffect(const Compositors FX, bool Enable) { CompositorList[FX] = Enable; }

void Compositor::OnSetUp() { InitMRT(); }

void Compositor::OnClean() {}

void Compositor::OnPause() {}

void Compositor::OnResume() {}

void Compositor::AddCompositorEnabled(const string &Name) {
  OgreAssert(OgreCompositorManager->addCompositor(OgreViewport, Name), "Failed to add MRT compoitor");
  OgreCompositorManager->setCompositorEnabled(OgreViewport, Name, true);
}

void Compositor::AddCompositorDisabled(const string &Name) {
  OgreAssert(OgreCompositorManager->addCompositor(OgreViewport, Name), "Failed to add MRT compoitor");
  OgreCompositorManager->setCompositorEnabled(OgreViewport, Name, false);
}

void Compositor::EnableCompositor(const string &Name) { OgreCompositorManager->setCompositorEnabled(OgreViewport, Name, true); }

static GpuProgramParametersSharedPtr GetFPparameters(const string &CompositorName) {
  static auto &MM = Ogre::MaterialManager::getSingleton();
  return MM.getByName(CompositorName)->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
}

void Compositor::InitMRT() {
  string MRT;

  if (GlobalMRTIsEnabled())
    MRT = "MRT";
  else
    MRT = "noMRT";

  OgreAssert(OgreCompositorManager->addCompositor(OgreViewport, MRT, 0), "Failed to add MRT compoitor");
  OgreCompositorManager->setCompositorEnabled(OgreViewport, MRT, false);

  if (IsFullscreen()) {
    auto *MRTCompositor = OgreCompositorChain->getCompositor(MRT);
    auto *MRTTexture = MRTCompositor->getTechnique()->getTextureDefinition("mrt");
    auto *SSAOTexture = MRTCompositor->getTechnique()->getTextureDefinition("ssao");
    OgreAssert(MRTTexture, "MRT texture not created");
#ifdef MOBILE
    MRTTexture->width = 1024;
    MRTTexture->height = 768;
    SSAOTexture->width = 1024;
    SSAOTexture->height = 768;
#else
    MRTTexture->width = 1360;
    MRTTexture->height = 768;
    SSAOTexture->width = 1360;
    SSAOTexture->height = 768;
#endif
  }

  if (GlobalMRTIsEnabled()) {
    if (CompositorList[FX_SSAO]) GetFPparameters(OutputCompositor)->setNamedConstant("uSSAOEnable", 1.0f);
    if (CompositorList[FX_SSAO]) GetFPparameters(SSAOCompositor)->setNamedConstant("uSSAOEnable", 1.0f);
    if (CompositorList[FX_BLOOM]) GetFPparameters(OutputCompositor)->setNamedConstant("uBloomEnable", 1.0f);
    if (CompositorList[FX_BLOOM]) GetFPparameters(BloomCompositor)->setNamedConstant("uBloomEnable", 1.0f);
    if (CompositorList[FX_BLUR]) GetFPparameters(BlurCompositor)->setNamedConstant("uMotionBlurEnable", 1.0f);
  }

  OgreCompositorManager->setCompositorEnabled(OgreViewport, MRT, true);
}

}  // namespace Glue
