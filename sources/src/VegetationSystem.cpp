// MIT License
//
// Copyright (c) 2021 Andrew Vasiliev
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "VegetationSystem.h"

#include "ComponentLocator.h"
#include "MeshUtils.h"
#include "PbrShaderUtils.h"
#include "pcheader.h"
#include "pgeometry/PagedGeometryAll.h"

using namespace Forests;
using namespace std;

struct GrassVertex {
  float x, y, z;
  float nx, ny, nz;
  float u, v;
};

namespace xio {

//----------------------------------------------------------------------------------------------------------------------
static void CreateGrassMesh(float width, float height) {
  using namespace Ogre;

  MeshPtr mesh = MeshManager::getSingleton().createManual(
      "grass", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

  // create a submesh with the grass material
  SubMesh *sm = mesh->createSubMesh();
  sm->setMaterialName("GrassCustom");
  sm->useSharedVertices = false;
  sm->vertexData = OGRE_NEW VertexData();
  sm->vertexData->vertexStart = 0;
  sm->vertexData->vertexCount = 12;
  sm->indexData->indexCount = 18;

  Pbr::UpdatePbrParams("GrassCustom");
  Pbr::UpdatePbrShadowReceiver("GrassCustom");

  // specify a vertex format declaration for our mesh: 3 floats for position, 3
  // floats for normal, 2 floats for UV
  VertexDeclaration *decl = sm->vertexData->vertexDeclaration;
  decl->addElement(0, 0, VET_FLOAT3, VES_POSITION);
  decl->addElement(0, sizeof(float) * 3, VET_FLOAT3, VES_NORMAL);
  decl->addElement(0, sizeof(float) * 6, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0);

  // create a vertex buffer
  HardwareVertexBufferSharedPtr vb = HardwareBufferManager::getSingleton().createVertexBuffer(
      decl->getVertexSize(0), sm->vertexData->vertexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY);

  GrassVertex *verts =
      (GrassVertex *)vb->lock(HardwareBuffer::HBL_DISCARD);  // start filling in vertex data

  for (unsigned int i = 0; i < 3; i++)  // each grass mesh consists of 3 planes
  {
    // planes intersect along the Y axis with 60 degrees between them
    Real x = Math::Cos(Degree(i * 60)) * width / 2;
    Real z = Math::Sin(Degree(i * 60)) * width / 2;

    for (unsigned int j = 0; j < 4; j++)  // each plane has 4 vertices
    {
      GrassVertex &vert = verts[i * 4 + j];

      vert.x = j < 2 ? -x : x;
      vert.y = j % 2 ? 0 : height;
      vert.z = j < 2 ? -z : z;

      // all normals point straight up
      vert.nx = 0;
      vert.ny = 1;
      vert.nz = 0;

      vert.u = j < 2 ? 0 : 1;
      vert.v = j % 2;
    }
  }

  vb->unlock();  // commit vertex changes

  sm->vertexData->vertexBufferBinding->setBinding(0, vb);  // bind vertex buffer to our submesh

  // create an index buffer
  sm->indexData->indexBuffer = HardwareBufferManager::getSingleton().createIndexBuffer(
      HardwareIndexBuffer::IT_16BIT, sm->indexData->indexCount,
      HardwareBuffer::HBU_STATIC_WRITE_ONLY);

  // start filling in index data
  Ogre::uint16 *indices =
      (Ogre::uint16 *)sm->indexData->indexBuffer->lock(HardwareBuffer::HBL_DISCARD);

  for (unsigned int i = 0; i < 3; i++)  // each grass mesh consists of 3 planes
  {
    unsigned int off = i * 4;  // each plane consists of 2 triangles

    *indices++ = 0 + off;
    *indices++ = 3 + off;
    *indices++ = 1 + off;

    *indices++ = 0 + off;
    *indices++ = 2 + off;
    *indices++ = 3 + off;
  }

  sm->indexData->indexBuffer->unlock();  // commit index changes
}

//----------------------------------------------------------------------------------------------------------------------
//static void CreateGrassMesh2(float width, float height) {
//  Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().createManual("grass", "General");
//
//  // Init a submesh with the grass material
//  Ogre::SubMesh *sm = mesh->createSubMesh();
//  const string grassMaterial = "GrassCustom";
//  Ogre::MaterialPtr tmp = Ogre::MaterialManager::getSingleton().getByName(grassMaterial);
//
//  Pbr::UpdatePbrParams(tmp);
//  Pbr::UpdatePbrShadowReceiver(tmp);
//
//  sm->setMaterialName(grassMaterial);
//  sm->useSharedVertices = false;
//  sm->vertexData = new Ogre::VertexData();
//  sm->vertexData->vertexStart = 0;
//  sm->vertexData->vertexCount = 8;
//  sm->indexData->indexCount = 12;
//
//  // specify a vertex format declaration for our mesh: 3 floats for position, 3
//  // floats for normal, 2 floats for UV
//  Ogre::VertexDeclaration *decl = sm->vertexData->vertexDeclaration;
//  decl->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
//  decl->addElement(0, sizeof(float) * 3, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
//  decl->addElement(0, sizeof(float) * 6, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES, 0);
//
//  // Init a vertex buffer
//  Ogre::HardwareVertexBufferSharedPtr vb =
//      Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
//          decl->getVertexSize(0), sm->vertexData->vertexCount,
//          Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
//
//  auto *verts =
//      (GrassVertex *)vb->lock(Ogre::HardwareBuffer::HBL_DISCARD);  // start filling in vertex data
//
//  for (int i = 0; i < 2; i++)  // each grass mesh consists of 3 planes
//  {
//    // planes intersect along the Y axis with 60 degrees between them
//    float x = Ogre::Math::Cos(Ogre::Degree(i * 60)) * width / 2;
//    float z = Ogre::Math::Sin(Ogre::Degree(i * 60)) * width / 2;
//
//    for (int j = 0; j < 4; j++)  // each plane has 4 vertices
//    {
//      GrassVertex &vert = verts[i * 4 + j];
//
//      vert.x = j < 2 ? -x : x;
//      vert.y = j % 2 ? 0 : height;
//      vert.z = j < 2 ? -z : z;
//
//      // all normals point straight up
//      vert.nx = 0;
//      vert.ny = 1;
//      vert.nz = 0;
//
//      vert.u = j < 2 ? 0 : 1;
//      vert.v = j % 2;
//    }
//  }
//
//  vb->unlock();  // commit vertex changes
//
//  sm->vertexData->vertexBufferBinding->setBinding(0, vb);  // bind vertex buffer to our submesh
//
//  // Init an index buffer
//  sm->indexData->indexBuffer = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
//      Ogre::HardwareIndexBuffer::IT_16BIT, sm->indexData->indexCount,
//      Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
//
//  // start filling in index data
//  Ogre::uint16 *indices =
//      (Ogre::uint16 *)sm->indexData->indexBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
//
//  for (unsigned int i = 0; i < 2; i++)  // each grass mesh consists of 3 planes
//  {
//    unsigned int off = i * 4;  // each plane consists of 2 triangles
//
//    *indices++ = 0 + off;
//    *indices++ = 3 + off;
//    *indices++ = 1 + off;
//
//    *indices++ = 0 + off;
//    *indices++ = 2 + off;
//    *indices++ = 3 + off;
//  }
//
//  sm->indexData->indexBuffer->unlock();  // commit index changes
//}

//----------------------------------------------------------------------------------------------------------------------
VegetationSystem::VegetationSystem() {}

//----------------------------------------------------------------------------------------------------------------------
VegetationSystem::~VegetationSystem() {
  auto &mesh_manager = Ogre::MeshManager::getSingleton();
  if (mesh_manager.getByName("grass", Ogre::RGN_AUTODETECT))
    mesh_manager.remove("grass", Ogre::RGN_AUTODETECT);
}

//----------------------------------------------------------------------------------------------------------------------
void VegetationSystem::Update(float time) {
  for (auto &it : pgeometry_) it->update();

  for (auto &it : gpages_) it->update();
}

//----------------------------------------------------------------------------------------------------------------------
void VegetationSystem::GenerateGrassStatic() {
  // create our grass mesh, and Init a grass entity from it
  CreateGrassMesh(1.0, 1.0);
  auto *scene = Ogre::Root::getSingleton().getSceneManager("Default");
  Ogre::Entity *grass = scene->createEntity("Grass", "grass");
  // Init a static geometry field, which we will populate with grass
  auto *field = scene->createStaticGeometry("FarnField");
  field->setRegionDimensions(Ogre::Vector3(50.0));

  const float bounds = 500.0f;
  // add grass uniformly throughout the field, with some random variations
  for (int i = 0; i < 1000000; i++) {
    Ogre::Vector3 pos(Ogre::Math::RangeRandom(-bounds, bounds), 0,
                      Ogre::Math::RangeRandom(-bounds, bounds));
    pos.y += GetLoader().GetHeigh(pos.x, pos.z);

    Ogre::Quaternion ori(Ogre::Degree(Ogre::Math::RangeRandom(0, 359)), Ogre::Vector3::UNIT_Y);
    Ogre::Vector3 scale(1, Ogre::Math::RangeRandom(0.85, 1.15), 1);
    field->addEntity(grass, pos, ori, scale);
  }

//  field->setVisibilityFlags(SUBMERGED_MASK);
  //  field->setRenderQueueGroup(Ogre::RENDER_QUEUE_6);
  field->build();
  field->setCastShadows(false);

  sgeometry_.push_back(field);
}

//----------------------------------------------------------------------------------------------------------------------
void VegetationSystem::GenerateRocksStatic() {
  // create our grass mesh, and Init a grass entity from it
  auto *scene = Ogre::Root::getSingleton().getSceneManager("Default");
  Ogre::Entity *rock = scene->createEntity("Rock", "rock.mesh");
  UpdateMeshMaterial("rock.mesh");

  // Init a static geometry field, which we will populate with grass
  auto *rocks = scene->createStaticGeometry("Rocks");
  auto *root_node = Ogre::Root::getSingleton().getSceneManager("Default")->getRootSceneNode();

  const float bounds = 100.0f;
  // add grass uniformly throughout the field, with some random variations
  for (int i = 0; i < 250; i++) {
    Ogre::Vector3 pos(Ogre::Math::RangeRandom(-bounds, bounds), 0,
                      Ogre::Math::RangeRandom(-bounds, bounds));
    pos.y += GetLoader().GetHeigh(pos.x, pos.z) - 0.1;
    Ogre::Quaternion ori(Ogre::Degree(Ogre::Math::RangeRandom(0, 359)), Ogre::Vector3::UNIT_Y);
    Ogre::Vector3 scale(2.0, 2.0 * Ogre::Math::RangeRandom(0.85, 1.15), 2.0);

    auto *node = root_node->createChildSceneNode(pos, ori);
    node->scale(scale);
    GetPhysics().ProcessData(rock, node);
    scene->destroySceneNode(node);

    rocks->addEntity(rock, pos, ori, scale);
  }

  rocks->setRegionDimensions(Ogre::Vector3(25.0));

  //  rocks->setVisibilityFlags(SUBMERGED_MASK);
  //  rocks->setRenderQueueGroup(Ogre::RENDER_QUEUE_6);
  rocks->build();
  rocks->setCastShadows(true);

  sgeometry_.push_back(rocks);
}

//----------------------------------------------------------------------------------------------------------------------
void VegetationSystem::GenerateGrassPaged() {
  auto *grass = new PagedGeometry(
      Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default"), 5);
  grass->addDetailLevel<GrassPage>(50, 10);  // Draw grass up to 100
  auto *grassLoader = new GrassLoader(grass);
  grass->setPageLoader(grassLoader);
  grassLoader->setRenderQueueGroup(Ogre::RENDER_QUEUE_MAIN);

  if (heigh_func_)
    grassLoader->setHeightFunction(
        [](float x, float z, void *) { return Ogre::Real(heigh_func_(x, z)); });

  pgeometry_.push_back(unique_ptr<PagedGeometry>(grass));
  ploaders_.push_back(unique_ptr<PageLoader>(grassLoader));

  GrassLayer *layer = grassLoader->addLayer("GrassCustom");
  layer->setFadeTechnique(FADETECH_ALPHA);
  layer->setRenderTechnique(GRASSTECH_CROSSQUADS);
  layer->setMaximumSize(2.0f, 2.0f);
  layer->setAnimationEnabled(true);
  layer->setSwayDistribution(10.0f);
  layer->setSwayLength(1.0f);
  layer->setSwaySpeed(0.5f);
  layer->setDensity(1.0f);
  layer->setMapBounds(TBounds(-250, -250, 250, 250));
  //  layer->setDensityMap("grass_density.png");

  Update(0);

  Pbr::UpdatePbrParams("GrassCustom");
  Pbr::UpdatePbrShadowReceiver("GrassCustom");
}

//----------------------------------------------------------------------------------------------------------------------
void VegetationSystem::GenerateTreesPaged() {
  const float bound = 50;

  auto *scene = Ogre::Root::getSingleton().getSceneManager("Default");
  float x = 0, y = 0, z = 0, yaw, scale = 1.0;
  auto *trees = new PagedGeometry(scene->getCamera("Default"), 50);
  trees->addDetailLevel<Forests::WindBatchPage>(100, 20);
  trees->addDetailLevel<Forests::BatchPage>(200, 60);
  auto *treeLoader = new TreeLoader3D(trees, TBounds(-200, -200, 200, 200));
  //  if (heigh_func_)
  //    treeLoader->setHeightFunction([](float x, float z, void *) { return
  //    Ogre::Real(heigh_func_(x, z) - 0.1); });

  pgeometry_.push_back(unique_ptr<PagedGeometry>(trees));
  ploaders_.push_back(unique_ptr<PageLoader>(treeLoader));

  trees->setPageLoader(treeLoader);
  Ogre::Entity *fir1EntPtr = scene->createEntity("fir1", "fir05_30.mesh");

  UpdateMeshMaterial("fir05_30.mesh");

  auto *root_node = Ogre::Root::getSingleton().getSceneManager("Default")->getRootSceneNode();

  for (int i = 0; i < 50; i++) {
    yaw = Ogre::Math::RangeRandom(0, 360);
    if (Ogre::Math::RangeRandom(0, 1) <= 0.8f) {
      x = Ogre::Math::RangeRandom(-bound, bound);
      z = Ogre::Math::RangeRandom(-bound, bound);
      if (x < -bound)
        x = -bound;
      else if (x > bound)
        x = bound;
      if (z < -bound)
        z = -bound;
      else if (z > bound)
        z = bound;
    } else {
      x = Ogre::Math::RangeRandom(-bound, bound);
      z = Ogre::Math::RangeRandom(-bound, bound);
    }

    y = heigh_func_(x, z);
    scale = Ogre::Math::RangeRandom(0.9f, 1.1f);
    scale *= 0.2;
    Ogre::Quaternion quat;
    quat.FromAngleAxis(Ogre::Degree(yaw), Ogre::Vector3::UNIT_Y);

    auto *node = root_node->createChildSceneNode(Ogre::Vector3(x, y, z), quat);
    node->scale(Ogre::Vector3(scale));
    GetPhysics().ProcessData(fir1EntPtr, node, "capsule");
    scene->destroySceneNode(node);

    treeLoader->addTree(fir1EntPtr, Ogre::Vector3(x, y, z), Ogre::Degree(yaw), scale);
  }

  Update(0);

  Pbr::UpdatePbrParams("3D-Diggers/fir01_Batched");
  Pbr::UpdatePbrParams("3D-Diggers/fir02_Batched");
  //  Pbr::UpdatePbrShadowReceiver("3D-Diggers/fir01_Batched");
  //  Pbr::UpdatePbrShadowReceiver("3D-Diggers/fir02_Batched");
}

//----------------------------------------------------------------------------------------------------------------------
void VegetationSystem::ProcessForest() {
  GenerateGrassPaged();
  GenerateTreesPaged();
  //  GenerateRocksStatic();
}

}  // namespace xio
