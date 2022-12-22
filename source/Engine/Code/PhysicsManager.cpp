/// created by Andrey Vasiliev

#include "pch.h"
#include "PhysicsManager.h"
#include <BtOgre/BtOgre.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <OgreTerrainGroup.h>

using namespace std;

namespace {
struct EntityCollisionListener {
  const Ogre::MovableObject *entity;
  BtOgre::CollisionListener *listener;
};

void OnTick(btDynamicsWorld *world, btScalar timeStep) {
  //  int numManifolds = world->getDispatcher()->getNumManifolds();
  //  auto manifolds = world->getDispatcher()->getInternalManifoldPointer();
  //  for (int i = 0; i < numManifolds; i++) {
  //    btPersistentManifold *manifold = manifolds[i];
  //
  //    for (int j = 0; j < manifold->getNumContacts(); j++) {
  //      const btManifoldPoint &mp = manifold->getContactPoint(i);
  //      auto body0 = static_cast<EntityCollisionListener *>(manifold->getBody0()->getUserPointer());
  //      auto body1 = static_cast<EntityCollisionListener *>(manifold->getBody1()->getUserPointer());
  //      if (body0->listener) {
  //        body0->listener->contact(body1->entity, mp);
  //      }
  //      if (body1->listener) {
  //        body1->listener->contact(body0->entity, mp);
  //      }
  //    }
  //  }
}
}  // namespace

namespace Glue {
PhysicsManager::PhysicsManager() : debugView(false), subSteps(4), fixedTimeStep(1.0 / 60.0) {}
PhysicsManager::~PhysicsManager() {}

void PhysicsManager::SetDebugView(bool debug) { this->debugView = debug; }

void PhysicsManager::OnSetUp() {
  auto *ogreRoot = Ogre::Root::getSingletonPtr();
  ASSERTION(ogreRoot, "[PhysicsManager] ogreRoot is not initialised");
  auto *ogreSceneManager = ogreRoot->getSceneManager("Default");
  ASSERTION(ogreSceneManager, "[PhysicsManager] ogreSceneManager is not initialised");
  auto *rootNode = ogreSceneManager->getRootSceneNode();

  dynamicWorld = make_unique<BtOgre::DynamicsWorld>(Ogre::Vector3(0.0, -9.8, 0.0));
  dynamicWorld->getBtWorld()->setInternalTickCallback(OnTick);
  debugDrawer = make_unique<BtOgre::DebugDrawer>(rootNode->createChildSceneNode(), dynamicWorld->getBtWorld());
}

void PhysicsManager::OnClean() {
  auto *btWorld = dynamicWorld->getBtWorld();
  btWorld->clearForces();

  // remove the rigidbodies from the dynamics world and delete them
  for (int i = btWorld->getNumCollisionObjects() - 1; i >= 0; i--) {
    btCollisionObject *obj = btWorld->getCollisionObjectArray()[i];
    btWorld->removeCollisionObject(obj);
    delete obj;
  }

  for (int i = btWorld->getNumConstraints() - 1; i >= 0; i--) {
    btTypedConstraint *constraint = btWorld->getConstraint(i);
    btWorld->removeConstraint(constraint);
    delete constraint;
  }
}

void PhysicsManager::OnUpdate(float time) {
  dynamicWorld->getBtWorld()->stepSimulation(time, subSteps, fixedTimeStep);
  if (debugView) {
    debugDrawer->update();
  }
}

void PhysicsManager::CreateTerrainHeightfieldShape(Ogre::TerrainGroup *terrainGroup) {
  for (auto it = terrainGroup->getTerrainIterator(); it.hasMoreElements();) {
    auto *terrain = it.getNext()->instance;
    CreateTerrainHeightfieldShape(terrain);
  }
}

void PhysicsManager::CreateTerrainHeightfieldShape(Ogre::Terrain *terrain) {
  auto *btWorld = dynamicWorld->getBtWorld();
  int size = terrain->getSize();
  float *data = terrain->getHeightData();
  float minHeight = terrain->getMinHeight();
  float maxHeight = terrain->getMaxHeight();
  Ogre::Vector3 position = terrain->getPosition();
  float scale = terrain->getWorldSize() / (static_cast<float>(terrain->getSize() - 1));

  // Convert height data in a format suitable for the physics engine
  auto *terrainHeights = new float[size * size];
  assert(terrainHeights != 0);

  for (int i = 0; i < size; i++) {
    memcpy(terrainHeights + size * i, data + size * (size - i - 1), sizeof(float) * size);
  }

  const btScalar heightScale = 1.0f;

  btVector3 localScaling(scale, heightScale, scale);

  auto *terrainShape = new btHeightfieldTerrainShape(size, size, terrainHeights, 1, minHeight, maxHeight, 1, PHY_FLOAT, false);

  terrainShape->setUseDiamondSubdivision(true);
  terrainShape->setLocalScaling(localScaling);

  auto *groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(size / 2, 0, size / 2)));
  btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, terrainShape, btVector3(0, 0, 0));

  // SetUp Rigid Body using 0 mass so it is static
  auto *rigidBody = new btRigidBody(groundRigidBodyCI);

  rigidBody->setFriction(0.4f);
  rigidBody->setHitFraction(0.8f);
  rigidBody->setRestitution(0.6f);
  rigidBody->getWorldTransform().setOrigin(btVector3(position.x, position.y + (maxHeight - minHeight) / 2 - 0.5, position.z));
  rigidBody->getWorldTransform().setRotation(BtOgre::convert(Ogre::Quaternion::IDENTITY));
  rigidBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);

  rigidBody->setUserIndex(0);
  btWorld->addRigidBody(rigidBody);
  btWorld->setForceUpdateAllAabbs(false);
}

inline static Ogre::Real ParseReal(const Ogre::Any &userData, Ogre::Real defaultValue = 0) {
  if (userData.has_value()) {
    return Ogre::any_cast<Ogre::Real>(userData);
  } else {
    return defaultValue;
  }
}

inline static int ParseInt(const Ogre::Any &userData, int defaultValue = 0) {
  if (userData.has_value()) {
    return Ogre::any_cast<int>(userData);
  } else {
    return defaultValue;
  }
}

inline static bool ParseBool(const Ogre::Any &userData, bool defaultValue = false) {
  if (userData.has_value()) {
    return Ogre::any_cast<bool>(userData);
  } else {
    return defaultValue;
  }
}

inline static Ogre::String ParseString(const Ogre::Any &userData, Ogre::String defaultValue = "") {
  if (userData.has_value()) {
    return Ogre::any_cast<Ogre::String>(userData);
  } else {
    return defaultValue;
  }
}

void PhysicsManager::ProcessData(Ogre::Entity *entity) {
  const Ogre::UserObjectBindings &userData = entity->getUserObjectBindings();
  constexpr BtOgre::ColliderType typeList[] = {BtOgre::ColliderType::CT_BOX,     BtOgre::ColliderType::CT_SPHERE,  BtOgre::ColliderType::CT_CYLINDER,
                                               BtOgre::ColliderType::CT_CAPSULE, BtOgre::ColliderType::CT_TRIMESH, BtOgre::ColliderType::CT_HULL};
  float mass = ParseReal(userData.getUserAny("mass"));
  int proxy = ParseInt(userData.getUserAny("proxy"));
  ASSERTION(proxy >= 0 && proxy < sizeof(typeList) / sizeof(BtOgre::ColliderType), "[PhysicsManager] proxy type is out of range");

  dynamicWorld->addRigidBody(mass, entity, typeList[proxy], nullptr);
}
}  // namespace Glue
