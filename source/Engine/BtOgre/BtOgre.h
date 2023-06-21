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

#include "btBulletDynamicsCommon.h"
#include "Ogre.h"

namespace BtOgre {
enum ColliderType
{
  CT_BOX,
  CT_SPHERE,
  CT_CYLINDER,
  CT_CAPSULE,
  CT_TRIMESH,
  CT_HULL
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

/// create sphere collider using ogre provided data
btSphereShape* createSphereCollider(const Ogre::MovableObject* mo);
/// create box collider using ogre provided data
btBoxShape* createBoxCollider(const Ogre::MovableObject* mo);
/// create capsule collider using ogre provided data
btCapsuleShape* createCapsuleCollider(const Ogre::MovableObject* mo);
/// create capsule collider using ogre provided data
btCylinderShape* createCylinderCollider(const Ogre::MovableObject* mo);

struct CollisionListener
    {
        virtual ~CollisionListener() {}
        virtual void contact(const Ogre::MovableObject* other, const btManifoldPoint& manifoldPoint) = 0;
    };

struct RayResultCallback
    {
        virtual ~RayResultCallback() {}
        virtual void addSingleResult(const Ogre::MovableObject* other, float distance) = 0;
    };

/// simplified wrapper with automatic memory management
class DynamicsWorld
    {
        std::unique_ptr<btCollisionConfiguration> mCollisionConfig;
    std::unique_ptr<btCollisionDispatcher> mDispatcher;
    std::unique_ptr<btConstraintSolver> mSolver;
    std::unique_ptr<btBroadphaseInterface> mBroadphase;
#if (OGRE_THREAD_SUPPORT > 0)
    std::unique_ptr<btITaskScheduler> mScheduler;
    std::unique_ptr<btConstraintSolver> mSolverPool;
#endif
    btDynamicsWorld* mBtWorld;

    public:
    explicit DynamicsWorld(const Ogre::Vector3& gravity);
    ~DynamicsWorld();
    DynamicsWorld(btDynamicsWorld* btWorld) : mBtWorld(btWorld) {}

    btRigidBody* addRigidBody(float mass, Ogre::Entity* ent, ColliderType ct, CollisionListener* listener = nullptr,
    int group = 1, int mask = -1);

    btDynamicsWorld* getBtWorld() const { return mBtWorld; }

    void rayTest(const Ogre::Ray& ray, RayResultCallback* callback, float maxDist = 1000);
    };

class DebugDrawer : public btIDebugDraw
{
Ogre::SceneNode* mNode;
btDynamicsWorld* mWorld;

Ogre::ManualObject mLines;
int mDebugMode;

public:
DebugDrawer(Ogre::SceneNode* node, btDynamicsWorld* world)
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
