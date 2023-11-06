/// created by Andrey Vasiliev

#include "pch.h"
#include "CompositorManager.h"
#include <OgreCustomCompositionPass.h>

using namespace std;

namespace gge {

class RenderShadows : public Ogre::CompositorInstance::RenderSystemOperation {
 public:
  RenderShadows(Ogre::CompositorInstance *instance, const Ogre::CompositionPass *pass) { viewport = instance->getChain()->getViewport(); }

  void execute(Ogre::SceneManager *sm, Ogre::RenderSystem *rs) override {
    if (sm->getShadowTechnique() != Ogre::SHADOWTYPE_NONE) {
      Ogre::Camera *camera = viewport->getCamera();
      auto *context = sm->_pauseRendering();
      sm->prepareShadowTextures(camera, viewport);
      sm->_resumeRendering(context);
    }
  }

  virtual ~RenderShadows() {}

 private:
  Ogre::Viewport *viewport = nullptr;
};

class RenderShadowsPass : public Ogre::CustomCompositionPass {
 public:
  Ogre::CompositorInstance::RenderSystemOperation *createOperation(Ogre::CompositorInstance *instance, const Ogre::CompositionPass *pass) override {
    return OGRE_NEW RenderShadows(instance, pass);
  }

 protected:
  virtual ~RenderShadowsPass() {}
};

CompositorManager::CompositorManager() : fixedViewportSize(false), MRT_COMPOSITOR("MRT"), BLOOM_COMPOSITOR("Glow") {}

CompositorManager::~CompositorManager() = default;

void CompositorManager::OnUpdate(float time) {
  viewProjPrev = viewProj;
  viewProj = camera->getProjectionMatrix() * camera->getViewMatrix();
}

void CompositorManager::SetSleep(bool sleep) { _sleep = sleep; }

void CompositorManager::compositorInstanceCreated(Ogre::CompositorInstance *newInstance) { newInstance->addListener(this); }

void CompositorManager::compositorInstanceDestroyed(Ogre::CompositorInstance *destroyedInstance) { destroyedInstance->removeListener(this); }

void CompositorManager::preRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) {
  string name = evt.source->getName();
  if (name.find("reflection") != string::npos) {
    camera->enableCustomNearClipPlane(Ogre::Plane(plane.normal, -plane.d));
    camera->enableReflection(Ogre::Plane(plane.normal, -plane.d));
  } else if (name.find("refraction") != string::npos) {
    camera->enableCustomNearClipPlane(Ogre::Plane(-plane.normal, plane.d));
  }
}

void CompositorManager::postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) {
  string name = evt.source->getName();
  if (name.find("reflection") != string::npos) {
    camera->disableCustomNearClipPlane();
    camera->disableReflection();
  } else if (name.find("refraction") != string::npos) {
    camera->disableCustomNearClipPlane();
  }
}

void CompositorManager::OnSetUp() {
  // init fields
  compositorManager = Ogre::CompositorManager::getSingletonPtr();
  compositorManager->registerCompositorLogic("DeferredLogic", this);
  ASSERTION(compositorManager, "[CompositorManager] compositorManager not initialised");
  sceneManager = Ogre::Root::getSingleton().getSceneManager("Default");
  ASSERTION(sceneManager, "[CompositorManager] ogreSceneManager not initialised");
  camera = sceneManager->getCamera("Camera");
  ASSERTION(camera, "[CompositorManager] ogreCamera not initialised");
  compositorManager->registerCustomCompositionPass("RenderShadowMap", OGRE_NEW RenderShadowsPass);

  viewport = camera->getViewport();
  compositorChain = compositorManager->getCompositorChain(viewport);
  cubeCamera = sceneManager->createCamera("CubeCamera");
  cubeCamera->setFOVy(Ogre::Degree(90.0));
  cubeCamera->setAspectRatio(1.0);
  cubeCamera->setNearClipDistance(1.0);
  cubeCamera->setFarClipDistance(camera->getFarClipDistance());
  sceneManager->getRootSceneNode()->createChildSceneNode(Ogre::Vector3::UNIT_Y * 2.0)->attachObject(cubeCamera);
  AddCompositor("CubeMap", true);
  auto *rt = compositorChain->getCompositor("CubeMap")->getRenderTarget("cube");
  rt->removeAllViewports();
  rt->addViewport(cubeCamera);
  rt->addListener(this);

  // create compositor chain
  AddCompositor("MRT", true);
  AddCompositor("SSAO", !RenderSystemIsGLES2());
  AddCompositor("SSR", false);
  if (!RenderSystemIsGLES2()) AddCompositor("Glow", true);
  if (!RenderSystemIsGLES2()) AddCompositor("HDR", true);
  AddCompositor("FXAA", true);
  if (!RenderSystemIsGLES2()) AddCompositor("SMAA", false);
  AddCompositor("Tonemap", false);
  AddCompositor("Blur", !RenderSystemIsGLES2());
  AddCompositor("FullScreenBlur", false);

  // reg as viewport listener
  viewport->addListener(this);
  fixedViewportSize = false;
  viewportDimensionsChanged(viewport);
}

void CompositorManager::OnClean() { viewport->removeListener(this); }

void CompositorManager::AddCompositor(const string &name, bool enable, int position) {
  auto *compositor = compositorManager->addCompositor(viewport, name, position);
  ASSERTION(compositor, "[CompositorManager] Failed to add MRT compositor");
  compositorManager->setCompositorEnabled(viewport, name, enable);
}

void CompositorManager::EnableCompositor(const string &name, bool enable) {
  ASSERTION(compositorChain->getCompositorPosition(name) != Ogre::CompositorChain::NPOS, "[CompositorManager] No compositor found");
  compositorManager->setCompositorEnabled(viewport, name, enable);
}

void CompositorManager::AddReflectionPlane(Ogre::Plane plane) {
  this->plane = plane;

  if (!IsCompositorInChain("Fresnel")) {
    auto *compositor = compositorManager->addCompositor(viewport, "Fresnel", 0);
    ASSERTION(compositor, "[CompositorManager] Failed to add compositor");
    compositorManager->setCompositorEnabled(viewport, "Fresnel", true);
    auto *rt1 = compositorChain->getCompositor("Fresnel")->getRenderTarget("reflection");
    rt1->addListener(this);
    auto *rt2 = compositorChain->getCompositor("Fresnel")->getRenderTarget("refraction");
    rt2->addListener(this);
  }
}

bool CompositorManager::IsCompositorInChain(const std::string &name) {
  return compositorChain->getCompositorPosition(name) != Ogre::CompositorChain::NPOS;
}

bool CompositorManager::IsCompositorEnabled(const std::string &name) {
  if (compositorChain->getCompositorPosition(name) == Ogre::CompositorChain::NPOS) return false;
  size_t index = compositorChain->getCompositorPosition(name);
  return compositorChain->getCompositor(index)->getEnabled();
}

void CompositorManager::SetCompositorScale(const std::string &name, float scale) {
  ASSERTION(compositorChain->getCompositorPosition(name) != Ogre::CompositorChain::NPOS, "[CompositorManager] No compositor found");
  size_t index = compositorChain->getCompositorPosition(name);
}

void CompositorManager::SetFixedViewport(bool fixed) { fixedViewportSize = fixed; }
void CompositorManager::SetFixedViewportSize(int x, int y) {
  fixedViewportSize = (x > 0) && (y > 0);

  if (!fixedViewportSize) {
    return;
  }

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

    AddCompositor(compositorName, false);
    auto *compositorPtr = compositorChain->getCompositor(compositorName);
    for (auto &jt : compositorPtr->getTechnique()->getTextureDefinitions()) {
      if (jt->type == Ogre::TEX_TYPE_2D && jt->refTexName.empty()) {
        jt->width = x * jt->widthFactor;
        jt->height = y * jt->heightFactor;
      }
    }

    compositorManager->setCompositorEnabled(viewport, compositorName, enabled);
  }
}

void CompositorManager::CacheCompositorChain() {
  // remove compositors that have to be changed
  for (const auto &it : compositorChain->getCompositorInstances()) {
    compositorList.push(make_pair(it->getCompositor()->getName(), it->getEnabled()));
  }
}

void CompositorManager::RemoveAllCompositors() { compositorChain->removeAllCompositors(); }

void CompositorManager::DisableRendering() {
  compositorChain->getCompositor(0)->getTechnique()->getTargetPass(0)->getPass(1)->setType(Ogre::CompositionPass::PT_CLEAR);
  compositorChain->_markDirty();
}
void CompositorManager::EnableRendering() {
  compositorChain->getCompositor(0)->getTechnique()->getTargetPass(0)->getPass(1)->setType(Ogre::CompositionPass::PT_RENDERSCENE);
  compositorChain->_markDirty();
}

Ogre::Camera *CompositorManager::GetOgreCamera() { return camera; }

void CompositorManager::ApplyCachedCompositorChain() {
  // compositors are automatically resized according to actual viewport size when added to chain
  while (!compositorList.empty()) {
    const std::string compositorName = compositorList.front().first;
    const bool enabled = compositorList.front().second;
    compositorList.pop();

    AddCompositor(compositorName, false);
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

void CompositorManager::viewportCameraChanged(Ogre::Viewport *viewport) { camera = viewport->getCamera(); }
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

    AddCompositor(compositorName, false);
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

static Ogre::Vector4 GetLightScreenSpaceCoords(Ogre::Light *light, Ogre::Camera *camera) {
  Ogre::Vector4 point = Ogre::Vector4::ZERO;

  if (light->getType() == Ogre::Light::LT_DIRECTIONAL)
    point = Ogre::Vector4(-light->getDerivedDirection(), 0.0);
  else
    point = Ogre::Vector4(light->getDerivedPosition(), 1.0);

  Ogre::Vector3 v = point.xyz().normalisedCopy();
  Ogre::Vector3 l = camera->getDerivedOrientation().zAxis().normalisedCopy();
  point = Ogre::Matrix4::CLIPSPACE2DTOIMAGESPACE * camera->getProjectionMatrix() * camera->getViewMatrix() * point;
  point /= point.w;

  if (light->getType() == Ogre::Light::LT_DIRECTIONAL)
    point.w = Ogre::Math::saturate(-v.dotProduct(l));
  else
    point.w = Ogre::Math::saturate(v.dotProduct(l));

  point.w = Ogre::Math::Abs(Ogre::Math::Sin(point.w * Ogre::Math::HALF_PI));

  return point;
}

void CompositorManager::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat) {
  if (pass_id == 10 || pass_id == 20) {  // 10 = SSAO, 20 = SSR
    const auto &fp = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
    fp->setNamedConstant("ProjMatrix", Ogre::Matrix4::CLIPSPACE2DTOIMAGESPACE * camera->getProjectionMatrix());
    fp->setNamedConstant("ClipDistance", camera->getFarClipDistance() - camera->getNearClipDistance());

  } else if (pass_id == 30) {  // 14 = Motion Blur
    const auto &fp = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
    fp->setNamedConstant("ViewProjPrev", Ogre::Matrix4::CLIPSPACE2DTOIMAGESPACE * viewProjPrev);
    fp->setNamedConstant("InvViewMatrix", camera->getViewMatrix().inverse());

  } else if (pass_id == 99999) {  // 12 = Rays
    const auto &fp = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
    const auto &ll = sceneManager->_getLightsAffectingFrustum();
    if (!ll.empty()) {
      Ogre::Vector4 lightPos = GetLightScreenSpaceCoords(ll[0], camera);
      fp->setNamedConstant("LightPositionViewSpace", lightPos);
    }
  } else if (pass_id == 99) {  // 99 = FullScreenBlur
  }
}

void CompositorManager::notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat) {}
}  // namespace gge
