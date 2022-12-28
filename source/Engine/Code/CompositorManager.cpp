/// created by Andrey Vasiliev

#include "pch.h"
#include "CompositorManager.h"
#include <OgreCompositorLogic.h>
#include <queue>

using namespace std;

namespace Glue {
CompositorManager::CompositorManager()
    : fixedViewportSize(true), forceSizeX(-1), forceSizeY(-1), MRT_COMPOSITOR("MRT"), BLOOM_COMPOSITOR("Bloom"), mipChain(14), oddMipsOnly(false) {
  if (RenderSystemIsGLES2()) {
    mipChain /= 2.0;
    // oddMipsOnly = true;
  }
}
CompositorManager::~CompositorManager() {}
void CompositorManager::OnUpdate(float time) {}

class DeferredLogic final : public Ogre::CompositorLogic {
 public:
  void compositorInstanceCreated(Ogre::CompositorInstance *newInstance) override { newInstance->addListener(GetComponentPtr<CompositorManager>()); }
  void compositorInstanceDestroyed(Ogre::CompositorInstance *destroyedInstance) override {}
};

void CompositorManager::OnSetUp() {
#ifdef MOBILE
  fixedViewportSize = true;
#endif
  // init fields
  compositorManager = Ogre::CompositorManager::getSingletonPtr();
  compositorManager->registerCompositorLogic("DeferredLogic", new DeferredLogic());
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

  // ambient is always enabled
  AddCompositor("Ambient", true);

  // fog is always enabled
  AddCompositor("Fog", true);

  // init bloom mipmaps
  AddCompositor("Rays", false);
  InitMipChain(false);

  // extra compositors
  AddCompositor("Blur", false);
  AddCompositor("FXAA", false);
  AddCompositor("Output", true);
  AddCompositor("Paused", false);

  // reg as viewport listener
  ogreViewport->addListener(this);
}

void CompositorManager::OnClean() { ogreViewport->removeListener(this); }

void CompositorManager::AddCompositor(const string &name, bool enable, int position) {
  auto *compositor = compositorManager->addCompositor(ogreViewport, name, position);
  ASSERTION(compositor, "[CompositorManager] Failed to add MRT compoitor");
  compositorManager->setCompositorEnabled(ogreViewport, name, enable);
}

bool CompositorManager::IsCompositorEnabled(const std::string &name) {
  ASSERTION(compositorChain->getCompositorPosition(name) != Ogre::CompositorChain::NPOS, "[CompositorManager] No compositor found");
  size_t index = compositorChain->getCompositorPosition(name);
  return compositorChain->getCompositor(index)->getEnabled();
}

void CompositorManager::SetCompositorScale(const std::string &name, float scale) {
  ASSERTION(compositorChain->getCompositorPosition(name) != Ogre::CompositorChain::NPOS, "[CompositorManager] No compositor found");
  size_t index = compositorChain->getCompositorPosition(name);
}

void CompositorManager::SetCompositorEnabled(const string &name, bool enable) {
  ASSERTION(compositorChain->getCompositorPosition(name) != Ogre::CompositorChain::NPOS, "[CompositorManager] No compositor found");
  compositorManager->setCompositorEnabled(ogreViewport, name, enable);

  if (name == BLOOM_COMPOSITOR) {
    for (int i = 0; i < mipChain; i++) {
      string newName = BLOOM_COMPOSITOR + "It" + to_string(i);
      ASSERTION(compositorChain->getCompositorPosition(newName) != Ogre::CompositorChain::NPOS,
                "[CompositorManager] Failed to find Bloom compositor");

      if (!oddMipsOnly) {
        compositorManager->setCompositorEnabled(ogreViewport, newName, enable);
      } else if (i % 2 == 1) {
        compositorManager->setCompositorEnabled(ogreViewport, newName, enable);
      } else {
        compositorManager->setCompositorEnabled(ogreViewport, newName, false);
      }
    }

    compositorManager->setCompositorEnabled(ogreViewport, BLOOM_COMPOSITOR + "End", enable);
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
  compositorManager->setCompositorEnabled(ogreViewport, BLOOM_COMPOSITOR, enable);

  for (int i = 0; i < mipChain; i++) {
    string name = BLOOM_COMPOSITOR + "It" + to_string(i);
    auto *bloomItCompositor = compositorManager->addCompositor(ogreViewport, name);
    ASSERTION(bloomItCompositor, "[CompositorManager] Failed to add Bloom compositor");
    compositorManager->setCompositorEnabled(ogreViewport, name, enable);
  }

  AddCompositor(BLOOM_COMPOSITOR + "End", enable);
}

void CompositorManager::viewportCameraChanged(Ogre::Viewport *viewport) { ogreCamera = viewport->getCamera(); }
void CompositorManager::viewportDimensionsChanged(Ogre::Viewport *viewport) {
  // don't update if viewport size handled manually
  if (fixedViewportSize) {
    return;
  }

  int sizeY = viewport->getActualDimensions().height();
  int sizeX = viewport->getActualDimensions().width();
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

//  source https://wiki.ogre3d.org/GetScreenspaceCoords
static Ogre::Vector4 GetScreenspaceCoords(Ogre::MovableObject *object, Ogre::Camera *camera) {
  if (!object->isInScene()) {
    return Ogre::Vector4::ZERO;
  }

  const Ogre::AxisAlignedBox &AABB = object->getWorldBoundingBox(true);

  /**
   * If you need the point above the object instead of the center point:
   * This snippet derives the average point between the top-most corners of the bounding box
   * Ogre::Vector3 point = (AABB.getCorner(AxisAlignedBox::FAR_LEFT_TOP)
   *    + AABB.getCorner(AxisAlignedBox::FAR_RIGHT_TOP)
   *    + AABB.getCorner(AxisAlignedBox::NEAR_LEFT_TOP)
   *    + AABB.getCorner(AxisAlignedBox::NEAR_RIGHT_TOP)) / 4;
   */

  // Get the center point of the object's bounding box
  Ogre::Vector3 center = AABB.getCenter();
  Ogre::Vector4 point = Ogre::Vector4(center, 1.0);

  // Is the camera facing that point? If not, return false
  Ogre::Plane cameraPlane = Ogre::Plane(camera->getDerivedOrientation().zAxis(), camera->getDerivedPosition());
  if (cameraPlane.getSide(center) != Ogre::Plane::NEGATIVE_SIDE) {
    return Ogre::Vector4::ZERO;
  }

  // Transform the 3D point into screen space
  point = Ogre::Matrix4::CLIPSPACE2DTOIMAGESPACE * camera->getProjectionMatrixWithRSDepth() * camera->getViewMatrix() * point;
  point /= point.w;

  return point;
}

static Ogre::Vector4 GetLightScreenspaceCoords(Ogre::Light *light, Ogre::Camera *camera) {
  Ogre::Vector4 point;

  if (light->getType() == Ogre::Light::LT_DIRECTIONAL)
    point = Ogre::Vector4(-light->getDerivedDirection(), 0.0);
  else
    point = Ogre::Vector4(light->getDerivedPosition(), 1.0);

  point = Ogre::Matrix4::CLIPSPACE2DTOIMAGESPACE * camera->getProjectionMatrixWithRSDepth() * camera->getViewMatrix() * point;
  point /= point.w;

  return point;
}

void CompositorManager::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat) {
  if (pass_id == 10) {  // SSAO
    const auto &fp = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
    fp->setIgnoreMissingParams(true);
    fp->setNamedConstant("ProjMatrix", Ogre::Matrix4::CLIPSPACE2DTOIMAGESPACE * ogreCamera->getProjectionMatrixWithRSDepth());
    fp->setIgnoreMissingParams(false);

  } else if (pass_id == 11) {
    const auto &fp = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
    const auto &lightList = ogreSceneManager->_getLightsAffectingFrustum();

    fp->setIgnoreMissingParams(true);
    fp->setNamedConstant("LightCount", lightList.size() > 0 ? static_cast<Ogre::Real>(1.0) : static_cast<Ogre::Real>(0.0));
    fp->setNamedConstant("LightPositionViewSpace", GetLightScreenspaceCoords(lightList[0], ogreCamera));
    fp->setIgnoreMissingParams(false);
  }
}

void CompositorManager::notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat) {}

}  // namespace Glue
