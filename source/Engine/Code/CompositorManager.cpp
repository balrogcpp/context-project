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
  viewProjPrev = viewProj;
  viewProj = camera->getProjectionMatrix() * camera->getViewMatrix();

  if (sceneManager->getShadowTechnique() != Ogre::SHADOWTYPE_NONE) {
    auto *pssm = dynamic_cast<Ogre::PSSMShadowCameraSetup *>(sceneManager->getShadowCameraSetup().get());
    int pssmCount = pssm->getSplitCount();
    const Ogre::PSSMShadowCameraSetup::SplitPointList &splitPointList = pssm->getSplitPoints();
    pssmPoints.w = sceneManager->getShadowFarDistance();
    for (unsigned int i = 0; i < Ogre::Math::Clamp<Ogre::uint32>(pssmCount, 0, 4); i++) {
      pssmPoints[i] = splitPointList[i + 1];
    }
  } else {
    pssmPoints = Ogre::Vector4(0.0, 0.0, 0.0, sceneManager->getShadowFarDistance());
  }

  pssmChanged = pssmPointsPrev != pssmPoints;
  pssmPointsPrev = pssmPoints;

  ArHosekSkyModelState *states[3];
  Ogre::Vector3f sunDir;
  if (sceneManager->hasLight("Sun")) {
    auto *SunPtr = sceneManager->getLight("Sun");
    sunDir = SunPtr ? -SunPtr->getDerivedDirection().normalisedCopy() : Ogre::Vector3::ZERO;
  } else {
    sunDir = Ogre::Vector3::ZERO;
  }

  Ogre::Vector3f albedo = Ogre::Vector3f(1.0f);
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
  Ogre::Real hosekParamsArray[10 * 3];
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
    camera->enableCustomNearClipPlane(Ogre::Plane(plane.normal, -plane.d));
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
  compositorManager->registerCustomCompositionPass("RenderShadowMap", OGRE_NEW RenderShadowsPass);

  viewport = camera->getViewport();
  compositorChain = compositorManager->getCompositorChain(viewport);
  cubeCamera = sceneManager->createCamera("CubeCamera");
  cubeCamera->setFOVy(Ogre::Degree(90.0));
  cubeCamera->setAspectRatio(1.0);
  cubeCamera->setNearClipDistance(camera->getFarClipDistance() / 6.0);
  cubeCamera->setFarClipDistance(camera->getFarClipDistance());
  sceneManager->getRootSceneNode()->createChildSceneNode(Ogre::Vector3::UNIT_Y)->attachObject(cubeCamera);
  AddCompositor("CubeMap", true);
  auto *rt = compositorChain->getCompositor("CubeMap")->getRenderTarget("cube");
  rt->removeAllViewports();
  rt->addViewport(cubeCamera);
  rt->addListener(this);
  AddCompositor("CubeMapSky", true);
  auto *rt1 = compositorChain->getCompositor("CubeMapSky")->getRenderTarget("cube");
  rt1->removeAllViewports();
  rt1->addViewport(cubeCamera);
  rt1->addListener(this);

  AddCompositor("MRT", true);
  AddCompositor("SSR", false);
  AddCompositor("SSAO", !RenderSystemIsGLES2());
  if (!RenderSystemIsGLES2()) AddCompositor("KinoFog", true);
  if (!RenderSystemIsGLES2()) AddCompositor("GodRays", true);
  if (!RenderSystemIsGLES2()) AddCompositor("Lens", false);
  if (!RenderSystemIsGLES2()) AddCompositor("Glow", true);
  if (!RenderSystemIsGLES2()) AddCompositor("HDR", true);
  if (!RenderSystemIsGLES2()) AddCompositor("SMAA", false);
  AddCompositor("FXAA", true);
  AddCompositor("Tonemap", false);
  AddCompositor("MotionBlur", false);
  AddCompositor("Pause", false);

  // reg as viewport listener
  viewport->addListener(this);
  fixedViewportSize = false;
  sceneManager->addRenderObjectListener(this);
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
  if (point.x < -0.1 || point.x > 1.1 || point.y < -0.1 || point.y > 1.1) point.w = 0;

  return point;
}

void CompositorManager::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat) {
  if (pass_id == 10) {  // 10 = SSAO
    const auto &fp = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
    fp->setNamedConstant("ProjMatrix", Ogre::Matrix4::CLIPSPACE2DTOIMAGESPACE * camera->getProjectionMatrix());
    fp->setNamedConstant("ClipDistance", camera->getFarClipDistance() - camera->getNearClipDistance());

  } else if (pass_id == 20) {  // 20 = SSR
    const auto &fp = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
    fp->setNamedConstant("ProjMatrix", Ogre::Matrix4::CLIPSPACE2DTOIMAGESPACE * camera->getProjectionMatrix());
    fp->setNamedConstant("ClipDistance", camera->getFarClipDistance() - camera->getNearClipDistance());

  } else if (pass_id == 30) {  // 14 = Motion Blur
    const auto &fp = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
    fp->setNamedConstant("ViewProjPrev", Ogre::Matrix4::CLIPSPACE2DTOIMAGESPACE * viewProjPrev);
    fp->setNamedConstant("InvViewMatrix", camera->getViewMatrix().inverse());

  } else if (pass_id == 40) {  // 40 = KinoFog
    const auto &fp = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
    const auto &ll = sceneManager->_getLightsAffectingFrustum();
    fp->setNamedConstant("CameraPosition", camera->getRealPosition());
    fp->setNamedConstant("InvViewMatrix", camera->getViewMatrix().inverse());
    fp->setNamedConstant("ClipDistance", camera->getFarClipDistance() - camera->getNearClipDistance());
    if (!ll.empty()) fp->setNamedConstant("LightDir0", ll[0]->getDerivedDirection());
    auto *tex = mat->getTechnique(0)->getPass(0)->getTextureUnitState(2);
    if (tex->getContentType() != Ogre::TextureUnitState::CONTENT_COMPOSITOR) {
      tex->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
      tex->setCompositorReference("CubeMapSky", "cube");
    }

  } else if (pass_id == 12) {  // 12 = GodRays
    const auto &fp = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
    const auto &ll = sceneManager->_getLightsAffectingFrustum();
    if (!ll.empty()) fp->setNamedConstant("LightPosition", GetLightScreenSpaceCoords(ll[0], camera));

  } else if (pass_id == 99) {  // 99 = FullScreenBlur
  }
}

void CompositorManager::notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat) {}

void CompositorManager::notifyResourcesCreated(bool forResizeOnly) {
  if (IsCompositorInChain("Fresnel")) {
    auto *rt1 = compositorChain->getCompositor("Fresnel")->getRenderTarget("reflection");
    if (rt1) {
      rt1->removeAllListeners();
      rt1->addListener(this);
    }
  }
}
void CompositorManager::notifyRenderSingleObject(Ogre::Renderable *rend, const Ogre::Pass *pass, const Ogre::AutoParamDataSource *source,
                                                 const Ogre::LightList *pLightList, bool suppressRenderStateChanges) {
  if (!pass || !pass->hasVertexProgram() || !pass->hasFragmentProgram()) return;

  const auto &vp = pass->getVertexProgramParameters();
  const auto &fp = pass->getFragmentProgramParameters();
  vp->setIgnoreMissingParams(true);
  fp->setIgnoreMissingParams(true);

  if (!pass->getLightingEnabled() || pass->getFogOverride()) return;

  if (auto *tex = pass->getTextureUnitState("ShadowAtlas")) {
    if (tex->getContentType() != Ogre::TextureUnitState::CONTENT_COMPOSITOR) {
      tex->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
      tex->setCompositorReference("MRT", "shadowmap");
    }
  }
  if (auto *tex = pass->getTextureUnitState("IBL")) {
    if (tex->getContentType() != Ogre::TextureUnitState::CONTENT_COMPOSITOR) {
      tex->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
      tex->setCompositorReference("CubeMap", "cube");
    }
  }
  if (auto *tex = pass->getTextureUnitState("AO")) {
    if (tex->getContentType() != Ogre::TextureUnitState::CONTENT_COMPOSITOR) {
      tex->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
      tex->setCompositorReference("MRT", "ao");
    }
  }
  if (auto *tex = pass->getTextureUnitState("SSR")) {
    if (tex->getContentType() != Ogre::TextureUnitState::CONTENT_COMPOSITOR) {
      tex->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
      tex->setCompositorReference("MRT", "ssr");
    }
  }
  if (auto *tex = pass->getTextureUnitState("RefractionTex")) {
    if (tex->getContentType() != Ogre::TextureUnitState::CONTENT_COMPOSITOR) {
      tex->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
      tex->setCompositorReference("MRT", "rt", 0);
    }
  }
  if (auto *tex = pass->getTextureUnitState("DepthTex")) {
    if (tex->getContentType() != Ogre::TextureUnitState::CONTENT_COMPOSITOR) {
      tex->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
      tex->setCompositorReference("MRT", "rt", 1);
    }
  }
  if (auto *tex = pass->getTextureUnitState("ReflectionTex")) {
    if (tex->getContentType() != Ogre::TextureUnitState::CONTENT_COMPOSITOR) {
      tex->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
      if (!IsCompositorEnabled("Fresnel")) AddCompositor("Fresnel", true, 0);
      tex->setCompositorReference("Fresnel", "reflection");
      tex->setProjectiveTexturing(true, camera);
    }
  }

  if (sceneManager->getShadowTechnique() != Ogre::SHADOWTYPE_NONE && pssmChanged) {
    fp->setNamedConstant("PssmSplitPoints", pssmPoints);
  }

  // apply for dynamic entities only
  Ogre::Matrix4 MVP;
  rend->getWorldTransforms(&MVP);
  vp->setNamedConstant("WorldViewProjPrev", viewProjPrev * MVP);

  if (auto *subentity = dynamic_cast<Ogre::SubEntity *>(rend)) {
    auto *entity = subentity->getParent();
    auto mass = entity->getUserObjectBindings().getUserAny("mass");
    if (!mass.has_value()) return;
    if (Ogre::any_cast<Ogre::Real>(mass) == 0.0) return;
    if (entity->getMesh()->isReloadable()) {
      Ogre::Any prevMVP = rend->getUserObjectBindings().getUserAny();
      rend->getUserObjectBindings().setUserAny(MVP);
      if (prevMVP.has_value()) vp->setNamedConstant("WorldViewProjPrev", viewProjPrev * Ogre::any_cast<Ogre::Matrix4>(prevMVP));
    }
  }
}

}  // namespace gge
