/// created by Andrey Vasiliev

#include "pch.h"
#include "SceneManager.h"
#include "SystemLocator.h"
#include <PagedGeometry/PagedGeometryAll.h>

using namespace std;

namespace gge {
SceneManager::SceneManager() {}
SceneManager::~SceneManager() {}

void SceneManager::OnSetUp() {
  ogreRoot = Ogre::Root::getSingletonPtr();
  ASSERTION(ogreRoot, "[SceneManager] ogreRoot is not initialised");
  sceneManager = ogreRoot->getSceneManager("Default");
  ASSERTION(sceneManager, "[SceneManager] sceneManager is not initialised");
  ASSERTION(sceneManager->hasCamera("Camera"), "[SceneManager] camera is not initialised");
  camera = sceneManager->getCamera("Camera");
}

void SceneManager::OnClean() {
  sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
  sceneManager->clearScene();
  InputSequencer::GetInstance().UnregDeviceListener(sinbad.get());
  sinbad.reset();
}

void SceneManager::OnUpdate(float time) {
  if (sinbad) sinbad->Update(time);
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
  auto *rootNode = sceneManager->getRootSceneNode();
  rootNode->loadChildren(filename);

  for (auto it : rootNode->getChildren()) {
    ScanNode(dynamic_cast<Ogre::SceneNode *>(it));
  }

  if (!sinbad && sceneManager->hasCamera("Camera")) {
    sinbad = make_unique<SinbadCharacterController>(sceneManager->getCamera("Camera"));
    InputSequencer::GetInstance().RegDeviceListener(sinbad.get());
  }

  // search for TerrainGroup
  const auto &objBindings = sceneManager->getRootSceneNode()->getUserObjectBindings();
  if (objBindings.getUserAny("TerrainGroup").has_value()) {
    auto *terrainGlobalOptions = Ogre::TerrainGlobalOptions::getSingletonPtr();
    auto *terrainGroup = Ogre::any_cast<Ogre::TerrainGroup *>(objBindings.getUserAny("TerrainGroup"));

    if (sceneManager->hasLight("Sun")) {
      terrainGlobalOptions->setLightMapDirection(sceneManager->getLight("Sun")->getDerivedDirection());
    }

    //    for (auto it = terrainGroup->getTerrainIterator(); it.hasMoreElements();) {
    //      auto *terrain = it.getNext()->instance;
    //    }

    GetComponent<TerrainManager>().RegTerrainGroup(terrainGroup);
    GetComponent<TerrainManager>().ProcessTerrainCollider(terrainGroup);
  }

  // search for GrassPage
  ScanForests(objBindings, "GrassPage");
  ScanForests(objBindings, "BatchPage");

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

void SceneManager::ScanEntity(const std::string &name) { ScanEntity(sceneManager->getEntity(name)); }

void SceneManager::ScanEntity(Ogre::Entity *entity) {
  if (entity->getName().rfind("GrassLDR", 0)) {
    Ogre::SceneNode *node = entity->getParentSceneNode();
    Ogre::Vector3 position = node->getPosition();
    //node->translate(0.0, GetComponent<TerrainManager>().GetHeight(position.x, position.z), 0.0);
  }

  if (!entity->getMesh()->isReloadable()) return;

  unsigned short src;
  if (!entity->getMesh()->suggestTangentVectorBuildParams(src)) entity->getMesh()->buildTangentVectors(src);

  static unsigned long long generator = 0;
  auto mass = entity->getUserObjectBindings().getUserAny("mass");
  if (mass.has_value() && entity->getMesh()->isReloadable()) {
    if (Ogre::any_cast<Ogre::Real>(mass) > 0.0) {
      for (auto &it : entity->getSubEntities()) it->setMaterial(it->getMaterial()->clone(std::to_string(generator++)));
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
    ScanNode(dynamic_cast<Ogre::SceneNode *>(it));
  }
}

}  // namespace gge
