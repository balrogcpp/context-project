/// created by Andrey Vasiliev

#include "pch.h"
#include "SceneManager.h"
#include "ForestsManager.h"
#include "PhysicsManager.h"
#include "TerrainManager.h"
#include "VideoManager.h"
#include <PagedGeometry/PagedGeometryAll.h>

using namespace std;

namespace {
inline void UpgradeTransparentShadowCaster(const Ogre::Material *material) {
  Ogre::LogManager::getSingleton().logMessage("[ScanNode] UpgradeTransparentShadowCaster: " + material->getName());

  auto *tech = material->getTechnique(0);
  auto *pass = tech->getPass(0);

  if (!material->getTransparencyCastsShadows() || !pass->getNumTextureUnitStates() || !pass->getAlphaRejectValue() ||
      !pass->getTextureUnitState("Albedo")) {
    return;
  }

  const std::string baseCaster = "PSSM/shadow_caster_alpha";
  auto casterMaterial = Ogre::MaterialManager::getSingleton().getByName(baseCaster);
  std::string newCasterName = baseCaster + "/" + material->getName();
  auto newCaster = Ogre::MaterialManager::getSingleton().getByName(newCasterName);

  if (!newCaster) {
    newCaster = casterMaterial->clone(newCasterName);
    std::string albedoTexture = pass->getTextureUnitState("Albedo")->getTextureName();
    auto *newPass = newCaster->getTechnique(0)->getPass(0);
    auto *baseColor = newPass->getTextureUnitState("Albedo");

    baseColor->setContentType(Ogre::TextureUnitState::CONTENT_NAMED);
    baseColor->setTextureName(albedoTexture);
    newPass->setCullingMode(pass->getCullingMode());
    newPass->setManualCullingMode(pass->getManualCullingMode());
    newPass->setAlphaRejectValue(pass->getAlphaRejectValue());

    tech->setShadowCasterMaterial(newCaster);
  }
}

inline void CheckTransparentShadowCaster(const std::string &material) {
  UpgradeTransparentShadowCaster(Ogre::MaterialManager::getSingleton().getByName(material).get());
}

inline bool HasNoTangentsAndCanGenerate(Ogre::VertexDeclaration *vertex_declaration) {
  bool hasNormals = false;
  bool hasTangents = false;
  bool hasUVs = false;
  auto &elementList = vertex_declaration->getElements();
  auto it = elementList.begin();
  auto end = elementList.end();

  while (it != end && !hasTangents) {
    const auto &vertexElem = *it;
    if (vertexElem.getSemantic() == Ogre::VES_TANGENT) hasTangents = true;
    if (vertexElem.getSemantic() == Ogre::VES_NORMAL) hasNormals = true;
    if (vertexElem.getSemantic() == Ogre::VES_TEXTURE_COORDINATES) hasUVs = true;

    ++it;
  }

  return !hasTangents && hasUVs && hasNormals;
}

inline void EnsureHasTangents(const Ogre::MeshPtr &mesh) {
  bool generateTangents = false;

  if (mesh->sharedVertexData) {
    auto *vertexDecl = mesh->sharedVertexData->vertexDeclaration;
    generateTangents |= HasNoTangentsAndCanGenerate(vertexDecl);
  }

  for (int i = 0; i < mesh->getNumSubMeshes(); ++i) {
    auto *subMesh = mesh->getSubMesh(i);
    if (subMesh->vertexData) {
      auto *vertexDecl = subMesh->vertexData->vertexDeclaration;
      generateTangents |= HasNoTangentsAndCanGenerate(vertexDecl);
    }
  }

  if (generateTangents) {
    mesh->buildTangentVectors();
  }
}
}  // namespace

namespace gge {
SceneManager::SceneManager() : viewProj(Ogre::Matrix4::ZERO), viewProjPrev(Ogre::Matrix4::ZERO), pssmPoints(Ogre::Vector4::ZERO) {}
SceneManager::~SceneManager() { ogreSceneManager->removeRenderObjectListener(this); }

void SceneManager::OnSetUp() {
  ogreRoot = Ogre::Root::getSingletonPtr();
  ASSERTION(ogreRoot, "[SceneManager] ogreRoot is not initialised");
  ogreSceneManager = ogreRoot->getSceneManager("Default");
  ASSERTION(ogreSceneManager, "[SceneManager] ogreSceneManager is not initialised");
  ASSERTION(ogreSceneManager->hasCamera("Default"), "[SceneManager] ogreCamera is not initialised");
  ogreCamera = ogreSceneManager->getCamera("Default");

  ogreSceneManager->addRenderObjectListener(this);
}

void SceneManager::OnClean() {
  ogreSceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
  ogreSceneManager->clearScene();
  InputSequencer::GetInstance().UnregDeviceListener(sinbad.get());
  sinbad.reset();
}

void SceneManager::OnUpdate(float time) {
  if (sinbad) {
    sinbad->Update(time);
  }

  viewProjPrev = viewProj;
  viewProj = ogreCamera->getProjectionMatrixWithRSDepth() * ogreCamera->getViewMatrix();

  if (ogreSceneManager->getShadowTechnique() != Ogre::SHADOWTYPE_NONE) {
    auto *pssm = static_cast<Ogre::PSSMShadowCameraSetup *>(ogreSceneManager->getShadowCameraSetup().get());
    const Ogre::PSSMShadowCameraSetup::SplitPointList &splitPointList = pssm->getSplitPoints();
    pssmPoints.w = ogreSceneManager->getShadowFarDistance();
    for (unsigned int j = 0; j < 3; j++) {
      pssmPoints[j] = splitPointList[j + 1];
    }
  } else {
    pssmPoints = Ogre::Vector4(0.0);
  }
}

static void ScanForests(const Ogre::UserObjectBindings &objBindings, const std::string &base) {
  auto &forests = GetComponent<ForestsManager>();
  unsigned long long generator = 0;
  std::string key = base + to_string(generator++);
  Ogre::Any value = objBindings.getUserAny(key);

  while (value.has_value()) {
    forests.RegPagedGeometry(Ogre::any_cast<Forests::PagedGeometry *>(value));
    key = base + to_string(generator++);
    value = objBindings.getUserAny(key);
  }
}

void SceneManager::LoadFromFile(const std::string &filename) {
  auto *rootNode = ogreSceneManager->getRootSceneNode();
  rootNode->loadChildren(filename);

  for (auto it : rootNode->getChildren()) {
    ScanNode(static_cast<Ogre::SceneNode *>(it));
  }

  if (!sinbad && ogreSceneManager->hasCamera("Default")) {
    sinbad = make_unique<SinbadCharacterController>(ogreSceneManager->getCamera("Default"));
    InputSequencer::GetInstance().RegDeviceListener(sinbad.get());
  }

  // search for TerrainGroup
  const auto &objBindings = ogreSceneManager->getRootSceneNode()->getUserObjectBindings();
  if (objBindings.getUserAny("TerrainGroup").has_value()) {
    auto *terrainGlobalOptions = Ogre::TerrainGlobalOptions::getSingletonPtr();
    auto *terrainGroup = Ogre::any_cast<Ogre::TerrainGroup *>(objBindings.getUserAny("TerrainGroup"));

    if (ogreSceneManager->hasLight("Sun")) {
      terrainGlobalOptions->setLightMapDirection(ogreSceneManager->getLight("Sun")->getDerivedDirection());
    }

    for (auto it = terrainGroup->getTerrainIterator(); it.hasMoreElements();) {
      auto *terrain = it.getNext()->instance;
    }

    GetComponent<TerrainManager>().RegTerrainGroup(terrainGroup);
    GetComponent<TerrainManager>().ProcessTerrainCollider(terrainGroup);
  }

  // search for GrassPage
  ScanForests(objBindings, "GrassPage");
  ScanForests(objBindings, "BatchPage");

  // scan second time, new objects added during first scan
  for (auto it : rootNode->getChildren()) {
    ScanNode(static_cast<Ogre::SceneNode *>(it));
  }
}

void SceneManager::ScanCamera(Ogre::Camera *camera) {
  Ogre::SceneNode *node = camera->getParentSceneNode();
  Ogre::Vector3 position = node->getPosition();
  node->translate(0.0, GetComponent<TerrainManager>().GetHeight(position.x, position.z), 0.0);
}

void SceneManager::ScanLight(Ogre::Light *light) {
  Ogre::SceneNode *node = light->getParentSceneNode();
  Ogre::Vector3 position = node->getPosition();
  node->translate(0.0, GetComponent<TerrainManager>().GetHeight(position.x, position.z), 0.0);
}

void SceneManager::ScanEntity(const std::string &name) { ScanEntity(ogreSceneManager->getEntity(name)); }

void SceneManager::ScanEntity(Ogre::Entity *entity) {
  EnsureHasTangents(entity->getMesh());

  if (entity->getName().rfind("GrassLDR", 0)) {
    Ogre::SceneNode *node = entity->getParentSceneNode();
    Ogre::Vector3 position = node->getPosition();
    node->translate(0.0, GetComponent<TerrainManager>().GetHeight(position.x, position.z), 0.0);
  }

  if (entity->getMesh()->isReloadable()) {
    static unsigned long long generator = 0;
    for (auto &it : entity->getSubEntities()) {
      const auto &old = it->getMaterial();
      const auto &mat = old->clone(std::to_string(generator++));

      if (0) {
        auto *pass = mat->getTechnique(0)->getPass(0);
        auto &vp = pass->getVertexProgram();
        auto &fp = pass->getFragmentProgram();

        std::string vpDefines = vp->getParameter("preprocessor_defines");
        std::string fpDefines = fp->getParameter("preprocessor_defines");

        if (auto *tex = pass->getTextureUnitState("Albedo")) {
          if (tex->getTextureName() == "white.dds") {
            auto i = fpDefines.find("HAS_BASECOLORMAP");
            if (i != std::string::npos) fpDefines[i] = 'X';
          }
        }

        if (auto *tex = pass->getTextureUnitState("Normal")) {
          if (tex->getTextureName() == "normal.dds") {
            auto i = fpDefines.find("HAS_NORMALMAP");
            if (i != std::string::npos) fpDefines[i] = 'X';
          }
        }

        if (auto *tex = pass->getTextureUnitState("ORM")) {
          if (tex->getTextureName() == "white.dds") {
            auto i = fpDefines.find("HAS_ORM");
            if (i != std::string::npos) fpDefines[i] = 'X';
          }
        }

        if (auto *tex = pass->getTextureUnitState("Emissive")) {
          if (tex->getTextureName() == "black.dds") {
            auto i = fpDefines.find("HAS_EMISSIVEMAP");
            if (i != std::string::npos) fpDefines[i] = 'X';
          }
        }

        vp->setParameter("preprocessor_defines", vpDefines);
        fp->setParameter("preprocessor_defines", fpDefines);
        vp->reload();
        fp->reload();
      }

      it->setMaterial(mat);
    }
  }

  if (entity->hasSkeleton()) {
    for (auto it : entity->getAttachedObjects()) {
      if (auto camera = dynamic_cast<Ogre::Camera *>(it)) {
        Ogre::LogManager::getSingleton().logMessage("[ScanNode] AnimatedEntity: " + entity->getName() + "  Camera: " + it->getName());
        ScanCamera(camera);
        continue;
      }

      if (auto light = dynamic_cast<Ogre::Light *>(it)) {
        Ogre::LogManager::getSingleton().logMessage("[ScanNode] AnimatedEntity: " + entity->getName() + "  Light: " + it->getName());
        ScanLight(light);
        continue;
      }

      if (auto entity = dynamic_cast<Ogre::Entity *>(it)) {
        Ogre::LogManager::getSingleton().logMessage("[ScanNode] AnimatedEntity: " + entity->getName() + "  Entity: " + it->getName());
        ScanEntity(entity);
        continue;
      }
    }
  }

  for (const auto &it : entity->getSubEntities()) {
    // do smth
  }
  auto objBindings = entity->getUserObjectBindings();
  if (objBindings.getUserAny("proxy").has_value()) {
    gge::GetComponent<gge::PhysicsManager>().ProcessData(entity);
  }
}

void SceneManager::ScanNode(Ogre::SceneNode *node) {
  for (auto it : node->getAttachedObjects()) {
    if (auto camera = dynamic_cast<Ogre::Camera *>(it)) {
      Ogre::LogManager::getSingleton().logMessage("[ScanNode] Node: " + node->getName() + "  Camera: " + it->getName());
      ScanCamera(camera);
      continue;
    }

    if (auto light = dynamic_cast<Ogre::Light *>(it)) {
      Ogre::LogManager::getSingleton().logMessage("[ScanNode] Node: " + node->getName() + "  Light: " + it->getName());
      ScanLight(light);
      continue;
    }

    if (auto entity = dynamic_cast<Ogre::Entity *>(it)) {
      Ogre::LogManager::getSingleton().logMessage("[ScanNode] Node: " + node->getName() + "  Entity: " + it->getName());
      ScanEntity(entity);
      continue;
    }

    if (auto geometry = dynamic_cast<Forests::BatchedGeometry *>(it)) {
      Ogre::LogManager::getSingleton().logMessage("[ScanNode] Node: " + node->getName() + "  Forests::BatchedGeometry: " + it->getName());
      auto it = geometry->getSubBatchIterator();
      while (it.hasMoreElements()) {
        it.getNext();
      }
      continue;
    }
  }

  // recurse
  for (auto it : node->getChildren()) {
    ScanNode(static_cast<Ogre::SceneNode *>(it));
  }
}

void SceneManager::notifyRenderSingleObject(Ogre::Renderable *rend, const Ogre::Pass *pass, const Ogre::AutoParamDataSource *source,
                                            const Ogre::LightList *pLightList, bool suppressRenderStateChanges) {
  if (_sleep || !pLightList || !pass || suppressRenderStateChanges) {
    return;
  }

  if (!pass->hasVertexProgram() || !pass->hasFragmentProgram() || !pass->getLightingEnabled() || pass->getFogOverride()) {
    return;
  }

  const auto &vp = pass->getVertexProgramParameters();
  const auto &fp = pass->getFragmentProgramParameters();
  vp->setIgnoreMissingParams(true);
  fp->setIgnoreMissingParams(true);

  // apply for entities, skip grass
  if (auto *subentity = dynamic_cast<Ogre::SubEntity *>(rend)) {
    auto *entity = subentity->getParent();
    if (!entity->isInScene() || !entity->isVisible()) {
      return;
    }

    if (entity->getMesh()->isReloadable()) {
      Ogre::Matrix4 MVP;
      Ogre::Any value = rend->getUserAny();
      rend->getWorldTransforms(&MVP);
      rend->setUserAny(viewProj * MVP);

      if (value.has_value()) {
        vp->setNamedConstant("uWorldViewProjPrev", Ogre::any_cast<Ogre::Matrix4>(value));
        vp->setNamedConstant("uStaticObj", Ogre::Real(0.0));
        vp->setNamedConstant("uMovableObj", Ogre::Real(1.0));
      }

    } else {
      vp->setNamedConstant("uWorldViewProjPrev", viewProjPrev);
      vp->setNamedConstant("uStaticObj", Ogre::Real(1.0));
      vp->setNamedConstant("uMovableObj", Ogre::Real(0.0));
    }
  }

  // skip this part
  // else if (dynamic_cast<Forests::BatchedGeometry::SubBatch *>(rend)) {}
  // else if (dynamic_cast<Ogre::StaticGeometry::GeometryBucket *>(rend)) {}
  // else if (dynamic_cast<Ogre::TerrainQuadTreeNode *>(rend)) {}

  else {
    vp->setNamedConstant("uWorldViewProjPrev", viewProjPrev);
    vp->setNamedConstant("uStaticObj", Ogre::Real(1.0));
    vp->setNamedConstant("uMovableObj", Ogre::Real(0.0));
  }

  fp->setNamedConstant("uPssmSplitPoints", pssmPoints);

  vp->setIgnoreMissingParams(false);
  fp->setIgnoreMissingParams(false);
}

}  // namespace gge
