// This source file is part of Glue Engine. Created by Andrey Vasiliev

#include "PCHeader.h"
#include "Components/Compositor.h"
#include "Config.h"

using namespace std;
using namespace Ogre;

namespace Glue {

static GpuProgramParametersSharedPtr GetFPparameters(const string &CompositorName) {
  static auto &MM = Ogre::MaterialManager::getSingleton();
  return MM.getByName(CompositorName)->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
}

const static FX Bloom{"Bloom", false, "uBloomEnable", "Output", {"Bloom", "Downscale3x3"}};
const static FX SSAO{"SSAO", false, "uSSAOEnable", "Output", {"SSAO", "FilterX", "FilterY"}};
const static FX Blur{"Blur", false, "uBlurEnable", "Blur", {}};
const static FX FXAA{"FXAA", false, "uFXAAEnable", "FXAA", {}};
const static FX HDR{"HDR", false, "uHDREnable", "Output", {}};
const static std::string FX_SSAO = "SSAO";
const static std::string FX_BLOOM = "Bloom";
const static std::string FX_BLUR = "Blur";
const static std::string FX_FXAA = "FXAA";
const static std::string FX_HDR = "HDR";

Compositor::Compositor() {
  OgreCompositorManager = Ogre::CompositorManager::getSingletonPtr();
  OgreSceneManager = Ogre::Root::getSingleton().getSceneManager("Default");
  OgreCamera = OgreSceneManager->getCamera("Default");
  OgreViewport = OgreCamera->getViewport();
  OgreCompositorChain = OgreCompositorManager->getCompositorChain(OgreViewport);
  CompositorList.insert(make_pair(FX_BLOOM, Bloom));
  CompositorList.insert(make_pair(FX_SSAO, SSAO));
  CompositorList.insert(make_pair(FX_BLUR, Blur));
  CompositorList.insert(make_pair(FX_FXAA, FXAA));
  CompositorList.insert(make_pair(FX_HDR, HDR));
#ifdef MOBILE
  EnableEffect(FX_SSAO, false);
  EnableEffect(FX_BLOOM, false);
  EnableEffect(FX_BLUR, false);
  EnableEffect(FX_HDR, false);
  EnableEffect(FX_FXAA, false);
#else
  EnableEffect(FX_SSAO, Config::GetInstance().GetBool("enable_ssao"));
  EnableEffect(FX_BLOOM, Config::GetInstance().GetBool("enable_bloom"));
  EnableEffect(FX_BLUR, Config::GetInstance().GetBool("enable_mblur"));
  EnableEffect(FX_HDR, Config::GetInstance().GetBool("enable_hdr"));
  EnableEffect(FX_FXAA, Config::GetInstance().GetBool("enable_fxaa"));
#endif
}

Compositor::~Compositor() {}

void Compositor::OnUpdate(float time) {}

void Compositor::EnableEffect(const std::string &FX, bool Enable) {
  CompositorList[FX].Enabled = Enable;
  const string EnableStr = "uEnable";
  const float Flag = Enable ? 1.0f : 0.0f;
  Log::Message(string(Enable ? "Enable " : "Disable ").append(FX).append(" FX"));
  const auto &FXPtr = CompositorList[FX];
  GetFPparameters(FXPtr.OutputCompositor)->setNamedConstant(FXPtr.EnableParam, Flag);
  for (const auto &it : FXPtr.CompositorChain) GetFPparameters(it)->setNamedConstant(EnableStr, Flag);
}

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

void Compositor::InitMRT() {
  string MRTCompositor = "MRT";
  ViewportSizeY = OgreViewport->getActualDimensions().height();
  ViewportSizeX = OgreViewport->getActualDimensions().width();
  OgreAssert(OgreCompositorManager->addCompositor(OgreViewport, MRTCompositor, 0), "Failed to add MRT compositor");
  OgreCompositorManager->setCompositorEnabled(OgreViewport, MRTCompositor, false);
  auto *MRTCompositorPtr = OgreCompositorChain->getCompositor(MRTCompositor);
  auto *MRTTexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("mrt");
  OgreAssert(MRTTexturePtr, "MRTCompositor texture not created");
#ifdef MOBILE
  MRTTexturePtr->width = 1024;
  MRTTexturePtr->height = 768;
#else
  MRTTexturePtr->width = ViewportSizeX;
  MRTTexturePtr->height = ViewportSizeY;
#endif
  auto *SSAOTexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("ssao");
  auto *FinalTexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("final");
  auto *RT1TexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("rt1");
  auto *RT2TexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("rt2");
  auto *RT3TexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("rt3");
  auto *RT4TexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("rt4");
  auto *RT5TexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("rt5");
  auto *RT6TexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("rt6");
  auto *RT7TexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("rt7");
  auto *RT8TexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("rt8");
  auto *RT9TexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("rt9");
  auto *RT10TexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("rt10");
  auto *RT11TexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("rt11");
  auto *RT12TexturePtr = MRTCompositorPtr->getTechnique()->getTextureDefinition("rt12");
  OgreAssert(SSAOTexturePtr, "SSAO texture not created");
  OgreAssert(FinalTexturePtr, "Final texture not created");
  OgreAssert(RT1TexturePtr, "RT1 texture not created");
  OgreAssert(RT2TexturePtr, "RT2 texture not created");
  OgreAssert(RT3TexturePtr, "RT3 texture not created");
  OgreAssert(RT4TexturePtr, "RT4 texture not created");
  OgreAssert(RT5TexturePtr, "RT5 texture not created");
  OgreAssert(RT6TexturePtr, "RT6 texture not created");
  OgreAssert(RT7TexturePtr, "RT7 texture not created");
  OgreAssert(RT8TexturePtr, "RT8 texture not created");
  OgreAssert(RT9TexturePtr, "RT9 texture not created");
  OgreAssert(RT10TexturePtr, "RT10 texture not created");
  OgreAssert(RT11TexturePtr, "RT11 texture not created");
  OgreAssert(RT12TexturePtr, "RT12 texture not created");
#ifdef MOBILE
  FinalTexturePtr->width = MRTTexturePtr->width;
  FinalTexturePtr->height = MRTTexturePtr->height;
#endif
  SSAOTexturePtr->width = MRTTexturePtr->width / 2;
  SSAOTexturePtr->height = MRTTexturePtr->height / 2;
  RT1TexturePtr->width = MRTTexturePtr->width / 2;
  RT1TexturePtr->height = MRTTexturePtr->height / 2;
  RT2TexturePtr->width = MRTTexturePtr->width / 4;
  RT2TexturePtr->height = MRTTexturePtr->height / 4;
  RT3TexturePtr->width = MRTTexturePtr->width / 8;
  RT3TexturePtr->height = MRTTexturePtr->height / 8;
  RT4TexturePtr->width = MRTTexturePtr->width / 16;
  RT4TexturePtr->height = MRTTexturePtr->height / 16;
  RT5TexturePtr->width = MRTTexturePtr->width / 32;
  RT5TexturePtr->height = MRTTexturePtr->height / 32;
  RT6TexturePtr->width = MRTTexturePtr->width / 64;
  RT6TexturePtr->height = MRTTexturePtr->height / 64;
  RT7TexturePtr->width = MRTTexturePtr->width / 128;
  RT7TexturePtr->height = MRTTexturePtr->height / 128;
  RT8TexturePtr->width = MRTTexturePtr->width / 256;
  RT8TexturePtr->height = MRTTexturePtr->height / 256;
  RT9TexturePtr->width = MRTTexturePtr->width / 512;
  RT9TexturePtr->height = MRTTexturePtr->height / 512;
  RT10TexturePtr->width = MRTTexturePtr->width / 1024;
  RT10TexturePtr->height = MRTTexturePtr->height / 1024;
  RT11TexturePtr->width = MRTTexturePtr->width / 2048;
  RT11TexturePtr->height = MRTTexturePtr->height / 2048;
  RT12TexturePtr->width = MRTTexturePtr->width / 4096;
  RT12TexturePtr->height = MRTTexturePtr->height / 4096;

  OgreCompositorManager->setCompositorEnabled(OgreViewport, MRTCompositor, true);
}

}  // namespace Glue
