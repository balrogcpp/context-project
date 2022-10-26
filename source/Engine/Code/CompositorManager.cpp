/// created by Andrey Vasiliev

#include "pch.h"
#include "CompositorManager.h"

using namespace std;

namespace Glue {

CompositorManager::CompositorManager() : MRT_COMPOSITOR("MRT") {}

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
  OgreAssert(compositorManager->addCompositor(ogreViewport, MRT_COMPOSITOR, 0), "Failed to add MRT compositor");
  InitMRT();
  compositorManager->setCompositorEnabled(ogreViewport, MRT_COMPOSITOR, true);

  // extra compositors
  AddCompositor("SSAO", false);
  AddCompositor("Bloom", false);
  AddCompositor("Fog", true);
  AddCompositor("FXAA", false);
  AddCompositor("Blur", false);
  AddCompositor("Output", true);

  //
  InputSequencer::GetInstance().RegWinObserver(this);
}

void CompositorManager::OnClean() {
  InputSequencer::GetInstance().UnregWinObserver(this);
}

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
  const string MRT = MRT_COMPOSITOR;
  int sizeY = ogreViewport->getActualDimensions().height();
  int sizeX = ogreViewport->getActualDimensions().width();
  printf("Actual viewport size: %dx%d\n", sizeX, sizeY);
  auto *mrtCompositor = compositorChain->getCompositor(MRT);
  auto *mrtTexture = mrtCompositor->getTechnique()->getTextureDefinition("mrt");
  auto *rtTexture = mrtCompositor->getTechnique()->getTextureDefinition("rt");
  auto *mrt1Texture = mrtCompositor->getTechnique()->getTextureDefinition("mrt1");
  auto *mrt2Texture = mrtCompositor->getTechnique()->getTextureDefinition("mrt2");
  OgreAssert(mrtTexture, "MRTCompositor mrtTexture not created");
  OgreAssert(rtTexture, "MRTCompositor rtTexture not created");
  OgreAssert(mrt1Texture, "MRTCompositor mrt1Texture not created");
  OgreAssert(mrt2Texture, "MRTCompositor mrt2Texture not created");
#ifdef MOBILE
  mrtTexture->width = 1024;
  mrtTexture->height = 768;
#else
  mrtTexture->width = sizeX;
  mrtTexture->height = sizeY;
#endif
  rtTexture->width = mrtTexture->width;
  rtTexture->height = mrtTexture->height;
  mrt1Texture->width = mrtTexture->width;
  mrt1Texture->height = mrtTexture->height;
  mrt2Texture->width = mrtTexture->width;
  mrt2Texture->height = mrtTexture->height;
}

void CompositorManager::OnEvent(const SDL_Event &event) {}
void CompositorManager::OnQuit() {}
void CompositorManager::OnFocusLost() {}
void CompositorManager::OnFocusGained() {}
void CompositorManager::OnSizeChanged(int x, int y, uint32_t id) {
#ifndef MOBILE
  compositorManager->removeCompositor(ogreViewport, MRT_COMPOSITOR);
  OgreAssert(compositorManager->addCompositor(ogreViewport, MRT_COMPOSITOR, 0), "Failed to add MRT compositor");
  InitMRT();
  compositorManager->setCompositorEnabled(ogreViewport, MRT_COMPOSITOR, true);
#endif
}
void CompositorManager::OnExposed() {}

}  // namespace Glue
