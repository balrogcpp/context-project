/// created by Andrey Vasiliev

#include "pch.h"
#include "CompositorManager.h"
#include "ArHosekSkyModel.h"
#include <OgreCustomCompositionPass.h>

using namespace std;
using namespace Ogre;

namespace gge {

namespace {
// Useful shader functions
inline float Deg2Rad(float deg) { return deg * (1.0f / 180.0f) * Ogre::Math::PI; }
inline float Rad2Deg(float rad) { return rad * (1.0f / Ogre::Math::PI) * 180.0f; }
inline float Clamp(float val, float min, float max) {
  assert(max >= min);
  val = val < min ? min : val > max ? max : val;
  return val;
}
inline float AngleBetween(const Ogre::Vector3 &dir0, const Ogre::Vector3 &dir1) { return std::acos(std::max(dir0.dotProduct(dir1), 0.00001f)); }
inline float Mix(float x, float y, float s) { return x + (y - x) * s; }
}  // namespace

class GBufferSchemeHandler : public Ogre::MaterialManager::Listener {
 public:
  GBufferSchemeHandler() = default;
  virtual ~GBufferSchemeHandler() = default;

  Ogre::Technique *handleSchemeNotFound(unsigned short schemeIndex, const Ogre::String &schemeName, Ogre::Material *originalMaterial,
                                        unsigned short lodIndex, const Ogre::Renderable *rend) final {
    const auto &refMat = Ogre::MaterialManager::getSingleton().getByName("GBuffer");
    Ogre::Technique *gBufferTech = originalMaterial->createTechnique();
    gBufferTech->setSchemeName(schemeName);
    Ogre::Pass *gbufPass = gBufferTech->createPass();
    *gbufPass = *refMat->getTechnique(0)->getPass(0);
    return gBufferTech;
  }
};

class RenderShadows : public Ogre::CompositorInstance::RenderSystemOperation {
 public:
  RenderShadows(Ogre::CompositorInstance *instance, const Ogre::CompositionPass *pass) { viewport = instance->getChain()->getViewport(); }
  virtual ~RenderShadows() {}

  void execute(Ogre::SceneManager *sm, Ogre::RenderSystem *rs) override {
    if (sm->getShadowTechnique() != Ogre::SHADOWTYPE_NONE) {
      Ogre::Camera *camera = viewport->getCamera();
      auto *context = sm->_pauseRendering();
      sm->prepareShadowTextures(camera, viewport);
      sm->_resumeRendering(context);
    }
  }

 private:
  Ogre::Viewport *viewport = nullptr;
};

class RenderShadowsPass : public Ogre::CustomCompositionPass {
 public:
  Ogre::CompositorInstance::RenderSystemOperation *createOperation(Ogre::CompositorInstance *instance, const Ogre::CompositionPass *pass) override {
    return OGRE_NEW RenderShadows(instance, pass);
  }
};

CompositorManager::CompositorManager() : fixedViewportSize(false), plane(Vector3(0, 1, 0), 0) {}

CompositorManager::~CompositorManager() = default;

void CompositorManager::OnUpdate(float time) {
  ArHosekSkyModelState *states[3];
  Ogre::Vector3f sunDir;
  if (sceneManager->hasLight("Sun")) {
    auto *SunPtr = sceneManager->getLight("Sun");
    sunDir = SunPtr ? -SunPtr->getDerivedDirection().normalisedCopy() : Ogre::Vector3::ZERO;
  } else {
    sunDir = Ogre::Vector3::ZERO;
  }

  Ogre::Vector3f albedo = Ogre::Vector3f(0.0f);
  sunDir.y = Clamp(sunDir.y, 0.0, 1.0);
  sunDir.normalise();
  Ogre::Real turbidity = 3.0;
  albedo = Ogre::Vector3f(Clamp(albedo.x, 0.0, 1.0), Clamp(albedo.y, 0.0, 1.0), Clamp(albedo.z, 0.0, 1.0));

  float thetaS = AngleBetween(sunDir, Ogre::Vector3f(0, 1, 0));
  float elevation = Ogre::Math::HALF_PI - thetaS;
  for (int i = 0; i < 3; i++) states[i] = arhosek_rgb_skymodelstate_alloc_init(turbidity, albedo[i], elevation);

  for (int i = 0; i < 9; i++)
    for (int j = 0; j < 3; j++) hosekParams[i][j] = states[j]->configs[j][i];
  hosekParams[9] = Ogre::Vector3(states[0]->radiances[0], states[1]->radiances[1], states[2]->radiances[2]);

  auto skyMaterial = Ogre::MaterialManager::getSingleton().getByName("SkyBox");
  auto fp = skyMaterial->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
  fp->setIgnoreMissingParams(true);

  for (int i = 0; i < 10; i++)
    for (int j = 0; j < 3; j++) hosekParamsArray[3 * i + j] = hosekParams[i][j];
  fp->setNamedConstant("HosekParams", hosekParamsArray, 10 * 3);
}

void CompositorManager::SetSleep(bool sleep) { _sleep = sleep; }

void CompositorManager::compositorInstanceCreated(Ogre::CompositorInstance *newInstance) { newInstance->addListener(this); }

void CompositorManager::compositorInstanceDestroyed(Ogre::CompositorInstance *destroyedInstance) { destroyedInstance->removeListener(this); }

void CompositorManager::preRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) {
  string name = evt.source->getName();
  if (name.find("reflection") != string::npos) {
    if (plane.getSide(camera->getRealPosition()) != Ogre::Plane::NEGATIVE_SIDE)
      camera->enableCustomNearClipPlane(Ogre::Plane(plane.normal, -plane.d));
    else
      camera->enableCustomNearClipPlane(Ogre::Plane(-plane.normal, -plane.d));
    camera->enableReflection(Ogre::Plane(plane.normal, -plane.d));
  }
}

void CompositorManager::postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) {
  string name = evt.source->getName();
  if (name.find("reflection") != string::npos) {
    camera->disableCustomNearClipPlane();
    camera->disableReflection();
  }
}

void CompositorManager::OnSetUp() {
  // init fields
  compositorManager = Ogre::CompositorManager::getSingletonPtr();
  compositorManager->registerCompositorLogic("DeferredLogic", this);
  ASSERTION(compositorManager, "[CompositorManager] compositorManager not initialised");
  sceneManager = Ogre::Root::getSingleton().getSceneManager("Default");
  ASSERTION(sceneManager, "[CompositorManager] sceneManager not initialised");
  camera = sceneManager->getCamera("Camera");
  ASSERTION(camera, "[CompositorManager] camera not initialised");
  compositorManager->registerCustomCompositionPass("RenderShadowMap", new RenderShadowsPass);

  viewport = camera->getViewport();
  compositorChain = compositorManager->getCompositorChain(viewport);
  cubeCamera = sceneManager->createCamera("CubeCamera");
  cubeCamera->setFOVy(Ogre::Degree(90.0));
  cubeCamera->setAspectRatio(1.0);
  cubeCamera->setNearClipDistance(camera->getNearClipDistance());
  cubeCamera->setFarClipDistance(camera->getFarClipDistance());
  sceneManager->getRootSceneNode()->createChildSceneNode(Ogre::Vector3::ZERO)->attachObject(cubeCamera);
  AddCompositor("CubeMap", true, 0);
  auto *rt = compositorChain->getCompositor("CubeMap")->getRenderTarget("cube");
  rt->removeAllViewports();
  rt->removeAllListeners();
  rt->addViewport(cubeCamera);
  rt->addListener(this);

  AddCompositor("MRT", true);
  AddCompositor("SSAO", false);
  AddCompositor("Copyback", true);
  AddCompositor("FXAA", false);
  if (!RenderSystemIsGLES2()) AddCompositor("SMAA", false);
  AddCompositor("HDR", false);
  AddCompositor("MB", false);

  // reg as viewport listener
  viewport->addListener(this);
  fixedViewportSize = false;
  sceneManager->addRenderObjectListener(this);
}

void CompositorManager::OnClean() { viewport->removeListener(this); }

void CompositorManager::AddCompositor(const string &name, bool enable, int position) {
  auto *compositor = compositorManager->addCompositor(viewport, name, position);
  ASSERTION(compositor, "[CompositorManager] Failed to add compositor " + name);
  compositorManager->setCompositorEnabled(viewport, name, enable);
}

void CompositorManager::EnableCompositor(const string &name, bool enable) {
  ASSERTION(compositorChain->getCompositorPosition(name) != Ogre::CompositorChain::NPOS, "[CompositorManager] Compositor not found: " + name);
  compositorManager->setCompositorEnabled(viewport, name, enable);
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
        // jt->width = 0;
        // jt->height = 0;
      }
    }

    compositorManager->setCompositorEnabled(viewport, compositorName, enabled);
  }
}

////  source https://wiki.ogre3d.org/GetScreenspaceCoords
// static Ogre::Vector4 GetScreenspaceCoords(Ogre::MovableObject *object, Ogre::Camera *camera) {
//   if (!object->isInScene()) {
//     return Ogre::Vector4::ZERO;
//   }
//
//   const Ogre::AxisAlignedBox &AABB = object->getWorldBoundingBox(true);
//
//   /**
//    * If you need the point above the object instead of the center point:
//    * This snippet derives the average point between the top-most corners of the bounding box
//    * Ogre::Vector3 point = (AABB.getCorner(AxisAlignedBox::FAR_LEFT_TOP)
//    *    + AABB.getCorner(AxisAlignedBox::FAR_RIGHT_TOP)
//    *    + AABB.getCorner(AxisAlignedBox::NEAR_LEFT_TOP)
//    *    + AABB.getCorner(AxisAlignedBox::NEAR_RIGHT_TOP)) / 4;
//    */
//
//   // Get the center point of the object's bounding box
//   Ogre::Vector3 center = AABB.getCenter();
//   Ogre::Vector4 point = Ogre::Vector4(center, 1.0);
//
//   // Is the camera facing that point? If not, return false
//   Ogre::Plane cameraPlane = Ogre::Plane(camera->getDerivedOrientation().zAxis(), camera->getDerivedPosition());
//   if (cameraPlane.getSide(center) != Ogre::Plane::NEGATIVE_SIDE) {
//     return Ogre::Vector4::ZERO;
//   }
//
//   // Transform the 3D point into screen space
//   point = Ogre::Matrix4::CLIPSPACE2DTOIMAGESPACE * camera->getProjectionMatrixWithRSDepth() * camera->getViewMatrix() * point;
//   point /= point.w;
//
//   return point;
// }
//
// static Ogre::Vector4 GetLightScreenSpaceCoords(Ogre::Light *light, Ogre::Camera *camera) {
//   Ogre::Vector4 point = Ogre::Vector4::ZERO;
//
//   if (light->getType() == Ogre::Light::LT_DIRECTIONAL)
//     point = Ogre::Vector4(-light->getDerivedDirection(), 0.0);
//   else
//     point = Ogre::Vector4(light->getDerivedPosition(), 1.0);
//
//   Ogre::Vector3 v = point.xyz().normalisedCopy();
//   Ogre::Vector3 l = camera->getDerivedOrientation().zAxis().normalisedCopy();
//   point = Ogre::Matrix4::CLIPSPACE2DTOIMAGESPACE * camera->getProjectionMatrix() * camera->getViewMatrix() * point;
//   point /= point.w;
//
//   if (light->getType() == Ogre::Light::LT_DIRECTIONAL)
//     point.w = Ogre::Math::saturate(-v.dotProduct(l));
//   else
//     point.w = Ogre::Math::saturate(v.dotProduct(l));
//
//   point.w = Ogre::Math::Abs(Ogre::Math::Sin(point.w * Ogre::Math::HALF_PI));
//
//   return point;
// }

void CompositorManager::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat) {
  const auto &fp = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
  fp->setIgnoreMissingParams(true);

  if (pass_id == 10) {
    float far = camera->getFarClipDistance();
    float near = camera->getNearClipDistance();
    Vector4f ZBufferParams = Vector4f(1.0 - far / near, far / near, (1.0 - far / near) / far, 1.0 / near);

    fp->setNamedConstant("ZBufferParams", ZBufferParams);

  } else if (pass_id == 30) {
    float far = camera->getFarClipDistance();
    float near = camera->getNearClipDistance();
    Vector4f ZBufferParams = Vector4f(1.0 - far / near, far / near, (1.0 - far / near) / far, 1.0 / near);

    fp->setNamedConstant("ZBufferParams", ZBufferParams);
    //    fp->setNamedConstant("WorldViewProjMatrix", viewProj);
    //    fp->setNamedConstant("ViewProjPrev", viewProjPrev);
    fp->setNamedConstant("InvViewMatrix", camera->getViewMatrix().inverse());

    //  } else if (pass_id == 12) {  // 12 = GodRays
    //    const auto &fp = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
    //    for (auto *l : sceneManager->_getLightsAffectingFrustum()) {
    //      if (l->getType() == Light::LT_DIRECTIONAL) {
    //        fp->setNamedConstant("LightPosition", GetLightScreenSpaceCoords(l, camera));
    //        break;
    //      }
    //    }
    //
    //  } else if (pass_id == 99) {  // 99 = FullScreenBlur
  }
}

void CompositorManager::notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat) {}

void CompositorManager::notifyResourcesCreated(bool forResizeOnly) {
  if (IsCompositorInChain("Fresnel")) {
    auto *rt = compositorChain->getCompositor("Fresnel")->getRenderTarget("reflection");
    if (rt) {
      rt->removeAllListeners();
      rt->addListener(this);
    }
  }
}

void CompositorManager::notifyRenderSingleObject(Ogre::Renderable *rend, const Ogre::Pass *pass, const Ogre::AutoParamDataSource *source,
                                                 const Ogre::LightList *pLightList, bool suppressRenderStateChanges) {
  if (!pass->getLightingEnabled() || !pass->getPolygonModeOverrideable()) return;

  const auto &fp = pass->getFragmentProgramParameters();
  fp->setIgnoreMissingParams(true);
  const auto &vp = pass->getVertexProgramParameters();
  vp->setIgnoreMissingParams(true);

  if (sceneManager->getShadowTechnique() != Ogre::SHADOWTYPE_NONE) {
    const Ogre::PSSMShadowCameraSetup::SplitPointList &splits =
        dynamic_cast<Ogre::PSSMShadowCameraSetup *>(sceneManager->getShadowCameraSetup().get())->getSplitPoints();
    fp->setNamedConstant("PssmSplitPoints", Vector4f(splits[1], splits[2], splits[3], sceneManager->getShadowFarDistance()));
  }
  if (source->getViewportHeight() == source->getViewportWidth() && source->getViewportHeight() != viewport->getHeight()) {
    for (auto *it : pass->getTextureUnitStates())
      if (it->getContentType() == Ogre::TextureUnitState::CONTENT_COMPOSITOR) it->setContentType(Ogre::TextureUnitState::CONTENT_NAMED);
    return;
  }

  Ogre::Matrix4 MVP;
  rend->getWorldTransforms(&MVP);
  Ogre::Matrix4 viewProj = Ogre::Matrix4::CLIPSPACE2DTOIMAGESPACE * camera->getProjectionMatrix() * camera->getViewMatrix() * MVP;
  Ogre::Any prevProj = rend->getUserObjectBindings().getUserAny();
  rend->getUserObjectBindings().setUserAny(viewProj);
  if (prevProj.has_value()) vp->setNamedConstant("WorldViewProjPrev", Ogre::any_cast<Ogre::Matrix4>(prevProj));

  if (auto *tex = pass->getTextureUnitState("ReflectionTex")) {
    if (tex->getContentType() != Ogre::TextureUnitState::CONTENT_COMPOSITOR) {
      tex->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
      if (!IsCompositorEnabled("Fresnel")) AddCompositor("Fresnel", true, 0);
      tex->setCompositorReference("Fresnel", "reflection");
      tex->setProjectiveTexturing(true, camera);
    }
  }

  if (!IsCompositorEnabled("MRT")) return;

  if (auto *tex = pass->getTextureUnitState("IBL")) {
    //  if (IsCompositorEnabled("CubeMap")) {
    //    auto &cube = compositorChain->getCompositor("CubeMap")->getTextureInstance("cube", 0);
    //    auto ibl = compositorChain->getCompositor("MRT")->getTextureInstance("ibl", 0);
    //    static bool copied = false;
    //    if (!copied) {
    //      cube->copyToTexture(ibl);
    //      copied = true;
    //    }
    //  }

    if (tex->getContentType() != Ogre::TextureUnitState::CONTENT_COMPOSITOR) {
      tex->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
      if (IsCompositorEnabled("CubeMap")) tex->setCompositorReference("CubeMap", "cube");
    }
  }

  if (auto *tex = pass->getTextureUnitState("SSAO")) {
    if (tex->getContentType() != Ogre::TextureUnitState::CONTENT_COMPOSITOR) {
      tex->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
      tex->setCompositorReference("MRT", "ao");
    }
  }
}

}  // namespace gge
