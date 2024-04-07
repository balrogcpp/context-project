/// created by Andrey Vasiliev

#include "pch.h"
#include "SceneManager.h"
#include "SystemLocator.h"
#include <PagedGeometry/PagedGeometryAll.h>

using namespace std;
using namespace Ogre;

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
    ProcessNode(dynamic_cast<Ogre::SceneNode *>(it));
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
    ProcessNode(dynamic_cast<Ogre::SceneNode *>(it));
  }
}

void SceneManager::ProcessCamera(Ogre::Camera *camera) {
  Ogre::SceneNode *node = camera->getParentSceneNode();
  Ogre::Vector3 position = node->getPosition();
  node->translate(0.0, GetComponent<TerrainManager>().GetHeight(position.x, position.z), 0.0);
}

void SceneManager::ProcessLight(Ogre::Light *light) {
  Ogre::SceneNode *node = light->getParentSceneNode();
  Ogre::Vector3 position = node->getPosition();
  node->translate(0.0, GetComponent<TerrainManager>().GetHeight(position.x, position.z), 0.0);
}

void SceneManager::ProcessEntity(const std::string &name) { ProcessEntity(sceneManager->getEntity(name)); }

// snippet helps to parse vertex buffers
namespace {
#define INT10_MAX ((1 << 9) - 1)

struct int_10_10_10_2 {
  int32_t x : 10;
  int32_t y : 10;
  int32_t z : 10;
  int32_t w : 2;
};

template <int INCLUDE_W>
void pack_10_10_10_2(uint8 *pDst, uint8 *pSrc, int elemOffset) {
  float *pFloat = (float *)(pSrc + elemOffset);
  int_10_10_10_2 packed = {int(INT10_MAX * pFloat[0]), int(INT10_MAX * pFloat[1]), int(INT10_MAX * pFloat[2]), 1};
  if (INCLUDE_W) packed.w = int(pFloat[3]);
  memcpy(pDst + elemOffset, &packed, sizeof(int_10_10_10_2));
}

template <int INCLUDE_W>
void unpack_10_10_10_2(uint8 *pDst, uint8 *pSrc, int elemOffset) {
  int_10_10_10_2 *pPacked = (int_10_10_10_2 *)(pSrc + elemOffset);
  float *pFloat = (float *)(pDst + elemOffset);

  pFloat[0] = float(pPacked->x) / INT10_MAX;
  pFloat[1] = float(pPacked->y) / INT10_MAX;
  pFloat[2] = float(pPacked->z) / INT10_MAX;
  if (INCLUDE_W) pFloat[3] = pPacked->w;
}

Vector2 encode(Vector3 n) {
  Real p = sqrt(n.z * 8.0 + 8.0);
  return Vector2(Vector2(n.x, n.y) / p + 0.5);
}

Vector3 decode(Vector2 enc) {
  Vector2 fenc = enc * 4.0 - 2.0;
  Real f = fenc.dotProduct(fenc);
  Real g = sqrt(1.0 - f / 4.0);
  Vector3 n;
  n.x = fenc.x * g;
  n.y = fenc.y * g;
  n.z = 1.0 - f / 2.0;
  return n;
}

void EncodeNormals(Ogre::Entity *entity) {
  vector<VertexData *> vlist;
  vlist.push_back(entity->getVertexDataForBinding());
  for (auto &submesh : entity->getMesh()->getSubMeshes()) {
    vlist.push_back(submesh->vertexData);
  }

  for (auto *vdata : vlist) {
    if (!vdata) continue;

    size_t nOffset = 0, tOffset = 0;
    auto *vdecl = vdata->vertexDeclaration;
    auto *nElement = vdecl->findElementBySemantic(Ogre::VES_NORMAL);
    if (nElement) nOffset = nElement->getOffset() / sizeof(float);
    auto *tElement = vdecl->findElementBySemantic(Ogre::VES_TANGENT);
    if (tElement) tOffset = tElement->getOffset() / sizeof(float);

    if (!nElement || !tElement) continue;

    auto &buf = vdata->vertexBufferBinding->getBuffer(0);
    float *data = static_cast<float *>(buf->lock(Ogre::HardwareBuffer::HBL_DISCARD));

    for (int i = 0; i < buf->getNumVertices(); i++) {
      size_t offset = i * buf->getVertexSize() / sizeof(float);
      Vector3 n = Vector3(data[offset + nOffset], data[offset + nOffset + 1], data[offset + nOffset + 2]);
      Vector3 t = Vector3(data[offset + tOffset], data[offset + tOffset + 1], data[offset + tOffset + 2]);

      Vector2 nPacked = encode(n);
      Vector3 tb = abs(n.x) > abs(n.z) ? Vector3(-n.y, n.x, 0.0) : Vector3(0.0, -n.z, n.y);
      Real cosa = max(tb.dotProduct(t), Real(0.0001));

      data[offset + nOffset] = nPacked.x;
      data[offset + nOffset + 1] = nPacked.y;
      data[offset + nOffset + 2] = cosa;
    }

    buf->unlock();
  }
}
}  // namespace

void SceneManager::ProcessEntity(Ogre::Entity *entity) {
  if (entity->getName().rfind("GrassLDR", 0)) {
    Ogre::SceneNode *node = entity->getParentSceneNode();
    Ogre::Vector3 position = node->getPosition();
    // node->translate(0.0, GetComponent<TerrainManager>().GetHeight(position.x, position.z), 0.0);
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
    gge::GetComponent<gge::PhysicsManager>().ProcessData(entity);
  }
}

void SceneManager::ProcessNode(Ogre::SceneNode *node) {
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

Ogre::SceneManager *SceneManager::GetOgreScene() { return sceneManager; }

}  // namespace gge
