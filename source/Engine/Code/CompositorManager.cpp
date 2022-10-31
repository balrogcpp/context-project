/// created by Andrey Vasiliev

#include "pch.h"
#include "CompositorManager.h"
#include <queue>

using namespace std;

namespace Glue {
CompositorManager::CompositorManager() : fixedViewportSize(false), forceSizeX(-1), forceSizeY(-1), MRT_COMPOSITOR("MRT"), BLOOM_COMPOSITOR("Bloom") {}
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
  auto *compositor = compositorManager->addCompositor(ogreViewport, name, position);
  OgreAssert(compositor, "Failed to add MRT compoitor");
  compositorManager->setCompositorEnabled(ogreViewport, name, enable);
}

void CompositorManager::SetCompositorEnabled(const string &name, bool enable) { compositorManager->setCompositorEnabled(ogreViewport, name, enable); }

void CompositorManager::SetFixedViewportSize(int x, int y) {
  fixedViewportSize = (x > 0) && (y > 0);

  if (!fixedViewportSize) {
    viewportDimensionsChanged(ogreViewport);
    return;
  }

  forceSizeX = x;
  forceSizeY = y;

  // remove compositors that have to be changed
  queue<pair<string, bool>> compositorList;

  for (const auto &it : compositorChain->getCompositorInstances()) {
    compositorList.push(make_pair(it->getCompositor()->getName(), it->getEnabled()));
  }

  compositorChain->removeAllCompositors();

  // compositors are automatically resized according to actual viewport size when added to chain
  while (!compositorList.empty()) {
    const std::string compositorName = compositorList.front().first;
    const bool enabled = compositorList.front().second;
    compositorList.pop();

    auto *compositor = compositorManager->addCompositor(ogreViewport, compositorName);
    OgreAssert(compositor, "[CompositorManager] Failed to add compositor");

    auto *compositorPtr = compositorChain->getCompositor(compositorName);
    for (auto &jt : compositorPtr->getTechnique()->getTextureDefinitions()) {
      if (jt->type == Ogre::TEX_TYPE_2D && jt->refTexName.empty()) {
        if (jt->width == 0) jt->width = x * jt->widthFactor;
        if (jt->height == 0) jt->height = y * jt->heightFactor;
      }
    }

    compositorManager->setCompositorEnabled(ogreViewport, compositorName, enabled);
  }
}

void CompositorManager::InitMRT() {
  auto *mrtCompositor = compositorManager->addCompositor(ogreViewport, MRT_COMPOSITOR, 0);
  OgreAssert(mrtCompositor, "Failed to add MRT compositor");

  // check textures
  auto *tech = mrtCompositor->getTechnique();

  OgreAssert(tech->getTextureDefinition("mrt"), "mrt texture failed to create");
  OgreAssert(tech->getTextureDefinition("rt"), "rt texture failed to create");
  OgreAssert(tech->getTextureDefinition("mrt1"), "mrt1 failed to create");
  OgreAssert(tech->getTextureDefinition("mrt2"), "mrt2 failed to create");

  compositorManager->setCompositorEnabled(ogreViewport, MRT_COMPOSITOR, true);
}

void CompositorManager::InitMipChain() {
  auto *bloomCompositor = compositorManager->addCompositor(ogreViewport, BLOOM_COMPOSITOR, 1);
  OgreAssert(bloomCompositor, "Failed to add Bloom compoitor");

  // check textures
  auto *tech = bloomCompositor->getTechnique();

  OgreAssert(tech->getTextureDefinition("rt1"), "rt1 texture failed to create");
  OgreAssert(tech->getTextureDefinition("rt2"), "rt2 texture failed to create");
  OgreAssert(tech->getTextureDefinition("rt3"), "rt3 texture failed to create");
  OgreAssert(tech->getTextureDefinition("rt4"), "rt4 texture failed to create");
  OgreAssert(tech->getTextureDefinition("rt5"), "rt5 texture failed to create");
  OgreAssert(tech->getTextureDefinition("rt6"), "rt6 texture failed to create");
  OgreAssert(tech->getTextureDefinition("rt7"), "rt7 texture failed to create");
  OgreAssert(tech->getTextureDefinition("rt8"), "rt8 texture failed to create");
  OgreAssert(tech->getTextureDefinition("rt9"), "rt9 texture failed to create");
  OgreAssert(tech->getTextureDefinition("rt10"), "rt10 texture failed to create");

  compositorManager->setCompositorEnabled(ogreViewport, BLOOM_COMPOSITOR, false);
}

void CompositorManager::viewportCameraChanged(Ogre::Viewport *viewport) {}
void CompositorManager::viewportDimensionsChanged(Ogre::Viewport *viewport) {
  // don't update if viewport size handled manually
  if (fixedViewportSize) {
    return;
  }

  int sizeY = ogreViewport->getActualDimensions().height();
  int sizeX = ogreViewport->getActualDimensions().width();
  Ogre::LogManager::getSingleton().logMessage("[CompositorManager] Actual viewport size: " + to_string(sizeX) + "x" + to_string(sizeY),
                                              Ogre::LML_TRIVIAL);

  // remove compositors that have to be changed
  queue<pair<string, bool>> compositorList;

  for (const auto &it : compositorChain->getCompositorInstances()) {
    compositorList.push(make_pair(it->getCompositor()->getName(), it->getEnabled()));
  }

  compositorChain->removeAllCompositors();

  // compositors are automatically resized according to actual viewport size when added to chain
  while (!compositorList.empty()) {
    const std::string compositorName = compositorList.front().first;
    const bool enabled = compositorList.front().second;
    compositorList.pop();

    auto *compositor = compositorManager->addCompositor(viewport, compositorName);
    OgreAssert(compositor, "[CompositorManager] Failed to add compositor");

    auto *compositorPtr = compositorChain->getCompositor(compositorName);
    for (auto &jt : compositorPtr->getTechnique()->getTextureDefinitions()) {
      if (jt->type == Ogre::TEX_TYPE_2D && jt->refTexName.empty()) {
        jt->width = 0;
        jt->height = 0;
      }
    }

    compositorManager->setCompositorEnabled(viewport, compositorName, enabled);
  }
}
}  // namespace Glue
