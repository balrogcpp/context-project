/// created by Andrey Vasiliev

#include "pch.h"
#include "CompositorManager.h"
#include <OgreCompositorLogic.h>

using namespace std;

namespace gge {
CompositorManager::CompositorManager() : fixedViewportSize(false), MRT_COMPOSITOR("MRT"), BLOOM_COMPOSITOR("Glow") {}

CompositorManager::~CompositorManager() = default;

void CompositorManager::OnUpdate(float time) {
  viewProjPrev = viewProj;
  viewProj = ogreCamera->getProjectionMatrix() * ogreCamera->getViewMatrix();
}

void CompositorManager::SetSleep(bool sleep) { _sleep = sleep; }

class DeferredLogic final : public Ogre::CompositorLogic {
 public:
  void compositorInstanceCreated(Ogre::CompositorInstance *newInstance) override { newInstance->addListener(GetComponentPtr<CompositorManager>()); }

  void compositorInstanceDestroyed(Ogre::CompositorInstance *destroyedInstance) override {
    destroyedInstance->removeListener(GetComponentPtr<CompositorManager>());
  }
};

void CompositorManager::preRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) {}

void CompositorManager::postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) {}

class ReflTexListener : public Ogre::RenderTargetListener {
 public:
  ReflTexListener(Ogre::Camera *camera, Ogre::Plane plane) {
    ogreCamera = camera;
    ogrePlane = plane;
  }

  void preRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) {
    ogreCamera->enableCustomNearClipPlane(ogrePlane);
    ogreCamera->enableReflection(ogrePlane);
  }

  void postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) {
    ogreCamera->disableCustomNearClipPlane();
    ogreCamera->disableReflection();
  }

 private:
  Ogre::Camera *ogreCamera = nullptr;
  Ogre::Plane ogrePlane;
};

class RefrTexListener : public Ogre::RenderTargetListener {
 public:
  RefrTexListener(Ogre::Camera *camera, Ogre::Plane plane) {
    ogreCamera = camera;
    ogrePlane = plane;
  }

  void preRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) override { ogreCamera->enableCustomNearClipPlane(ogrePlane); }

  void postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) override { ogreCamera->disableCustomNearClipPlane(); }

 private:
  Ogre::Camera *ogreCamera = nullptr;
  Ogre::Plane ogrePlane;
};

void CompositorManager::OnSetUp() {
  // init fields
  compositorManager = Ogre::CompositorManager::getSingletonPtr();
  compositorManager->registerCompositorLogic("DeferredLogic", new DeferredLogic());
  ASSERTION(compositorManager, "[CompositorManager] compositorManager not initialised");
  ogreSceneManager = Ogre::Root::getSingleton().getSceneManager("Default");
  ASSERTION(ogreSceneManager, "[CompositorManager] ogreSceneManager not initialised");
  ogreCamera = ogreSceneManager->getCamera("Camera");
  ASSERTION(ogreCamera, "[CompositorManager] ogreCamera not initialised");
  ogreViewport = ogreCamera->getViewport();
  compositorChain = compositorManager->getCompositorChain(ogreViewport);

  AddCubeCamera();
  InitMRT(true);
  AddCompositor("SSAO", false);
  AddCompositor("SSR", false);
  AddCompositor("FXAA", false);
  AddCompositor("SMAA", false);
  AddCompositor("Blur", false);
  AddCompositor("HDR", true);
  AddCompositor("Glow", false);
  AddCompositor("Paused", false);

  // reg as viewport listener
  ogreViewport->addListener(this);
  fixedViewportSize = false;
  viewportDimensionsChanged(ogreViewport);
}

void CompositorManager::OnClean() { ogreViewport->removeListener(this); }

void CompositorManager::AddCompositor(const string &name, bool enable, int position) {
  if (name == "Fresnel") {
    AddFresnelCompositor(plane);
    return;
  }

  auto *compositor = compositorManager->addCompositor(ogreViewport, name, position);
  ASSERTION(compositor, "[CompositorManager] Failed to add MRT compositor");
  compositorManager->setCompositorEnabled(ogreViewport, name, enable);

  if (name == BLOOM_COMPOSITOR) {
    AddCompositor("GlowIt4", enable);
    AddCompositor("GlowIt7", enable);
  }
}

void CompositorManager::EnableCompositor(const string &name, bool enable) {
  ASSERTION(compositorChain->getCompositorPosition(name) != Ogre::CompositorChain::NPOS, "[CompositorManager] No compositor found");

  if (name == "SSAO" || name == "SSR") {
    compositorManager->setCompositorEnabled(ogreViewport, "DepthHalved", enable);
  } else if (name == "HDR") {
    EnableCompositor("HDR", true);
  }

  compositorManager->setCompositorEnabled(ogreViewport, name, enable);

  if (name == BLOOM_COMPOSITOR) {
    EnableCompositor("GlowIt4", enable);
    EnableCompositor("GlowIt7", enable);
  }
}

void CompositorManager::AddFresnelCompositor(Ogre::Plane plane) {
  this->plane = plane;

  if (!IsCompositorInChain("Fresnel")) {
    auto *compositor = compositorManager->addCompositor(ogreViewport, "Fresnel", 0);
    ASSERTION(compositor, "[CompositorManager] Failed to add compositor");
    compositorManager->setCompositorEnabled(ogreViewport, "Fresnel", true);
    auto *rt1 = compositorChain->getCompositor("Fresnel")->getRenderTarget("reflection");
    rt1->addListener(new ReflTexListener(ogreCamera, Ogre::Plane(plane.normal, -plane.d)));
    auto *rt2 = compositorChain->getCompositor("Fresnel")->getRenderTarget("refraction");
    rt2->addListener(new RefrTexListener(ogreCamera, Ogre::Plane(-plane.normal, plane.d)));
  }
}

void CompositorManager::RemoveFresnelCompositor() {
  if (IsCompositorInChain("Fresnel")) {
    compositorChain->getCompositor("Fresnel")->getRenderTarget("reflection")->removeAllListeners();
    compositorChain->getCompositor("Fresnel")->getRenderTarget("refraction")->removeAllListeners();
    compositorChain->removeCompositor(compositorChain->getCompositorPosition("Fresnel"));
  }
}

void CompositorManager::AddCubeCamera() {
  if (!ogreSceneManager->hasCamera("CubeCamera")) {
    cubeCamera = ogreSceneManager->createCamera("CubeCamera");
    cubeCamera->setFOVy(Ogre::Degree(90.0));
    cubeCamera->setAspectRatio(1.0);
    cubeCamera->setNearClipDistance(50.0);
    cubeCamera->setFarClipDistance(ogreCamera->getFarClipDistance());
    ogreSceneManager->getRootSceneNode()->createChildSceneNode(Ogre::Vector3::UNIT_Y * 5.0)->attachObject(cubeCamera);
  }

  if (!IsCompositorInChain("CubeMap")) {
    AddCompositor("CubeMap", true);
    auto *rt = compositorChain->getCompositor("CubeMap")->getRenderTarget("cube");
    rt->removeAllViewports();
    rt->addViewport(cubeCamera);
  }
}

void CompositorManager::DestroyCubeCamera() {
  if (ogreSceneManager->hasCamera("CubeCamera")) {
    if (IsCompositorInChain("CubeMap")) {
      compositorChain->getCompositor("CubeMap")->getRenderTarget("cube")->removeAllViewports();
      compositorChain->removeCompositor(compositorChain->getCompositorPosition("CubeMap"));
    }

    ogreSceneManager->destroyCamera(cubeCamera);
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

    compositorManager->setCompositorEnabled(ogreViewport, compositorName, enabled);
  }
}

void CompositorManager::InitMRT(bool enable) {
  auto *mrtCompositor = compositorManager->addCompositor(ogreViewport, MRT_COMPOSITOR);
  ASSERTION(mrtCompositor, "[CompositorManager] Failed to add MRT compositor");

  // check textures
  auto *tech = mrtCompositor->getTechnique();

  ASSERTION(tech->getTextureDefinition("mrt"), "[CompositorManager] mrt texture failed to create");

  compositorManager->setCompositorEnabled(ogreViewport, MRT_COMPOSITOR, enable);

  AddCompositor("DepthHalved", false);
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

Ogre::Camera *CompositorManager::GetOgreCamera() { return ogreCamera; }

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

    compositorManager->setCompositorEnabled(ogreViewport, compositorName, enabled);
  }
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
  if (pass_id == 0) {         // 0 = mrt render
  } else if (pass_id == 1) {  // 1 = Reconstructor
    static bool isEven = false;
    const auto &fp = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
    fp->setNamedConstant("IsEven", Ogre::Real(isEven = !isEven));
    fp->setNamedConstant("ViewProjPrev", Ogre::Matrix4::CLIPSPACE2DTOIMAGESPACE * viewProjPrev);
    fp->setNamedConstant("InvViewMatrix", ogreCamera->getViewMatrix().inverse());

  } else if (pass_id == 10) {  // 10 = SSAO
    static bool isEven = false;
    const auto &fp = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
    fp->setNamedConstant("ProjMatrix", Ogre::Matrix4::CLIPSPACE2DTOIMAGESPACE * ogreCamera->getProjectionMatrix());
    fp->setNamedConstant("ClipDistance", ogreCamera->getFarClipDistance() - ogreCamera->getNearClipDistance());
    fp->setNamedConstant("IsEven", Ogre::Real(isEven = !isEven));

  } else if (pass_id == 11) {  // 11 = SSR
    static bool isEven = false;
    const auto &fp = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
    fp->setNamedConstant("ProjMatrix", Ogre::Matrix4::CLIPSPACE2DTOIMAGESPACE * ogreCamera->getProjectionMatrix());
    fp->setNamedConstant("ClipDistance", ogreCamera->getFarClipDistance() - ogreCamera->getNearClipDistance());
    fp->setNamedConstant("IsEven", Ogre::Real(isEven = !isEven));

  } else if (pass_id == 14) {  // 14 = Motion Blur
    const auto &fp = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
    fp->setNamedConstant("ViewProjPrev", Ogre::Matrix4::CLIPSPACE2DTOIMAGESPACE * viewProjPrev);
    fp->setNamedConstant("InvViewMatrix", ogreCamera->getViewMatrix().inverse());

  } else if (pass_id == 13) {  // 12 = Rays
    const auto &fp = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
    const auto &lightList = ogreSceneManager->_getLightsAffectingFrustum();
    static Ogre::Real LightPositionViewSpace[OGRE_MAX_SIMULTANEOUS_LIGHTS * 4];

    int directionals = 0;  // count directional light
    for (int i = 0; i < lightList.size(); i++) {
      Ogre::Light *light = lightList[i];
      if (light->getType() == Ogre::Light::LT_DIRECTIONAL) {
        directionals++;
        Ogre::Vector4 point = GetLightScreenSpaceCoords(lightList[i], ogreCamera);
        LightPositionViewSpace[4 * i] = point.x;
        LightPositionViewSpace[4 * i + 1] = point.y;
        LightPositionViewSpace[4 * i + 2] = point.z;
        LightPositionViewSpace[4 * i + 3] = point.w;
      }
    }

    fp->setNamedConstant("LightPositionViewSpace", LightPositionViewSpace, OGRE_MAX_SIMULTANEOUS_LIGHTS);
    fp->setNamedConstant("LightCount", static_cast<Ogre::int32>(directionals));

  } else if (pass_id == 100) {  // paused

  } else if (pass_id == 99) {  // end render
  }
}

void CompositorManager::notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat) {}
}  // namespace gge
