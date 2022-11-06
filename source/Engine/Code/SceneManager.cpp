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

namespace Glue {
SceneManager::SceneManager() {
  vpParams.reserve(200);
  fpParams.reserve(200);
}
SceneManager::~SceneManager() { Ogre::MaterialManager::getSingleton().removeListener(this); }

void SceneManager::OnSetUp() {
  ogreRoot = Ogre::Root::getSingletonPtr();
  ASSERTION(ogreRoot, "[SceneManager] ogreRoot is not initialised");
  ogreSceneManager = ogreRoot->getSceneManager("Default");
  ASSERTION(ogreSceneManager, "[SceneManager] ogreSceneManager is not initialised");
  ASSERTION(ogreSceneManager->hasCamera("Default"), "[SceneManager] ogreCamera is not initialised");
  ogreCamera = ogreSceneManager->getCamera("Default");

  ogreSceneManager->addRenderObjectListener(this);
  Ogre::MaterialManager::getSingleton().addListener(this);
}

void SceneManager::OnClean() {
  ogreSceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
  ogreSceneManager->clearScene();
  ogreSceneManager->removeRenderObjectListener(this);
  InputSequencer::GetInstance().UnregMouseListener(sinbad.get());
  InputSequencer::GetInstance().UnregKeyboardListener(sinbad.get());
  sinbad.reset();
  fpParams.clear();
  vpParams.clear();
}

void SceneManager::OnUpdate(float time) {
  if (sinbad) {
    sinbad->Update(time);
  }
  static Ogre::Matrix4 MVP;
  static Ogre::Matrix4 MVPprev;
  MVPprev = MVP;
  MVP = ogreCamera->getProjectionMatrixWithRSDepth() * ogreCamera->getViewMatrix();
  for (auto &it : vpParams) {
    it->setNamedConstant("uWorldViewProjPrev", MVPprev);
  }
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
    InputSequencer::GetInstance().RegMouseListener(sinbad.get());
    InputSequencer::GetInstance().RegKeyboardListener(sinbad.get());
  }

  // search for TerrainGroup
  const auto &objBindings = ogreSceneManager->getRootSceneNode()->getUserObjectBindings();
  if (objBindings.getUserAny("TerrainGroup").has_value()) {
    auto *terrainGroup = Ogre::any_cast<Ogre::TerrainGroup *>(objBindings.getUserAny("TerrainGroup"));
    for (auto it = terrainGroup->getTerrainIterator(); it.hasMoreElements();) {
      auto *terrain = it.getNext()->instance;
      RegMaterial(terrain->getMaterial().get());
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

void SceneManager::RegCamera(Ogre::Camera *camera) {}

void SceneManager::RegLight(Ogre::Light *light) {}

void SceneManager::RegEntity(const std::string &name) { RegEntity(ogreSceneManager->getEntity(name)); }

void SceneManager::RegEntity(Ogre::Entity *entity) {
  // EnsureHasTangents(entity->getMesh());

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

  for (const auto &it : entity->getSubEntities()) {
    RegMaterial(it->getMaterial().get());
  }

  auto objBindings = entity->getUserObjectBindings();
  if (objBindings.getUserAny("proxy").has_value()) {
    Glue::GetComponent<Glue::PhysicsManager>().ProcessData(entity);
  }
}

void SceneManager::RegMaterial(const Ogre::Material *material) {
  Ogre::LogManager::getSingleton().logMessage("[ScanNode] Material: " + material->getName());

  // UpgradeTransparentShadowCaster(material);

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
        RegMaterial(it.getNext()->getMaterial().get());
      }
      continue;
    }
  }

  // recurse
  for (auto it : node->getChildren()) {
    ScanNode(static_cast<Ogre::SceneNode *>(it));
  }
}

Ogre::Technique *SceneManager::handleSchemeNotFound(unsigned short schemeIndex, const std::string &schemeName, Ogre::Material *originalMaterial,
                                                    unsigned short lodIndex, const Ogre::Renderable *rend) {
  return nullptr;
}
bool SceneManager::afterIlluminationPassesCreated(Ogre::Technique *tech) { return false; }
bool SceneManager::beforeIlluminationPassesCleared(Ogre::Technique *tech) { return false; }

void SceneManager::notifyRenderSingleObject(Ogre::Renderable *rend, const Ogre::Pass *pass, const Ogre::AutoParamDataSource *source,
                                            const Ogre::LightList *pLightList, bool suppressRenderStateChanges) {}

}  // namespace Glue
