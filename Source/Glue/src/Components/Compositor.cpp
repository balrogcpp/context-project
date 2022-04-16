// This source file is part of Glue Engine. Created by Andrey Vasiliev

#include "PCHeader.h"
#include "Components/Compositor.h"
#include "Config.h"
#include "ShaderHelpers.h"

using namespace std;
using namespace Ogre;

namespace Glue {

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

void Compositor::EnableEffect(const string &name, bool enable) { EffectsList[name] = enable; }

void Compositor::OnClean() {}

void Compositor::OnPause() {}

void Compositor::OnResume() {}

void Compositor::AddCompositorEnabled(const string &name) {
  if (OgreCompositorManager->addCompositor(OgreViewport, name))
    OgreCompositorManager->setCompositorEnabled(OgreViewport, name, true);
  else
    throw Exception(string("Failed to add ") + name + " compositor");
}

void Compositor::AddCompositorDisabled(const string &name) {
  if (OgreCompositorManager->addCompositor(OgreViewport, name))
    OgreCompositorManager->setCompositorEnabled(OgreViewport, name, false);
  else
    throw Exception(string("Failed to add ") + name + " compositor");
}

void Compositor::EnableCompositor(const string &name) { OgreCompositorManager->setCompositorEnabled(OgreViewport, name, true); }

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
    MRTTextureDefinition->width = 1280;
    MRTTextureDefinition->height = 720;
#endif
  }

  OgreCompositorManager->setCompositorEnabled(OgreViewport, MRT, true);
}

void Compositor::InitDummyOutput() { AddCompositorEnabled("OutputDummy"); }

void Compositor::InitOutput() {
  string OutputCompositor;

  if (EffectsList["mblur"])
    OutputCompositor = "Output";
  else
    OutputCompositor = "OutputFinal";

  const string MotionBlurCompositor = "Blur";
  auto &material_manager = Ogre::MaterialManager::getSingleton();
  auto material = material_manager.getByName(OutputCompositor);
  auto *pass = material->getTechnique(0)->getPass(0);
  auto OutputParamsPtr = pass->getFragmentProgramParameters();

  AddCompositorDisabled(OutputCompositor);

  if (EffectsList["ssao"]) {
    auto texture = pass->getTextureUnitState("SSAO");
    texture->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
    texture->setCompositorReference("SSAO", "ssao");
    OutputParamsPtr->setNamedConstant("uSSAOEnable", 1.0f);
  }

  if (EffectsList["bloom"]) {
    auto texture = pass->getTextureUnitState("Bloom");
    texture->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
    texture->setCompositorReference("Bloom", "bloom");
    OutputParamsPtr->setNamedConstant("uBloomEnable", 1.0f);
  }

  if (EffectsList["mblur"]) {
    auto material = material_manager.getByName(MotionBlurCompositor);
    auto *pass = material->getTechnique(0)->getPass(0);
    auto fs_params = pass->getFragmentProgramParameters();
    fs_params->setNamedConstant("uMotionBlurEnable", 1.0f);
  }

  EnableCompositor(OutputCompositor);
  if (EffectsList["mblur"]) AddCompositorEnabled("Blur");
}

void Compositor::SetUp() {
  AnyEffectEnabled = false;
  for (const auto &it : EffectsList) {
    if (it.second) {
      AnyEffectEnabled = true;
      break;
    }
  }

  InitMRT();

  // nothing to do without MTR
  if (!GlobalMRTIsEnabled()) {
    InitDummyOutput();
    return;
  }

  if (EffectsList["ssao"]) {
    AddCompositorEnabled("SSAO");
  }

  if (EffectsList["bloom"]) {
    AddCompositorEnabled("Bloom");
    AddCompositorEnabled("Bloom/FilterX");
    AddCompositorEnabled("Bloom/FilterY");
  }

  InitOutput();
}

}  // namespace Glue
