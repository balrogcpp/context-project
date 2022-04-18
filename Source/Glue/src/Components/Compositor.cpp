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

  EnableEffect(FX_SSAO, Config::GetInstance().GetBool("enable_ssao"));
  EnableEffect(FX_BLOOM, Config::GetInstance().GetBool("enable_bloom"));
  EnableEffect(FX_BLUR, Config::GetInstance().GetBool("enable_mblur"));

  InitMRT();
}

Compositor::~Compositor() {}

void Compositor::OnUpdate(float time) {}

void Compositor::EnableEffect(const Compositors FX, bool Enable) { CompositorList[FX] = Enable; }

void Compositor::OnClean() {}

void Compositor::OnPause() {}

void Compositor::OnResume() {}

void Compositor::AddCompositorEnabled(const string &Name) {
  OgreAssert(OgreCompositorManager->addCompositor(OgreViewport, Name), "Failed to add MRT compoitor");
  OgreCompositorManager->setCompositorEnabled(OgreViewport, Name, true);
}

void Compositor::AddCompositorDisabled(const string &Name) {
  OgreAssert(OgreCompositorManager->addCompositor(OgreViewport, Name), "Failed to add MRT compoitor");
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

  OgreAssert(OgreCompositorManager->addCompositor(OgreViewport, MRT), "Failed to add MRT compoitor");

  if (IsFullscreen()) {
    auto *MRTCompositor = OgreCompositorChain->getCompositor(MRT);
    auto *MRTTextureDefinition = MRTCompositor->getTechnique()->getTextureDefinition("mrt");
    OgreAssert(MRTTextureDefinition, "MRT texture not created");
#ifdef MOBILE
    MRTTextureDefinition->width = 1024;
    MRTTextureDefinition->height = 768;
#else
    MRTTextureDefinition->width = 1360;
    MRTTextureDefinition->height = 768;
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
