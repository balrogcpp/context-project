/*
 * =====================================================================================
 *
 *       Filename:  BtOgreGP.h
 *
 *    Description:  The part of BtOgre that handles information transfer from Ogre to
 *                  Bullet (like mesh data for making trimeshes).
 *
 *        Version:  1.0
 *        Created:  27/12/2008 03:29:56 AM
 *
 *         Author:  Nikhilesh (nikki)
 *
 * =====================================================================================
 */

#pragma once
#ifndef _BtOgrePG_H_
#define _BtOgrePG_H_


#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include <Ogre.h>

namespace Ogre {
  class TerrainGroup;
  class Terrain;
};


namespace BtOgre {
enum ColliderType
{
  CT_BOX,
  CT_SPHERE,
  CT_CYLINDER,
  CT_CAPSULE,
  CT_TRIMESH,
  CT_HULL,
  CT_COMPOUND
};

inline btQuaternion convert(const Ogre::Quaternion& q) { return btQuaternion(q.x, q.y, q.z, q.w); }
inline btVector3 convert(const Ogre::Vector3& v) { return btVector3(v.x, v.y, v.z); }

inline Ogre::Quaternion convert(const btQuaternion& q) { return Ogre::Quaternion(q.w(), q.x(), q.y(), q.z()); }
inline Ogre::Vector3 convert(const btVector3& v) { return Ogre::Vector3(v.x(), v.y(), v.z()); }

/** A MotionState is Bullet's way of informing you about updates to an object.
 * Pass this MotionState to a btRigidBody to have your SceneNode updated automaticaly.
 */
class RigidBodyState : public btMotionState
{
Ogre::Node* mNode;

public:
RigidBodyState(Ogre::Node* node) : mNode(node) {}

void getWorldTransform(btTransform& ret) const override
{
ret = btTransform(convert(mNode->getOrientation()), convert(mNode->getPosition()));
}

void setWorldTransform(const btTransform& in) override
{
btQuaternion rot = in.getRotation();
btVector3 pos = in.getOrigin();
mNode->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());
mNode->setPosition(pos.x(), pos.y(), pos.z());
}
};

/// height field data
struct HeightFieldData {
    /** the position for a center of the shape, i.e. where to place btRigidBody
     *  or a child of btCompoundShape */
    Ogre::Vector3 bodyPosition;
    /** a heightfield pointer to be freed when
     * btHeightfieldTerrainShape is freed */
    float *terrainHeights;
};

/// create sphere collider using ogre provided data
btSphereShape* createSphereCollider(const Ogre::MovableObject* mo);
/// create box collider using ogre provided data
btBoxShape* createBoxCollider(const Ogre::MovableObject* mo);
/// create capsule collider using ogre provided data
btCapsuleShape* createCapsuleCollider(const Ogre::MovableObject* mo);
/// create capsule collider using ogre provided data
btCylinderShape* createCylinderCollider(const Ogre::MovableObject* mo);
/// create triMesh collider
btBvhTriangleMeshShape* createTrimeshCollider(const Ogre::Entity* ent);
/// create convex hull collider
btConvexHullShape* createConvexHullCollider(const Ogre::Entity* ent);
/// create compound shape
btCompoundShape* createCompoundShape();
/// create height field collider
btHeightfieldTerrainShape* createHeightfieldTerrainShape(const Ogre::Terrain* terrain, struct HeightFieldData *data);

struct CollisionListener
    {
        virtual ~CollisionListener() {}
        /** Called when two objects collide
        * @param other the other object
        * @param manifoldPoint the collision point
         */
        virtual void contact(const Ogre::MovableObject* other, const btManifoldPoint& manifoldPoint) = 0;
    };

struct RayResultCallback
    {
        virtual ~RayResultCallback() {}
        virtual void addSingleResult(const Ogre::MovableObject* other, float distance) = 0;
    };

/// simplified wrapper with automatic memory management
class CollisionWorld
{
protected:
    std::unique_ptr<btCollisionConfiguration> mCollisionConfig;
    std::unique_ptr<btCollisionDispatcher> mDispatcher;
    std::unique_ptr<btConstraintSolver> mSolver;
    std::unique_ptr<btBroadphaseInterface> mBroadphase;
    btCollisionWorld* mBtWorld;
    btGhostPairCallback* mGhostPairCallback;
#if (OGRE_THREAD_SUPPORT > 0)
    std::unique_ptr<btITaskScheduler> mScheduler;
    std::unique_ptr<btConstraintSolver> mSolverPool;
#endif

public:
    CollisionWorld(btCollisionWorld* btWorld) : mBtWorld(btWorld) {}
    virtual ~CollisionWorld();

    btCollisionObject* addCollisionObject(Ogre::Entity* ent, ColliderType ct, int group = 1, int mask = -1);

    void rayTest(const Ogre::Ray& ray, RayResultCallback* callback, float maxDist = 1000);
    void attachCollisionObject(btCollisionObject *collisionObject, Ogre::Entity *ent, int group = 1, int mask = -1);
 };

/// helper class for kinematic body motion
class KinematicMotionSimple : public btActionInterface
{
    std::vector<btCollisionShape*> mCollisionShapes;
    std::vector<btTransform> mCollisionTransforms;
    btPairCachingGhostObject* mGhostObject;
    btVector3 mCurrentPosition;
    btQuaternion mCurrentOrientation;
    btManifoldArray mManifoldArray;
    btScalar mMaxPenetrationDepth;
    Ogre::Node* mNode;
    virtual bool needsCollision(const btCollisionObject* body0, const btCollisionObject* body1);
    void preStep(btCollisionWorld* collisionWorld);
    void playerStep(btCollisionWorld* collisionWorld, btScalar dt);
    void setupCollisionShapes(btCollisionObject* body);

public:
    KinematicMotionSimple(btPairCachingGhostObject* ghostObject, Ogre::Node* node);
    ~KinematicMotionSimple();
    bool recoverFromPenetration(btCollisionWorld* collisionWorld);
    virtual void updateAction(btCollisionWorld* collisionWorld, btScalar deltaTimeStep) override;
    virtual void debugDraw(btIDebugDraw* debugDrawer) override;
};
/// simplified wrapper with automatic memory management
class DynamicsWorld : public CollisionWorld
{
    std::unique_ptr<btConstraintSolver> mSolver;

public:
    explicit DynamicsWorld(const Ogre::Vector3& gravity);
    DynamicsWorld(btDynamicsWorld* btWorld) : CollisionWorld(btWorld) {}

    /** Add an Entity as a rigid body to the DynamicsWorld
    * @param mass the mass of the object
    * @param ent the entity to control
    * @param ct the collider type
    * @param listener a listener to call on collision with other objects
    * @param group the collision group
    * @param mask the collision mask
     */
    btRigidBody* addRigidBody(float mass, Ogre::Entity* ent, ColliderType ct, CollisionListener* listener = nullptr,
                              int group = 1, int mask = -1);
    btRigidBody* addKinematicRigidBody(Ogre::Entity* ent, ColliderType ct, int group = 1, int mask = -1);

    /** Add static body for Ogre terrain
     * @param terrainGroup the TerrainGroup of the terrain
     * @param x x coordinate of the terrain slot
     * @param y y coordinate of the terrain slot
     * @param group the collision group
     * @param mask the collision mask
     */
    btRigidBody* addTerrainRigidBody(Ogre::TerrainGroup* terrainGroup, long x, long y, int group = 1, int mask = -1);
    /** Add static body for Ogre terrain
     * @param terrain the terrain
     * @param group the collision group
     * @param mask the collision mask
     */
    btRigidBody* addTerrainRigidBody(Ogre::Terrain* terrain, int group = 1, int mask = -1);

    void attachRigidBody(btRigidBody *rigidBody, Ogre::Entity *ent, CollisionListener* listener = nullptr,
                              int group = 1, int mask = -1);
    btDynamicsWorld* getBtWorld() const { return static_cast<btDynamicsWorld*>(mBtWorld); }
};

class DebugDrawer : public btIDebugDraw
{
  Ogre::SceneNode* mNode;
  btCollisionWorld* mWorld;

  Ogre::ManualObject mLines;
  int mDebugMode;

public:
    DebugDrawer(Ogre::SceneNode* node, btCollisionWorld* world)
        : mNode(node), mWorld(world), mLines(""), mDebugMode(DBG_DrawWireframe)
    {
      mLines.setCastShadows(false);
      mNode->attachObject(&mLines);
      mWorld->setDebugDrawer(this);
    }

void update()
{
  mWorld->debugDrawWorld();
  if (!mLines.getSections().empty()) // begin was called
    mLines.end();
}

void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;

void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime,
                      const btVector3& color) override
{
drawLine(PointOnB, PointOnB + normalOnB * distance * 20, color);
}

void reportErrorWarning(const char* warningString) override { Ogre::LogManager::getSingleton().logWarning(warningString); }

void draw3dText(const btVector3& location, const char* textString) override {}

void setDebugMode(int mode) override
{
mDebugMode = mode;

if (mDebugMode == DBG_NoDebug)
clear();
}

void clear() { mLines.clear(); }

int getDebugMode() const override { return mDebugMode; }
};
}

#endif
