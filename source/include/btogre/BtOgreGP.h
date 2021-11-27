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
#include <OgreMatrix4.h>
#include <OgreMesh.h>
#include <OgrePrerequisites.h>
#include <OgreVector.h>
#include <btBulletDynamicsCommon.h>
#include <map>

namespace Ogre {
class SceneManager;
class SceneNode;
class IndexData;
class VertexData;
class Renderable;
class Entity;
}  // namespace Ogre

namespace BtOgre {

typedef std::map<unsigned char, Vector3Array *> BoneIndex;
typedef std::pair<unsigned short, Vector3Array *> BoneKeyIndex;

class VertexIndexToShape {
 public:
  VertexIndexToShape(const Ogre::Matrix4 &transform = Ogre::Matrix4::IDENTITY);
  ~VertexIndexToShape();

  float getRadius();
  Ogre::Vector3 getSize();

  btSphereShape *createSphere();
  btBoxShape *createBox();
  btBvhTriangleMeshShape *createTrimesh();
  btCylinderShape *createCylinder();
  btConvexHullShape *createConvex();
  btCapsuleShape *createCapsule();

  const Ogre::Vector3 *getVertices();
  unsigned int getVertexCount();
  const unsigned int *getIndices();
  unsigned int getIndexCount();

 protected:
  void addStaticVertexData(const Ogre::VertexData *vertex_data);

  void addAnimatedVertexData(const Ogre::VertexData *vertex_data, const Ogre::VertexData *blended_data,
                             const Ogre::Mesh::IndexMap *indexMap);

  void addIndexData(Ogre::IndexData *data, const unsigned int offset = 0);

 protected:
  Ogre::Vector3 *vertex_buffer_;
  unsigned int *index_buffer_;
  unsigned int vertex_count_;
  unsigned int index_count_;

  Ogre::Vector3 bounds_;
  float bound_radius_;

  BoneIndex *bone_index_;

  Ogre::Matrix4 transform_;

  Ogre::Vector3 scale_;
};

// For static (non-animated) meshes.
class StaticMeshToShapeConverter : public VertexIndexToShape {
 public:
  explicit StaticMeshToShapeConverter(Ogre::Renderable *rend, const Ogre::Matrix4 &transform = Ogre::Matrix4::IDENTITY);
  explicit StaticMeshToShapeConverter(Ogre::Entity *entity, Ogre::SceneNode *parent = nullptr,
                                      const Ogre::Matrix4 &transform = Ogre::Matrix4::IDENTITY);
  explicit StaticMeshToShapeConverter(Ogre::Entity *entity, const Ogre::MeshPtr &mesh,
                                      Ogre::SceneNode *parent = nullptr,
                                      const Ogre::Matrix4 &transform = Ogre::Matrix4::IDENTITY);
  StaticMeshToShapeConverter();

  virtual ~StaticMeshToShapeConverter();

  void addEntity(Ogre::Entity *entity, const Ogre::Matrix4 &transform = Ogre::Matrix4::IDENTITY,
                 Ogre::SceneNode *parent = nullptr);
  void addEntity(Ogre::Entity *entity, const Ogre::MeshPtr &mesh,
                 const Ogre::Matrix4 &transform = Ogre::Matrix4::IDENTITY, Ogre::SceneNode *parent = nullptr);

  void addMesh(const Ogre::MeshPtr &mesh, const Ogre::Matrix4 &transform = Ogre::Matrix4::IDENTITY);

 protected:
  Ogre::Entity *entity_;
  Ogre::SceneNode *node_;
};

// For animated meshes.
class AnimatedMeshToShapeConverter : public VertexIndexToShape {
 public:
  explicit AnimatedMeshToShapeConverter(Ogre::Entity *entity, const Ogre::Matrix4 &transform = Ogre::Matrix4::IDENTITY);
  AnimatedMeshToShapeConverter();
  virtual ~AnimatedMeshToShapeConverter();

  void addEntity(Ogre::Entity *entity, const Ogre::Matrix4 &transform = Ogre::Matrix4::IDENTITY);
  void addMesh(const Ogre::MeshPtr &mesh, const Ogre::Matrix4 &transform);

  btBoxShape *createAlignedBox(unsigned char bone, const Ogre::Vector3 &bonePosition,
                               const Ogre::Quaternion &boneOrientation);

  btBoxShape *createOrientedBox(unsigned char bone, const Ogre::Vector3 &bonePosition,
                                const Ogre::Quaternion &boneOrientation);

 protected:
  bool getBoneVertices(unsigned char bone, unsigned int &vertex_count, Ogre::Vector3 *&vertices,
                       const Ogre::Vector3 &bonePosition);

  bool getOrientedBox(unsigned char bone, const Ogre::Vector3 &bonePosition, const Ogre::Quaternion &boneOrientation,
                      Ogre::Vector3 &extents, Ogre::Vector3 *axis, Ogre::Vector3 &center);

  Ogre::Entity *mEntity;
  Ogre::SceneNode *mNode;

  Ogre::Vector3 *mTransformedVerticesTemp;
  size_t mTransformedVerticesTempSize;
};

}  // namespace BtOgre
