/// created by Andrey Vasiliev

#include "pch.h"
#include "SceneManager.h"
#include "ForestsManager.h"
#include "PhysicsManager.h"
#include "TerrainManager.h"
#include "VideoManager.h"

using namespace std;

namespace {
void CheckTransparentShadowCaster(const Ogre::Material *material) {
  const auto *pass = material->getTechnique(0)->getPass(0);

  if (!material->getTransparencyCastsShadows() || !pass->getNumTextureUnitStates() || !pass->getAlphaRejectValue() ||
      !pass->getTextureUnitState("Albedo")) {
    return;
  }

  auto casterMaterial = Ogre::MaterialManager::getSingleton().getByName("PSSM/shadow_caster_alpha");
  std::string newCasterName = "PSSM/shadow_caster_alpha/" + material->getName();
  auto newCaster = Ogre::MaterialManager::getSingleton().getByName(newCasterName);

  if (!newCaster) {
    newCaster = casterMaterial->clone(newCasterName);
    std::string albedoTexture = pass->getTextureUnitState("Albedo")->getTextureName();
    auto *newPass = newCaster->getTechnique(0)->getPass(0);

    auto *baseColor = newPass->getTextureUnitState("BaseColor");
    baseColor->setContentType(Ogre::TextureUnitState::CONTENT_NAMED);
    baseColor->setTextureName(albedoTexture);

    newPass->setCullingMode(pass->getCullingMode());
    newPass->setManualCullingMode(pass->getManualCullingMode());
    newPass->setAlphaRejectValue(pass->getAlphaRejectValue());
    material->getTechnique(0)->setShadowCasterMaterial(newCaster);
  }
}

void CheckTransparentShadowCaster(const std::string &material) {
  CheckTransparentShadowCaster(Ogre::MaterialManager::getSingleton().getByName(material).get());
}

bool HasNoTangentsAndCanGenerate(Ogre::VertexDeclaration *vertex_declaration) {
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

void EnsureHasTangents(const Ogre::MeshPtr &mesh) {
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
SceneManager::SceneManager() {}
SceneManager::~SceneManager() { Ogre::MaterialManager::getSingleton().removeListener(this); }

void SceneManager::OnSetUp() {
  ogreRoot = Ogre::Root::getSingletonPtr();
  ASSERTION(ogreRoot, "[SceneManager] ogreRoot is not initialised");
  ogreSceneManager = ogreRoot->getSceneManager("Default");
  ASSERTION(ogreSceneManager, "[SceneManager] ogreSceneManager is not initialised");
  ASSERTION(ogreSceneManager->hasCamera("Default"), "[SceneManager] ogreCamera is not initialised");
  ogreCamera = ogreSceneManager->getCamera("Default");

  ogreSceneManager->addRenderObjectListener(this);
  // Ogre::MaterialManager::getSingleton().addListener(this);
}

void SceneManager::OnClean() {
  ogreSceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
  ogreSceneManager->clearScene();
  ogreSceneManager->removeRenderObjectListener(this);
  InputSequencer::GetInstance().UnregMouseListener(sinbad.get());
  InputSequencer::GetInstance().UnregKeyboardListener(sinbad.get());
  sinbad.reset();
  fpParams.clear();
  fpParams.shrink_to_fit();
  vpParams.clear();
  vpParams.shrink_to_fit();
}

void SceneManager::OnUpdate(float time) {
  if (sinbad) sinbad->Update(time);
  static Ogre::Matrix4 MVP;
  static Ogre::Matrix4 MVPprev;
  MVPprev = MVP;
  MVP = ogreCamera->getProjectionMatrixWithRSDepth() * ogreCamera->getViewMatrix();
  for (auto &it : vpParams) it->setNamedConstant("uWorldViewProjPrev", MVPprev);
}

void SceneManager::LoadFromFile(const std::string filename) {
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
  auto objBindings = ogreSceneManager->getRootSceneNode()->getUserObjectBindings();
  if (objBindings.getUserAny("TerrainGroup").has_value()) {
    auto terrainGroup = Ogre::any_cast<Ogre::TerrainGroup *>(objBindings.getUserAny("TerrainGroup"));
    GetComponent<TerrainManager>().RegTerrainGroup(terrainGroup);
    GetComponent<TerrainManager>().ProcessTerrainCollider(terrainGroup);
  }

  // search for PagedGeometry
  auto &forests = GetComponent<ForestsManager>();
  const std::string base = "PagedGeometry";
  std::string key = base;
  int i = 1;  // counter
  Ogre::Any value = objBindings.getUserAny(key);

  while (value.has_value()) {
    forests.RegPagedGeometry(Ogre::any_cast<Forests::PagedGeometry *>(value));
    key = base + to_string(i++);
    value = objBindings.getUserAny(key);
  }

  // this will update al objects in scene
  // GetComponent<VideoManager>().RenderFrame();

  // scan second time, new objects added during first scan
  for (auto it : rootNode->getChildren()) {
    ScanNode(static_cast<Ogre::SceneNode *>(it));
  }
}

void SceneManager::RegCamera(Ogre::Camera *camera) {}

void SceneManager::RegLight(Ogre::Light *light) {}

void SceneManager::RegEntity(const std::string &name) { RegEntity(ogreSceneManager->getEntity(name)); }

void SceneManager::RegEntity(Ogre::Entity *entity) {
  EnsureHasTangents(entity->getMesh());

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
  CheckTransparentShadowCaster(material);

  const auto *pass = material->getTechnique(0)->getPass(0);

  if (pass->hasVertexProgram()) {
    const auto ptr = pass->getVertexProgramParameters();
    if (find(vpParams.begin(), vpParams.end(), ptr) == vpParams.end()) {
      vpParams.push_back(ptr);
    }
  }

  if (pass->hasFragmentProgram()) {
    const auto ptr = pass->getFragmentProgramParameters();
    if (find(fpParams.begin(), fpParams.end(), ptr) == fpParams.end()) {
      fpParams.push_back(ptr);
    }
  }
}

void SceneManager::RegMaterial(const std::string &name) { RegMaterial(Ogre::MaterialManager::getSingleton().getByName(name).get()); }

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
