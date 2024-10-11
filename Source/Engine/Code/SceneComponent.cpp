/// created by Andrey Vasiliev

#include "pch.h"
#include "SceneComponent.h"
#include "Application.h"
#include <PagedGeometry/PagedGeometryAll.h>
#include <Terrain/OgreTerrainGroup.h>

using namespace std;
using namespace Ogre;

namespace gge {

SceneComponent::SceneComponent() {}
SceneComponent::~SceneComponent() {}

void SceneComponent::OnSetUp() {
  ogreRoot = Ogre::Root::getSingletonPtr();
  ASSERTION(ogreRoot, "[SceneManager] ogreRoot is not initialised");
  sceneManager = ogreRoot->getSceneManager("Default");
  ASSERTION(sceneManager, "[SceneManager] sceneManager is not initialised");
  ASSERTION(sceneManager->hasCamera("Camera"), "[SceneManager] camera is not initialised");
  camera = sceneManager->getCamera("Camera");
}

void SceneComponent::OnClean() {
  sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
  sceneManager->clearScene();
}

void SceneComponent::OnUpdate(float time) {}

static void ScanForests(const Ogre::UserObjectBindings &objBindings, const std::string &base) {
  //auto &forests = GetComponent<ForestsManager>();
  //unsigned long long generator = 0;
  //std::string key = base + to_string(generator++);
  //Ogre::Any value = objBindings.getUserAny(key);

  //while (value.has_value()) {
  //  forests.RegPagedGeometry(Ogre::any_cast<Forests::PagedGeometry *>(value));
  //  key = base + to_string(generator++);
  //  value = objBindings.getUserAny(key);
  //}
}

void SceneComponent::LoadFromFile(const std::string &filename) {
  auto *rootNode = sceneManager->getRootSceneNode();
  rootNode->loadChildren(filename);

  for (auto it : rootNode->getChildren()) {
    ProcessNode(dynamic_cast<Ogre::SceneNode *>(it));
  }

  //if (!sinbad && sceneManager->hasCamera("Camera")) {
  //  sinbad = make_unique<SinbadCharacterController>(sceneManager->getCamera("Camera"));
  //  InputSequencer::GetInstance().RegDeviceListener(sinbad.get());
  //}

  // search for TerrainGroup
  const auto &objBindings = sceneManager->getRootSceneNode()->getUserObjectBindings();
  if (objBindings.getUserAny("TerrainGroup").has_value()) {
    auto *terrainGlobalOptions = Ogre::TerrainGlobalOptions::getSingletonPtr();
    auto terrainGroup = Ogre::any_cast<shared_ptr<Ogre::TerrainGroup>>(objBindings.getUserAny("TerrainGroup"));

    if (sceneManager->hasLight("Sun")) {
      terrainGlobalOptions->setLightMapDirection(sceneManager->getLight("Sun")->getDerivedDirection());
    }

    for (auto it = terrainGroup->getTerrainIterator(); it.hasMoreElements();) {
      auto *terrain = it.getNext()->instance;
    }

    //terrainGroup->saveAllTerrains(true, true);
    //GetComponent<TerrainManager>().RegTerrainGroup(terrainGroup);
    //GetComponent<TerrainManager>().ProcessTerrainCollider(terrainGroup.get());
  }

  // search for GrassPage
  ScanForests(objBindings, "GrassPage");
  ScanForests(objBindings, "BatchPage");

  // scan second time, new objects added during first scan
  for (auto it : rootNode->getChildren()) {
    ProcessNode(dynamic_cast<Ogre::SceneNode *>(it));
  }
}

void SceneComponent::ProcessCamera(Ogre::Camera *camera) {
  Ogre::SceneNode *node = camera->getParentSceneNode();
  Ogre::Vector3 position = node->getPosition();
}

void SceneComponent::ProcessLight(Ogre::Light *light) {
  Ogre::SceneNode *node = light->getParentSceneNode();
  Ogre::Vector3 position = node->getPosition();
}

void SceneComponent::ProcessEntity(const std::string &name) { ProcessEntity(sceneManager->getEntity(name)); }

void SceneComponent::ProcessEntity(Ogre::Entity *entity) {
  if (entity->getName().rfind("GrassLDR", 0)) {
    Ogre::SceneNode *node = entity->getParentSceneNode();
    Ogre::Vector3 position = node->getPosition();
  }

  if (!entity->getMesh()->isReloadable()) return;
  unsigned short src;
  if (!entity->getMesh()->suggestTangentVectorBuildParams(src)) entity->getMesh()->buildTangentVectors(src);

  static unsigned long long generator = 0;
  auto mass = entity->getUserObjectBindings().getUserAny("mass");
  if (mass.has_value()) {
    if (Ogre::any_cast<Ogre::Real>(mass) > 0.0) {
      for (auto &it : entity->getSubEntities()) it->setMaterial(it->getMaterial()->clone(std::to_string(generator++)));
    }
  }

  if (entity->hasSkeleton()) {
    for (auto it : entity->getAttachedObjects()) {
      if (auto camera = dynamic_cast<Ogre::Camera *>(it)) {
        Ogre::LogManager::getSingleton().logMessage("[ProcessNode] AnimatedEntity: " + entity->getName() + "  Camera: " + it->getName());
        ProcessCamera(camera);
        continue;
      }

      if (auto light = dynamic_cast<Ogre::Light *>(it)) {
        Ogre::LogManager::getSingleton().logMessage("[ProcessNode] AnimatedEntity: " + entity->getName() + "  Light: " + it->getName());
        ProcessLight(light);
        continue;
      }

      if (auto entity = dynamic_cast<Ogre::Entity *>(it)) {
        Ogre::LogManager::getSingleton().logMessage("[ProcessNode] AnimatedEntity: " + entity->getName() + "  Entity: " + it->getName());
        ProcessEntity(entity);
        continue;
      }
    }
  }

  for (const auto &it : entity->getSubEntities()) {
    // do smth
  }
  auto objBindings = entity->getUserObjectBindings();
  if (objBindings.getUserAny("proxy").has_value()) {
    //gge::GetComponent<gge::PhysicsManager>().ProcessData(entity);
  }
}

void SceneComponent::ProcessNode(Ogre::SceneNode *node) {
  for (auto it : node->getAttachedObjects()) {
    if (auto camera = dynamic_cast<Ogre::Camera *>(it)) {
      Ogre::LogManager::getSingleton().logMessage("[ProcessNode] Node: " + node->getName() + "  Camera: " + it->getName());
      ProcessCamera(camera);
      continue;
    }

    if (auto light = dynamic_cast<Ogre::Light *>(it)) {
      Ogre::LogManager::getSingleton().logMessage("[ProcessNode] Node: " + node->getName() + "  Light: " + it->getName());
      ProcessLight(light);
      continue;
    }

    if (auto entity = dynamic_cast<Ogre::Entity *>(it)) {
      Ogre::LogManager::getSingleton().logMessage("[ProcessNode] Node: " + node->getName() + "  Entity: " + it->getName());
      ProcessEntity(entity);
      continue;
    }

    if (auto geometry = dynamic_cast<Forests::BatchedGeometry *>(it)) {
      Ogre::LogManager::getSingleton().logMessage("[ProcessNode] Node: " + node->getName() + "  Forests::BatchedGeometry: " + it->getName());
      auto it = geometry->getSubBatchIterator();
      while (it.hasMoreElements()) {
        it.getNext();
      }
      continue;
    }
  }

  // recurse
  for (auto it : node->getChildren()) {
    ProcessNode(dynamic_cast<Ogre::SceneNode *>(it));
  }
}

Ogre::SceneManager *SceneComponent::GetOgreScene() { return sceneManager; }

}  // namespace gge
