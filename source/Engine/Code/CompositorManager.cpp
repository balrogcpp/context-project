/// created by Andrey Vasiliev

#include "pch.h"
#include "CompositorManager.h"
#include <queue>

using namespace std;

namespace Glue {

CompositorManager::CompositorManager() : fixedViewportSize(false), MRT_COMPOSITOR("MRT"), BLOOM_COMPOSITOR("Bloom") {}

CompositorManager::~CompositorManager() {}

void CompositorManager::OnUpdate(float time) {}

void CompositorManager::OnSetUp() {
#ifdef MOBILE
  fixedViewportSize = true;
#endif
  // init fields
  compositorManager = Ogre::CompositorManager::getSingletonPtr();
  OgreAssert(compositorManager, "[CompositorManager] compositorManager not initialised");
  ogreSceneManager = Ogre::Root::getSingleton().getSceneManager("Default");
  OgreAssert(ogreSceneManager, "[CompositorManager] ogreSceneManager not initialised");
  ogreCamera = ogreSceneManager->getCamera("Default");
  OgreAssert(ogreCamera, "[CompositorManager] ogreCamera not initialised");
  ogreViewport = ogreCamera->getViewport();
  compositorChain = compositorManager->getCompositorChain(ogreViewport);

  // init compositor chain
  InitMRT();

  // init bloom mipmaps
  InitMipChain();

  // extra compositors
  AddCompositor("SSAO", false);
  AddCompositor("Fog", true);
  AddCompositor("FXAA", false);
  AddCompositor("Blur", false);
  AddCompositor("Output", true);

  // reg as viewport listener
  ogreViewport->addListener(this);
}

void CompositorManager::OnClean() { ogreViewport->removeListener(this); }

void CompositorManager::AddCompositor(const string &name, bool enable, int position) {
  OgreAssert(compositorManager->addCompositor(ogreViewport, name, position), "Failed to add MRT compoitor");
  compositorManager->setCompositorEnabled(ogreViewport, name, enable);
}

void CompositorManager::SetCompositorEnabled(const string &name, bool enable) { compositorManager->setCompositorEnabled(ogreViewport, name, enable); }

void CompositorManager::InitMRT() {
  OgreAssert(compositorManager->addCompositor(ogreViewport, MRT_COMPOSITOR, 0), "Failed to add MRT compositor");

  // check textures
  auto *mrtCompositor = compositorChain->getCompositor(MRT_COMPOSITOR);
  auto *mrtTexture = mrtCompositor->getTechnique()->getTextureDefinition("mrt");
  auto *rtTexture = mrtCompositor->getTechnique()->getTextureDefinition("rt");
  auto *mrt1Texture = mrtCompositor->getTechnique()->getTextureDefinition("mrt1");
  auto *mrt2Texture = mrtCompositor->getTechnique()->getTextureDefinition("mrt2");
  OgreAssert(mrtTexture, "MRTCompositor mrtTexture not created");
  OgreAssert(rtTexture, "MRTCompositor rtTexture not created");
  OgreAssert(mrt1Texture, "MRTCompositor mrt1Texture not created");
  OgreAssert(mrt2Texture, "MRTCompositor mrt2Texture not created");

  // resize them properly
  if (fixedViewportSize) {
#ifdef MOBILE
    mrtTexture->width = 1024;
    mrtTexture->height = 768;
#endif
    int w = mrtTexture->width;
    int h = mrtTexture->height;

    rtTexture->width = w;
    rtTexture->height = h;
    mrt1Texture->width = w;
    mrt1Texture->height = h;
    mrt2Texture->width = w;
    mrt2Texture->height = h;
  }

  compositorManager->setCompositorEnabled(ogreViewport, MRT_COMPOSITOR, true);
}

void CompositorManager::InitMipChain() {
  OgreAssert(compositorManager->addCompositor(ogreViewport, BLOOM_COMPOSITOR, 1), "Failed to add Bloom compoitor");

  // check textures
  auto *mrtCompositor = compositorChain->getCompositor(MRT_COMPOSITOR);
  auto *mrtTexture = mrtCompositor->getTechnique()->getTextureDefinition("mrt");
  auto *bloomCompositor = compositorChain->getCompositor(BLOOM_COMPOSITOR);
  auto *rt1Texture = bloomCompositor->getTechnique()->getTextureDefinition("rt1");
  auto *rt2Texture = bloomCompositor->getTechnique()->getTextureDefinition("rt2");
  auto *rt3Texture = bloomCompositor->getTechnique()->getTextureDefinition("rt3");
  auto *rt4Texture = bloomCompositor->getTechnique()->getTextureDefinition("rt4");
  auto *rt5Texture = bloomCompositor->getTechnique()->getTextureDefinition("rt5");
  auto *rt6Texture = bloomCompositor->getTechnique()->getTextureDefinition("rt6");
  auto *rt7Texture = bloomCompositor->getTechnique()->getTextureDefinition("rt7");
  auto *rt8Texture = bloomCompositor->getTechnique()->getTextureDefinition("rt8");
  auto *rt9Texture = bloomCompositor->getTechnique()->getTextureDefinition("rt9");
  auto *rt10Texture = bloomCompositor->getTechnique()->getTextureDefinition("rt10");
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

  if (fixedViewportSize) {
    int w = mrtTexture->width;
    int h = mrtTexture->height;

    rt1Texture->width = w / 2;
    rt1Texture->height = h / 2;
    rt2Texture->width = w / 4;
    rt2Texture->height = h / 4;
    rt3Texture->width = w / 8;
    rt3Texture->height = h / 8;
    rt4Texture->width = w / 16;
    rt4Texture->height = h / 16;
    rt5Texture->width = w / 32;
    rt5Texture->height = h / 32;
    rt6Texture->width = w / 64;
    rt6Texture->height = h / 64;
    rt7Texture->width = w / 128;
    rt7Texture->height = h / 128;
    rt8Texture->width = w / 256;
    rt8Texture->height = h / 256;
    rt9Texture->width = w / 512;
    rt9Texture->height = h / 512;
    rt10Texture->width = w / 1024;
    rt10Texture->height = h / 1024;
  }
}

void CompositorManager::viewportDimensionsChanged(Ogre::Viewport *viewport) {
  // don't update if viewport size handled manually
  if (fixedViewportSize) {
    return;
  }

  // remove compositor that have to be changed
  std::queue<std::string> compositorList;
  std::queue<bool> compositorEnabled;

  int sizeY = ogreViewport->getActualDimensions().height();
  int sizeX = ogreViewport->getActualDimensions().width();
  Ogre::LogManager::getSingleton().logMessage("[CompositorManager] Actual viewport size: " + to_string(sizeX) + "x" + to_string(sizeY),
                                              Ogre::LML_TRIVIAL);

  for (const auto it : compositorChain->getCompositorInstances()) {
    compositorList.push(it->getCompositor()->getName());
    compositorEnabled.push(it->getEnabled());
  }

  compositorChain->removeAllCompositors();

  while (!compositorList.empty()) {
    const std::string compositor = compositorList.front();
    compositorList.pop();
    const bool enabled = compositorEnabled.front();
    compositorEnabled.pop();
    OgreAssert(compositorManager->addCompositor(ogreViewport, compositor), std::string("Failed to add " + compositor + " compositor").c_str());
    compositorManager->setCompositorEnabled(ogreViewport, compositor, enabled);
  }
}

}  // namespace Glue
