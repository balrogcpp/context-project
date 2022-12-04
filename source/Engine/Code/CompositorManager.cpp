/// created by Andrey Vasiliev

#include "pch.h"
#include "CompositorManager.h"
#include <queue>

using namespace std;

namespace {
inline bool RenderSystemIsGL() { return Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL Rendering Subsystem"; };
inline bool RenderSystemIsGL3() { return Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL 3+ Rendering Subsystem"; };
inline bool RenderSystemIsGLES2() { return Ogre::Root::getSingleton().getRenderSystem()->getName() == "OpenGL ES 2.x Rendering Subsystem"; };
}  // namespace

namespace Glue {
CompositorManager::CompositorManager()
    : fixedViewportSize(false), forceSizeX(-1), forceSizeY(-1), MRT_COMPOSITOR("MRT"), BLOOM_COMPOSITOR("Bloom"), mipChain(14), oddMipsOnly(false) {
  if (RenderSystemIsGLES2()) {
    mipChain /= 2.0;
    //oddMipsOnly = true;
  }
}
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
  InitMRT(true);

  // shadows before bloom
  AddCompositor("SSAO", false);

  // fog is always enabled
  AddCompositor("Fog", true);

  // init bloom mipmaps
  InitMipChain(false);

  // extra compositors
  AddCompositor("Blur", false);
  AddCompositor("FXAA", false);
  AddCompositor("Output", true);

  // reg as viewport listener
  ogreViewport->addListener(this);
}

void CompositorManager::OnClean() { ogreViewport->removeListener(this); }

void CompositorManager::AddCompositor(const string &name, bool enable, int position) {
  auto *compositor = compositorManager->addCompositor(ogreViewport, name, position);
  ASSERTION(compositor, "[CompositorManager] Failed to add MRT compoitor");
  compositor->addListener(this);
  compositorManager->setCompositorEnabled(ogreViewport, name, enable);
}

void CompositorManager::SetCompositorEnabled(const string &name, bool enable) {
  ASSERTION(compositorChain->getCompositorPosition(name) != Ogre::CompositorChain::NPOS, "[CompositorManager] No compositor found");
  compositorManager->setCompositorEnabled(ogreViewport, name, enable);

  if (name == BLOOM_COMPOSITOR) {
    for (int i = 0; i < mipChain; i++) {
      string newName = BLOOM_COMPOSITOR + "It" + to_string(i);
      ASSERTION(compositorChain->getCompositorPosition(newName) != Ogre::CompositorChain::NPOS, "[CompositorManager] Failed to find Bloom compositor");

      if (!oddMipsOnly) {
        compositorManager->setCompositorEnabled(ogreViewport, newName, enable);
      } else if (i % 2 == 1) {
        compositorManager->setCompositorEnabled(ogreViewport, newName, enable);
      } else {
        compositorManager->setCompositorEnabled(ogreViewport, newName, false);
      }
    }

    AddCompositor("BloomEnd", enable);
  }
}

void CompositorManager::SetFixedViewport(bool fixed) { fixedViewportSize = fixed; }
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
    compositorPtr->addListener(this);
    for (auto &jt : compositorPtr->getTechnique()->getTextureDefinitions()) {
      if (jt->type == Ogre::TEX_TYPE_2D && jt->refTexName.empty()) {
        jt->width = x * jt->widthFactor;
        jt->height = y * jt->heightFactor;
      }
    }

    compositorManager->setCompositorEnabled(ogreViewport, compositorName, enabled);
  }
}

void CompositorManager::InitMRT(bool enable) {
  auto *mrtCompositor = compositorManager->addCompositor(ogreViewport, MRT_COMPOSITOR, 0);
  ASSERTION(mrtCompositor, "[CompositorManager] Failed to add MRT compositor");

  // check textures
  auto *tech = mrtCompositor->getTechnique();

  ASSERTION(tech->getTextureDefinition("mrt"), "[CompositorManager] mrt texture failed to create");

  compositorManager->setCompositorEnabled(ogreViewport, MRT_COMPOSITOR, enable);
}

void CompositorManager::InitNoMRT(bool enable) {
  auto *mrtCompositor = compositorManager->addCompositor(ogreViewport, "NoMRT", 0);
  ASSERTION(mrtCompositor, "[CompositorManager] Failed to add MRT compositor");

  // check textures
  auto *tech = mrtCompositor->getTechnique();

  ASSERTION(tech->getTextureDefinition("rt"), "[CompositorManager] rt texture failed to create");

  compositorManager->setCompositorEnabled(ogreViewport, "NoMRT", enable);
}

void CompositorManager::InitMipChain(bool enable) {
  auto *bloomCompositor = compositorManager->addCompositor(ogreViewport, BLOOM_COMPOSITOR);
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
  ASSERTION(tech->getTextureDefinition("rt11"), "[CompositorManager] rt11 texture failed to create");
  ASSERTION(tech->getTextureDefinition("rt12"), "[CompositorManager] rt12 texture failed to create");
  ASSERTION(tech->getTextureDefinition("rt13"), "[CompositorManager] rt13 texture failed to create");

  compositorManager->setCompositorEnabled(ogreViewport, BLOOM_COMPOSITOR, enable);

  for (int i = 0; i < mipChain; i++) {
    string name = BLOOM_COMPOSITOR + "It" + to_string(i);
    auto *bloomItCompositor = compositorManager->addCompositor(ogreViewport, name);
    ASSERTION(bloomItCompositor, "[CompositorManager] Failed to add Bloom compositor");
    compositorManager->setCompositorEnabled(ogreViewport, name, enable);
  }

  AddCompositor("BloomEnd", enable);
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
    compositor->addListener(this);
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

void CompositorManager::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr& mat) {
  if (pass_id != 42)  // not SSAO, return
    return;

  // this is the camera you're using
  Ogre::Camera *cam = compositorChain->getViewport()->getCamera();

  // get the pass
  Ogre::Pass *pass = mat->getBestTechnique()->getPass(0);

  // get the fragment shader parameters
  auto params = pass->getFragmentProgramParameters();
  // set the projection matrix we need
  params->setNamedConstant("ptMat", Ogre::Matrix4::CLIPSPACE2DTOIMAGESPACE * cam->getProjectionMatrixWithRSDepth());
}
}  // namespace Glue
