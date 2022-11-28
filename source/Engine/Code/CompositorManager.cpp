/// created by Andrey Vasiliev

#include "pch.h"
#include "CompositorManager.h"
#include <queue>

using namespace std;

namespace Glue {
CompositorManager::CompositorManager()
    : fixedViewportSize(false), forceSizeX(-1), forceSizeY(-1), MRT_COMPOSITOR("MRT"), BLOOM_COMPOSITOR("Bloom"), mipChain(7) {}
CompositorManager::~CompositorManager() {}

void CompositorManager::OnUpdate(float time) {}

void CompositorManager::OnSetUp() {
#ifdef MOBILE
  fixedViewportSize = true;
#endif
  // init fields
  compositorManager = Ogre::CompositorManager::getSingletonPtr();
  ASSERTION(compositorManager, "[CompositorManager] compositorManager not initialised");
  ogreSceneManager = Ogre::Root::getSingleton().getSceneManager("Default");
  ASSERTION(ogreSceneManager, "[CompositorManager] ogreSceneManager not initialised");
  ogreCamera = ogreSceneManager->getCamera("Default");
  ASSERTION(ogreCamera, "[CompositorManager] ogreCamera not initialised");
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
  ASSERTION(compositor, "[CompositorManager] Failed to add MRT compoitor");
  compositorManager->setCompositorEnabled(ogreViewport, name, enable);
}

void CompositorManager::SetCompositorEnabled(const string &name, bool enable) {
  ASSERTION(compositorChain->getCompositorPosition(name) != Ogre::CompositorChain::NPOS, "[CompositorManager] No compositor found");
  compositorManager->setCompositorEnabled(ogreViewport, name, enable);

  if (name == BLOOM_COMPOSITOR) {
    for (int i = 0; i < mipChain; i++) {
      string newName = BLOOM_COMPOSITOR + "It" + to_string(i);
      ASSERTION(compositorChain->getCompositorPosition(newName) != Ogre::CompositorChain::NPOS, "[CompositorManager] No compositor found");
      compositorManager->setCompositorEnabled(ogreViewport, newName, enable);
    }
  }
}

void CompositorManager::SetFixedViewportSize(int x, int y) {
  fixedViewportSize = (x > 0) && (y > 0);

  if (!fixedViewportSize) {
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
    ASSERTION(compositor, "[CompositorManager] Failed to add compositor");

    auto *compositorPtr = compositorChain->getCompositor(compositorName);
    for (auto &jt : compositorPtr->getTechnique()->getTextureDefinitions()) {
      if (jt->type == Ogre::TEX_TYPE_2D && jt->refTexName.empty()) {
        jt->width = x * jt->widthFactor;
        jt->height = y * jt->heightFactor;
      }
    }

    compositorManager->setCompositorEnabled(ogreViewport, compositorName, enabled);
  }
}

void CompositorManager::InitMRT() {
  auto *mrtCompositor = compositorManager->addCompositor(ogreViewport, MRT_COMPOSITOR, 0);
  ASSERTION(mrtCompositor, "[CompositorManager] Failed to add MRT compositor");

  // check textures
  auto *tech = mrtCompositor->getTechnique();

  ASSERTION(tech->getTextureDefinition("mrt"), "[CompositorManager] mrt texture failed to create");
  ASSERTION(tech->getTextureDefinition("rt"), "[CompositorManager] rt texture failed to create");
  ASSERTION(tech->getTextureDefinition("mrt1"), "[CompositorManager] mrt1 failed to create");
  ASSERTION(tech->getTextureDefinition("mrt2"), "[CompositorManager] mrt2 failed to create");

  compositorManager->setCompositorEnabled(ogreViewport, MRT_COMPOSITOR, true);
}

void CompositorManager::InitNoMRT() {
  auto *mrtCompositor = compositorManager->addCompositor(ogreViewport, "NoMRT", 0);
  ASSERTION(mrtCompositor, "[CompositorManager] Failed to add MRT compositor");

  // check textures
  auto *tech = mrtCompositor->getTechnique();

  ASSERTION(tech->getTextureDefinition("rt"), "[CompositorManager] rt texture failed to create");

  compositorManager->setCompositorEnabled(ogreViewport, "NoMRT", true);
}

void CompositorManager::InitMipChain() {
  auto *bloomCompositor = compositorManager->addCompositor(ogreViewport, BLOOM_COMPOSITOR, 1);
  ASSERTION(bloomCompositor, "[CompositorManager] Failed to add Bloom compoitor");

  // check textures
  auto *tech = bloomCompositor->getTechnique();

  ASSERTION(tech->getTextureDefinition("rt0"), "[CompositorManager] rt0 texture failed to create");
  ASSERTION(tech->getTextureDefinition("rt1"), "[CompositorManager] rt1 texture failed to create");
  ASSERTION(tech->getTextureDefinition("rt2"), "[CompositorManager] rt2 texture failed to create");
  ASSERTION(tech->getTextureDefinition("rt3"), "[CompositorManager] rt3 texture failed to create");
  ASSERTION(tech->getTextureDefinition("rt4"), "[CompositorManager] rt4 texture failed to create");
  ASSERTION(tech->getTextureDefinition("rt5"), "[CompositorManager] rt5 texture failed to create");
  ASSERTION(tech->getTextureDefinition("rt6"), "[CompositorManager] rt6 texture failed to create");
  ASSERTION(tech->getTextureDefinition("rt7"), "[CompositorManager] rt7 texture failed to create");
  ASSERTION(tech->getTextureDefinition("rt8"), "[CompositorManager] rt8 texture failed to create");
  ASSERTION(tech->getTextureDefinition("rt9"), "[CompositorManager] rt9 texture failed to create");
  ASSERTION(tech->getTextureDefinition("rt10"), "[CompositorManager] rt10 texture failed to create");

  compositorManager->setCompositorEnabled(ogreViewport, BLOOM_COMPOSITOR, false);

  for (int i = 0; i < mipChain; i++) {
    string name = BLOOM_COMPOSITOR + "It" + to_string(i);
    auto *bloomItCompositor = compositorManager->addCompositor(ogreViewport, name, i + 2);
    ASSERTION(bloomItCompositor, "[CompositorManager] Failed to add Bloom compoitor");
    compositorManager->setCompositorEnabled(ogreViewport, name, false);
  }
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
    ASSERTION(compositor, "[CompositorManager] Failed to add compositor");

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
