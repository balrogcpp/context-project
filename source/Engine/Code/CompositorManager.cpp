/// created by Andrey Vasiliev

#include "pch.h"
#include "CompositorManager.h"

using namespace std;

namespace Glue {

CompositorManager::CompositorManager() {}

CompositorManager::~CompositorManager() {}

void CompositorManager::OnUpdate(float time) {}

void CompositorManager::OnSetUp() {
  // init fields
  compositorManager = Ogre::CompositorManager::getSingletonPtr();
  sceneManager = Ogre::Root::getSingleton().getSceneManager("Default");
  camera = sceneManager->getCamera("Default");
  viewport = camera->getViewport();
  compositorChain = compositorManager->getCompositorChain(viewport);

  // init compositor chain
  InitMRT();

//  AddCompositorEnabled("SSAO");
//  AddCompositorEnabled("FXAA");
  AddCompositorEnabled("Output");
}

void CompositorManager::OnClean() {}

void CompositorManager::OnPause() {}

void CompositorManager::OnResume() {}

void CompositorManager::AddCompositorEnabled(const string &name) {
  OgreAssert(compositorManager->addCompositor(viewport, name), "Failed to add MRT compoitor");
  compositorManager->setCompositorEnabled(viewport, name, true);
}

void CompositorManager::AddCompositorDisabled(const string &name) {
  OgreAssert(compositorManager->addCompositor(viewport, name), "Failed to add MRT compoitor");
  compositorManager->setCompositorEnabled(viewport, name, false);
}

void CompositorManager::EnableCompositor(const string &name) { compositorManager->setCompositorEnabled(viewport, name, true); }

void CompositorManager::InitMRT() {
  const string MRT = "MRT";
  int sizeY = viewport->getActualDimensions().height();
  int sizeX = viewport->getActualDimensions().width();
  OgreAssert(compositorManager->addCompositor(viewport, MRT, 0), "Failed to add MRT compositor");
  compositorManager->setCompositorEnabled(viewport, MRT, false);
  auto *mrtCompositor = compositorChain->getCompositor(MRT);
  auto *mrtTexture = mrtCompositor->getTechnique()->getTextureDefinition("mrt");
  OgreAssert(mrtTexture, "MRTCompositor texture not created");
#ifdef MOBILE
  mrtTexture->width = 1024;
  mrtTexture->height = 768;
#else
  mrtTexture->width = sizeX;
  mrtTexture->height = sizeY;
#endif
  auto *rt0Texture = mrtCompositor->getTechnique()->getTextureDefinition("rt0");
  auto *rt1Texture = mrtCompositor->getTechnique()->getTextureDefinition("rt1");
  auto *rt2Texture = mrtCompositor->getTechnique()->getTextureDefinition("rt2");
  auto *rt3Texture = mrtCompositor->getTechnique()->getTextureDefinition("rt3");
  auto *rt4Texture = mrtCompositor->getTechnique()->getTextureDefinition("rt4");
  auto *rt5Texture = mrtCompositor->getTechnique()->getTextureDefinition("rt5");
  auto *rt6Texture = mrtCompositor->getTechnique()->getTextureDefinition("rt6");
  auto *rt7Texture = mrtCompositor->getTechnique()->getTextureDefinition("rt7");
  auto *rt8Texture = mrtCompositor->getTechnique()->getTextureDefinition("rt8");
  auto *rt9Texture = mrtCompositor->getTechnique()->getTextureDefinition("rt9");
  auto *rt10Texture = mrtCompositor->getTechnique()->getTextureDefinition("rt10");
  auto *rt11Texture = mrtCompositor->getTechnique()->getTextureDefinition("rt11");
  auto *rt12Texture = mrtCompositor->getTechnique()->getTextureDefinition("rt12");
  OgreAssert(rt0Texture, "rt0 texture failed to create");
  OgreAssert(rt1Texture, "rt1 texture failed to create");
  OgreAssert(rt2Texture, "rt2 texture failed to create");
  OgreAssert(rt3Texture, "rt3 texture failed to create");
  OgreAssert(rt4Texture, "rt4 texture failed to create");
  OgreAssert(rt5Texture, "rt5 texture failed to create");
  OgreAssert(rt6Texture, "rt6 texture failed to create");
  OgreAssert(rt7Texture, "rt7 texture failed to create");
  OgreAssert(rt8Texture, "rt8 texture failed to create");
  OgreAssert(rt9Texture, "rt9 texture failed to create");
  OgreAssert(rt10Texture, "rt10 texture failed to create");
  OgreAssert(rt11Texture, "rt11 texture failed to create");
  OgreAssert(rt12Texture, "rt12 texture failed to create");
#ifdef MOBILE
  rt0Texture->width = mrtTexture->width;
  rt0Texture->height = mrtTexture->height;
#endif
  rt1Texture->width = mrtTexture->width / 2;
  rt1Texture->height = mrtTexture->height / 2;
  rt2Texture->width = mrtTexture->width / 4;
  rt2Texture->height = mrtTexture->height / 4;
  rt3Texture->width = mrtTexture->width / 8;
  rt3Texture->height = mrtTexture->height / 8;
  rt4Texture->width = mrtTexture->width / 16;
  rt4Texture->height = mrtTexture->height / 16;
  rt5Texture->width = mrtTexture->width / 32;
  rt5Texture->height = mrtTexture->height / 32;
  rt6Texture->width = mrtTexture->width / 64;
  rt6Texture->height = mrtTexture->height / 64;
  rt7Texture->width = mrtTexture->width / 128;
  rt7Texture->height = mrtTexture->height / 128;
  rt8Texture->width = mrtTexture->width / 256;
  rt8Texture->height = mrtTexture->height / 256;
  rt9Texture->width = mrtTexture->width / 512;
  rt9Texture->height = mrtTexture->height / 512;
  rt10Texture->width = mrtTexture->width / 1024;
  rt10Texture->height = mrtTexture->height / 1024;
  rt11Texture->width = mrtTexture->width / 2048;
  rt11Texture->height = mrtTexture->height / 2048;
  rt12Texture->width = mrtTexture->width / 4096;
  rt12Texture->height = mrtTexture->height / 4096;

  compositorManager->setCompositorEnabled(viewport, MRT, true);
}

}  // namespace Glue
