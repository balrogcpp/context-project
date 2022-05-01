// This source file is part of Glue Engine. Created by Andrey Vasiliev

#include "PCHeader.h"
#include "Components/Compositor.h"
#include "Config.h"

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
  EnableEffect(FX_FXAA, Config::GetInstance().GetBool("enable_fxaa"));
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
  string MRTCompositor;

  if (GlobalMRTIsEnabled())
    MRTCompositor = "MRT";
  else
    MRTCompositor = "noMRT";

  OgreAssert(OgreCompositorManager->addCompositor(OgreViewport, MRTCompositor, 0), "Failed to add MRT compositor");
  OgreCompositorManager->setCompositorEnabled(OgreViewport, MRTCompositor, false);

  if (IsFullscreen()) {
    auto *MRTCompositorPtr = OgreCompositorChain->getCompositor(MRTCompositor);
    auto *MRTTexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("mrt");
    auto *SSAOTexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("ssao");
    auto *FinalTexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("final");
    OgreAssert(MRTTexturePtr, "MRTCompositor texture not created");
    OgreAssert(SSAOTexturePtr, "SSAO texture not created");
    OgreAssert(FinalTexturePtr, "Final texture not created");
#ifdef MOBILE
    MRTTexturePtr->width = 1024;
    MRTTexturePtr->height = 768;
    FinalTexturePtr->width = MRTTexturePtr->width;
    FinalTexturePtr->height = MRTTexturePtr->height;
    SSAOTexturePtr->width = MRTTexturePtr->width;
    SSAOTexturePtr->height = MRTTexturePtr->height;
#else
    MRTTexturePtr->width = 1360;
    MRTTexturePtr->height = 768;
    SSAOTexturePtr->width = MRTTexturePtr->width;
    SSAOTexturePtr->height = MRTTexturePtr->height;
    FinalTexturePtr->width = MRTTexturePtr->width;
    FinalTexturePtr->height = MRTTexturePtr->height;
#endif
  }

  if (GlobalMRTIsEnabled()) {
    if (CompositorList[FX_SSAO])
      for (const auto &it : SSAOCompositorChain) GetFPparameters(it)->setNamedConstant(SSAOEnable, 1.0f);
    if (CompositorList[FX_SSAO])
      for (const auto &it : BloomCompositorChain) GetFPparameters(it)->setNamedConstant(BloomEnable, 1.0f);
    if (CompositorList[FX_BLUR])
      for (const auto &it : BlurCompositorChain) GetFPparameters(it)->setNamedConstant(BlurEnable, 1.0f);
  }

  OgreCompositorManager->setCompositorEnabled(OgreViewport, MRTCompositor, true);
}

}  // namespace Glue
