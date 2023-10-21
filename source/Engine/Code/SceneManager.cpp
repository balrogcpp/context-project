/// created by Andrey Vasiliev

#include "pch.h"
#include "SceneManager.h"
#include "SystemLocator.h"
#include <PagedGeometry/PagedGeometryAll.h>

using namespace std;

namespace {
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
SceneManager::SceneManager()
    : viewProj(Ogre::Matrix4::IDENTITY), viewProjPrev(Ogre::Matrix4::IDENTITY), pssmPoints(Ogre::Vector4::ZERO), isEven(false) {}
SceneManager::~SceneManager() { ogreSceneManager->removeRenderObjectListener(this); }

void SceneManager::OnSetUp() {
  ogreRoot = Ogre::Root::getSingletonPtr();
  ASSERTION(ogreRoot, "[SceneManager] ogreRoot is not initialised");
  ogreSceneManager = ogreRoot->getSceneManager("Default");
  ASSERTION(ogreSceneManager, "[SceneManager] ogreSceneManager is not initialised");
  ASSERTION(ogreSceneManager->hasCamera("Camera"), "[SceneManager] ogreCamera is not initialised");
  ogreCamera = ogreSceneManager->getCamera("Camera");

  ogreSceneManager->addRenderObjectListener(this);
}

void SceneManager::OnClean() {
  ogreSceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
  ogreSceneManager->clearScene();
  InputSequencer::GetInstance().UnregDeviceListener(sinbad.get());
  sinbad.reset();
}

void SceneManager::OnUpdate(float time) {
  if (sinbad) sinbad->Update(time);

  viewProjPrev = viewProj;
  viewProj = ogreCamera->getProjectionMatrix() * ogreCamera->getViewMatrix();
  isEven = !isEven;

  if (ogreSceneManager->getShadowTechnique() != Ogre::SHADOWTYPE_NONE) {
    auto *pssm = dynamic_cast<Ogre::PSSMShadowCameraSetup *>(ogreSceneManager->getShadowCameraSetup().get());
    pssmCount = pssm->getSplitCount();
    const Ogre::PSSMShadowCameraSetup::SplitPointList &splitPointList = pssm->getSplitPoints();
    pssmPoints.w = ogreSceneManager->getShadowFarDistance();
    for (unsigned int i = 0; i < Ogre::Math::Clamp<Ogre::uint32>(pssmCount, 0, 4); i++) {
      pssmPoints[i] = splitPointList[i + 1];
    }
  } else {
    pssmPoints = Ogre::Vector4(0.0, 0.0, 0.0, ogreSceneManager->getShadowFarDistance());
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
    ScanNode(dynamic_cast<Ogre::SceneNode *>(it));
  }

  if (!sinbad && ogreSceneManager->hasCamera("Camera")) {
    sinbad = make_unique<SinbadCharacterController>(ogreSceneManager->getCamera("Camera"));
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

  // sky is separate component
  GetComponent<SkyManager>().SetUpSky();

  // scan second time, new objects added during first scan
  for (auto it : rootNode->getChildren()) {
    ScanNode(dynamic_cast<Ogre::SceneNode *>(it));
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

  if (!entity->getMesh()->isReloadable()) {
    return;
  }

  static unsigned long long generator = 0;
  for (auto &it : entity->getSubEntities()) {
    const auto &old = it->getMaterial();
    const auto &mat = old->clone(std::to_string(generator++));

    auto *pass = mat->getTechnique(0)->getPass(0);
    const auto &vp = pass->getVertexProgramParameters();
    const auto &fp = pass->getFragmentProgramParameters();
    string vpDefines = pass->getVertexProgram()->getParameter("preprocessor_defines");
    string fpDefines = pass->getFragmentProgram()->getParameter("preprocessor_defines");

    if (ogreSceneManager->getShadowTechnique() != Ogre::SHADOWTYPE_NONE) {
      auto *pssm = dynamic_cast<Ogre::PSSMShadowCameraSetup *>(ogreSceneManager->getShadowCameraSetup().get());
      pssmCount = pssm->getSplitCount();
      if (fpDefines.find("PSSM_SPLIT_COUNT") == string::npos) fpDefines.append(",PSSM_SPLIT_COUNT=").append(to_string(pssmCount));
      if (vpDefines.find("PSSM_SPLIT_COUNT") == string::npos) vpDefines.append(",PSSM_SPLIT_COUNT=").append(to_string(pssmCount));
    } else {
      if (auto i = vpDefines.find("MAX_SHADOW_TEXTURES"); i != string::npos) vpDefines[i] = 'X';
      if (auto i = fpDefines.find("MAX_SHADOW_TEXTURES"); i != string::npos) fpDefines[i] = 'X';
    }

    pass->getVertexProgram()->setParameter("preprocessor_defines", vpDefines);
    pass->getFragmentProgram()->setParameter("preprocessor_defines", fpDefines);
    pass->getVertexProgram()->reload();
    pass->getFragmentProgram()->reload();
    it->setMaterial(mat);
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
    ScanNode(dynamic_cast<Ogre::SceneNode *>(it));
  }
}

void SceneManager::notifyRenderSingleObject(Ogre::Renderable *rend, const Ogre::Pass *pass, const Ogre::AutoParamDataSource *source,
                                            const Ogre::LightList *pLightList, bool suppressRenderStateChanges) {
  if (!pass || !pass->hasVertexProgram() || !pass->hasFragmentProgram() || !pass->getLightingEnabled() || pass->getFogOverride()) return;
  const auto &vp = pass->getVertexProgramParameters();
  const auto &fp = pass->getFragmentProgramParameters();
  vp->setIgnoreMissingParams(true);
  fp->setIgnoreMissingParams(true);

  fp->setNamedConstant("PssmSplitPoints", pssmPoints);

  // apply for dynamic entities only
  if (auto *subentity = dynamic_cast<Ogre::SubEntity *>(rend)) {
    auto *entity = subentity->getParent();
    auto mass = entity->getUserObjectBindings().getUserAny("mass");
    if (!mass.has_value()) return;
    if (Ogre::any_cast<Ogre::Real>(mass) == 0.0) return;
    if (entity->getMesh()->isReloadable()) {
      Ogre::Matrix4 MVP;
      rend->getWorldTransforms(&MVP);
      Ogre::Any prevMVP = rend->getUserObjectBindings().getUserAny();
      rend->getUserObjectBindings().setUserAny(viewProj * MVP);
      if (prevMVP.has_value()) vp->setNamedConstant("WorldViewProjPrev", Ogre::any_cast<Ogre::Matrix4>(prevMVP));
    }
  }
}

}  // namespace gge
