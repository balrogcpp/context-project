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
  ogreCamera = sceneManager->getCamera("Default");
  ogreViewport = ogreCamera->getViewport();
  compositorChain = compositorManager->getCompositorChain(ogreViewport);

  // init compositor chain
  InitMRT();

  // extra compositors
  AddCompositor("SSAO", false);
  AddCompositor("Bloom", false);
  AddCompositor("Fog", true);
  AddCompositor("FXAA", false);
  AddCompositor("Blur", false);
  AddCompositor("Output", true);
}

void CompositorManager::OnClean() {}

void CompositorManager::OnPause() {}

void CompositorManager::OnResume() {}

void CompositorManager::AddCompositor(const string &name, bool enable, int position) {
  OgreAssert(compositorManager->addCompositor(ogreViewport, name, position), "Failed to add MRT compoitor");
  compositorManager->setCompositorEnabled(ogreViewport, name, enable);
}

void CompositorManager::SetCompositorEnabled(const string &name, bool enable) { compositorManager->setCompositorEnabled(ogreViewport, name, enable); }

static void AdjustBlur(Ogre::CompositorInstance *compositor) {
  auto *rt1Texture = compositor->getTechnique()->getTextureDefinition("rt1");
  auto *rt2Texture = compositor->getTechnique()->getTextureDefinition("rt2");
  auto *rt3Texture = compositor->getTechnique()->getTextureDefinition("rt3");
  auto *rt4Texture = compositor->getTechnique()->getTextureDefinition("rt4");
  auto *rt5Texture = compositor->getTechnique()->getTextureDefinition("rt5");
  auto *rt6Texture = compositor->getTechnique()->getTextureDefinition("rt6");
  auto *rt7Texture = compositor->getTechnique()->getTextureDefinition("rt7");
  auto *rt8Texture = compositor->getTechnique()->getTextureDefinition("rt8");
  auto *rt9Texture = compositor->getTechnique()->getTextureDefinition("rt9");
  auto *rt10Texture = compositor->getTechnique()->getTextureDefinition("rt10");
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
}

void CompositorManager::InitMRT() {
  const string MRT = "MRT";
  int sizeY = ogreViewport->getActualDimensions().height();
  int sizeX = ogreViewport->getActualDimensions().width();
  OgreAssert(compositorManager->addCompositor(ogreViewport, MRT, 0), "Failed to add MRT compositor");
  compositorManager->setCompositorEnabled(ogreViewport, MRT, false);
  //  auto *mrtCompositor = compositorChain->getCompositor(MRT);
  //  auto *mrtTexture = mrtCompositor->getTechnique()->getTextureDefinition("mrt");
  //  auto *mrt0Texture = mrtCompositor->getTechnique()->getTextureDefinition("mrt0");
  //  auto *mrt1Texture = mrtCompositor->getTechnique()->getTextureDefinition("mrt1");
  //  auto *mrt2Texture = mrtCompositor->getTechnique()->getTextureDefinition("mrt2");
  //  OgreAssert(mrtTexture, "MRTCompositor mrtTexture not created");
  //  OgreAssert(mrt0Texture, "MRTCompositor mrt0Texture not created");
  //  OgreAssert(mrt1Texture, "MRTCompositor mrt1Texture not created");
  //  OgreAssert(mrt2Texture, "MRTCompositor mrt2Texture not created");
  //#ifdef MOBILE
  //  mrtTexture->width = 1024;
  //  mrtTexture->height = 768;
  //#else
  //  mrtTexture->width = sizeX;
  //  mrtTexture->height = sizeY;
  //#endif
  //  mrt0Texture->width = mrtTexture->width;
  //  mrt0Texture->height = mrtTexture->height;
  //  mrt1Texture->width = mrtTexture->width;
  //  mrt1Texture->height = mrtTexture->height;
  //  mrt2Texture->width = mrtTexture->width;
  //  mrt2Texture->height = mrtTexture->height;
  compositorManager->setCompositorEnabled(ogreViewport, MRT, true);
}

}  // namespace Glue
