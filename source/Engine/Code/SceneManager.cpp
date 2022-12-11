/// created by Andrey Vasiliev

#include "pch.h"
#include "SceneManager.h"
#include "ForestsManager.h"
#include "PhysicsManager.h"
#include "TerrainManager.h"
#include "VideoManager.h"
#include <PagedGeometry/PagedGeometryAll.h>
#include <Terrain/OgreTerrainQuadTreeNode.h>

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
  bool hasTangents = false;
  bool hasUVs = false;
  auto &elementList = vertex_declaration->getElements();
  auto it = elementList.begin();
  auto end = elementList.end();

  while (it != end && !hasTangents) {
    const auto &vertexElem = *it;
    if (vertexElem.getSemantic() == Ogre::VES_TANGENT) hasTangents = true;
    if (vertexElem.getSemantic() == Ogre::VES_TEXTURE_COORDINATES) hasUVs = true;

    ++it;
  }

  return !hasTangents && hasUVs;
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

namespace {
static Ogre::Matrix4 ViewProj;
static Ogre::Matrix4 ViewProjPrev;
}  // namespace

namespace Glue {
SceneManager::SceneManager() {}
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
  fpParams.clear();
  vpParams.clear();
}

void SceneManager::OnUpdate(float time) {
  if (sinbad) {
    sinbad->Update(time);
  }

  ViewProjPrev = ViewProj;
  ViewProj = ogreCamera->getProjectionMatrixWithRSDepth() * ogreCamera->getViewMatrix();
}

static inline void ScanForests(const Ogre::UserObjectBindings &objBindings, const std::string &base) {
  auto &forests = GetComponent<ForestsManager>();
  int i = 0;  // counter
  std::string key = base + to_string(i++);
  Ogre::Any value = objBindings.getUserAny(key);

  while (value.has_value()) {
    forests.RegPagedGeometry(Ogre::any_cast<Forests::PagedGeometry *>(value));
    key = base + to_string(i++);
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

void SceneManager::RegCamera(Ogre::Camera *camera) {
  Ogre::SceneNode *node = camera->getParentSceneNode();
  Ogre::Vector3 position = node->getPosition();
  node->translate(0.0, GetComponent<TerrainManager>().GetHeight(position.x, position.z), 0.0);
}

void SceneManager::RegLight(Ogre::Light *light) {
  Ogre::SceneNode *node = light->getParentSceneNode();
  Ogre::Vector3 position = node->getPosition();
  node->translate(0.0, GetComponent<TerrainManager>().GetHeight(position.x, position.z), 0.0);
}

void SceneManager::RegEntity(const std::string &name) { RegEntity(ogreSceneManager->getEntity(name)); }

void SceneManager::RegEntity(Ogre::Entity *entity) {
  // EnsureHasTangents(entity->getMesh());

  if (entity->getName().rfind("GrassLDR", 0)) {
    Ogre::SceneNode *node = entity->getParentSceneNode();
    Ogre::Vector3 position = node->getPosition();
    node->translate(0.0, GetComponent<TerrainManager>().GetHeight(position.x, position.z), 0.0);
  }

  if (entity->hasSkeleton()) {
    for (auto it : entity->getAttachedObjects()) {
      if (auto camera = dynamic_cast<Ogre::Camera *>(it)) {
        Ogre::LogManager::getSingleton().logMessage("[ScanNode] AnimatedEntity: " + entity->getName() + "  Camera: " + it->getName());
        RegCamera(camera);
        continue;
      }

      if (auto light = dynamic_cast<Ogre::Light *>(it)) {
        Ogre::LogManager::getSingleton().logMessage("[ScanNode] AnimatedEntity: " + entity->getName() + "  Light: " + it->getName());
        RegLight(light);
        continue;
      }

      if (auto entity = dynamic_cast<Ogre::Entity *>(it)) {
        Ogre::LogManager::getSingleton().logMessage("[ScanNode] AnimatedEntity: " + entity->getName() + "  Entity: " + it->getName());
        RegEntity(entity);
        continue;
      }
    }
  }

  auto objBindings = entity->getUserObjectBindings();
  if (objBindings.getUserAny("proxy").has_value()) {
    Glue::GetComponent<Glue::PhysicsManager>().ProcessData(entity);
  }
}

void SceneManager::RegMaterial(const Ogre::Material *material) {
  Ogre::LogManager::getSingleton().logMessage("[ScanNode] Material: " + material->getName());

  // UpgradeTransparentShadowCaster(material);

  // skip registering, keep it for future needs
  return;

  const auto *pass = material->getTechnique(0)->getPass(0);

  if (!pass->hasVertexProgram() || !pass->hasFragmentProgram()) {
    return;
  }

  const auto &vp = pass->getVertexProgramParameters();
  const auto &fp = pass->getFragmentProgramParameters();

  if (vp->getConstantDefinitions().map.count("uWorldViewProjPrev") > 0) {
    if (find(vpParams.begin(), vpParams.end(), vp) == vpParams.end()) {
      vpParams.push_back(vp);
    }

    if (find(fpParams.begin(), fpParams.end(), fp) == fpParams.end()) {
      fpParams.push_back(fp);
    }
  }
}

void SceneManager::UnregMaterial(const Ogre::Material *material) {
  const auto *pass = material->getTechnique(0)->getPass(0);
  const auto &vp = pass->getVertexProgramParameters();
  const auto &fp = pass->getFragmentProgramParameters();

  auto it = find(vpParams.begin(), vpParams.end(), vp), end = vpParams.end();
  if (it != end) {
    swap(it, --end);
    vpParams.pop_back();
  }

  it = find(fpParams.begin(), fpParams.end(), fp), end = fpParams.end();
  if (it != end) {
    swap(it, --end);
    fpParams.pop_back();
  }
}

void SceneManager::RegMaterial(const std::string &name) { RegMaterial(Ogre::MaterialManager::getSingleton().getByName(name).get()); }
void SceneManager::UnregMaterial(const std::string &name) { UnregMaterial(Ogre::MaterialManager::getSingleton().getByName(name).get()); }

void SceneManager::ScanNode(Ogre::SceneNode *node) {
  for (auto it : node->getAttachedObjects()) {
    if (auto camera = dynamic_cast<Ogre::Camera *>(it)) {
      Ogre::LogManager::getSingleton().logMessage("[ScanNode] Node: " + node->getName() + "  Camera: " + it->getName());
      RegCamera(camera);
      continue;
    }

    if (auto light = dynamic_cast<Ogre::Light *>(it)) {
      Ogre::LogManager::getSingleton().logMessage("[ScanNode] Node: " + node->getName() + "  Light: " + it->getName());
      RegLight(light);
      continue;
    }

    if (auto entity = dynamic_cast<Ogre::Entity *>(it)) {
      Ogre::LogManager::getSingleton().logMessage("[ScanNode] Node: " + node->getName() + "  Entity: " + it->getName());
      RegEntity(entity);
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


  if (_sleep) {
    return;
  }

  auto &vp = pass->getVertexProgramParameters();
  auto &fp = pass->getFragmentProgramParameters();
  Ogre::Matrix4 MVP;
  rend->getWorldTransforms(&MVP);
  MVP = ViewProj * MVP;

  Ogre::Any value = rend->getUserAny();
  rend->setUserAny(MVP);
  vp->setIgnoreMissingParams(true);

  // apply for entities, except grass
  if (auto *entity = dynamic_cast<Ogre::SubEntity *>(rend)) {
    if (!entity->getParent()->getName().rfind("GrassLDR", 0)) {
      vp->setNamedConstant("uWorldViewProjPrev", ViewProjPrev);
    } else if (value.has_value()) {
      vp->setNamedConstant("uWorldViewProjPrev", Ogre::any_cast<Ogre::Matrix4>(value));
    }
  }

  // skip BatchedGeometry
  else if (dynamic_cast<Forests::BatchedGeometry::SubBatch *>(rend)) {
    vp->setNamedConstant("uWorldViewProjPrev", ViewProjPrev);
  }

  // skip StaticGeometry
  else if (dynamic_cast<Ogre::StaticGeometry::GeometryBucket *>(rend)) {
    vp->setNamedConstant("uWorldViewProjPrev", ViewProjPrev);
  }

  // skip terrain
  else if (dynamic_cast<Ogre::TerrainQuadTreeNode *>(rend)) {
    vp->setNamedConstant("uWorldViewProjPrev", ViewProjPrev);
  }

  // default, keep it as it is
  else if (value.has_value()) {
    vp->setNamedConstant("uWorldViewProjPrev", ViewProjPrev);
  }

  vp->setIgnoreMissingParams(false);
}

}  // namespace Glue
