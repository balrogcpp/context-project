/// created by Andrey VasilievBullet

#include "pch.h"
#include "PhysicsManager.h"
#include "BtOgre/BtOgre.h"
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <OgreTerrainGroup.h>

using namespace std;

namespace {
void onTick(btDynamicsWorld *world, btScalar timeStep) {
  //  int numManifolds = world->getDispatcher()->getNumManifolds();
  //  auto manifolds = world->getDispatcher()->getInternalManifoldPointer();
  //  for (int i = 0; i < numManifolds; i++) {
  //    btPersistentManifold *manifold = manifolds[i];
  //
  //    for (int j = 0; j < manifold->getNumContacts(); j++) {
  //      const btManifoldPoint &mp = manifold->getContactPoint(i);
  //      auto body0 = static_cast<EntityCollisionListener *>(manifold->getBody0()->getUserPointer());
  //      auto body1 = static_cast<EntityCollisionListener *>(manifold->getBody1()->getUserPointer());
  //      if (body0->listener) body0->listener->contact(body1->entity, mp);
  //      if (body1->listener) body1->listener->contact(body0->entity, mp);
  //    }
  //  }
}
}  // namespace

namespace Glue {
PhysicsManager::PhysicsManager() : sleep(false), subSteps(4) {}
PhysicsManager::~PhysicsManager() {}

void PhysicsManager::OnSetUp() {
  dynamicWorld = make_unique<BtOgre::DynamicsWorld>(Ogre::Vector3(0.0, -9.8, 0.0));
  dynamicWorld->getBtWorld()->setInternalTickCallback(onTick);
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
  if (!sleep) {
    dynamicWorld->getBtWorld()->stepSimulation(time, subSteps, time);
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

void PhysicsManager::ProcessData(Ogre::Entity *entity, const Ogre::UserObjectBindings &userData) {
  auto *btWorld = dynamicWorld->getBtWorld();
  string proxy = Ogre::any_cast<string>(userData.getUserAny("proxy"));
  float mass = Ogre::any_cast<float>(userData.getUserAny("mass"));
  //  string physics_type = Ogre::any_cast<string>(userData.getUserAny("physics_type"));
  //  float mass_radius = Ogre::any_cast<float>(userData.getUserAny("mass_radius"));
  //  float inertia_tensor = Ogre::any_cast<float>(userData.getUserAny("inertia_tensor"));
  //  float velocity_min = Ogre::any_cast<float>(userData.getUserAny("velocity_min"));
  //  float velocity_max = Ogre::any_cast<float>(userData.getUserAny("velocity_max"));
  //  bool lock_trans_x = Ogre::any_cast<bool>(userData.getUserAny("lock_trans_x"));
  //  bool lock_trans_y = Ogre::any_cast<bool>(userData.getUserAny("lock_trans_y"));
  //  bool lock_trans_z = Ogre::any_cast<bool>(userData.getUserAny("lock_trans_z"));
  //  bool lock_rot_x = Ogre::any_cast<bool>(userData.getUserAny("lock_rot_x"));
  //  bool lock_rot_y = Ogre::any_cast<bool>(userData.getUserAny("lock_rot_y"));
  //  bool lock_rot_z = Ogre::any_cast<bool>(userData.getUserAny("lock_rot_z"));
  //  bool anisotropic_friction = Ogre::any_cast<bool>(userData.getUserAny("anisotropic_friction"));
  //  float friction_x = Ogre::any_cast<float>(userData.getUserAny("friction_x"));
  //  float friction_y = Ogre::any_cast<float>(userData.getUserAny("friction_y"));
  //  float friction_z = Ogre::any_cast<float>(userData.getUserAny("friction_z"));
  //  float damping_trans = Ogre::any_cast<float>(userData.getUserAny("damping_trans"));
  //  float damping_rot = Ogre::any_cast<float>(userData.getUserAny("damping_rot"));
  //  bool isStatic = (physics_type != "dynamic");

  btRigidBody *rigidBody = nullptr;
  btVector3 inertia = btVector3(0, 0, 0);

  Ogre::SceneNode *parent = entity->getParentSceneNode();
  btConvexInternalShape *entShape = nullptr;

  if (proxy == "box")
    entShape = BtOgre::createBoxCollider(entity);
  else if (proxy == "capsule")
    entShape = BtOgre::createCapsuleCollider(entity);
  else if (proxy == "sphere")
    entShape = BtOgre::createSphereCollider(entity);
  else if (proxy == "cylinder")
    entShape = BtOgre::createCylinderCollider(entity);
  else
    entShape = BtOgre::createBoxCollider(entity);

  if (mass > 0.0) {
    entShape->calculateLocalInertia(mass, inertia);
  }

  auto *bodyState = new BtOgre::RigidBodyState(parent);

  rigidBody = new btRigidBody(mass, bodyState, entShape, inertia);
  if (mass <= 0.0) {
    rigidBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
  }

  btWorld->addRigidBody(rigidBody);
}
}  // namespace Glue
