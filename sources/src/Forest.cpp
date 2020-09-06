//Cpp file for dummy context2_deps target
/*
MIT License

Copyright (c) 2020 Andrey Vasiliev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "pcheader.h"

#include "Forest.h"
#include "PbrUtils.h"
using namespace xio;

namespace xio {
#pragma pack(push, 1)
struct GrassVertex {
  float x, y, z;
  float nx, ny, nz;
  float u, v;
};
#pragma pack(pop)
//----------------------------------------------------------------------------------------------------------------------
void Forest::CreateGrassMesh(float heigh) {
  if (Ogre::MeshManager::getSingleton().getByName("grass", Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME))
    return;

  Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().createManual("grass", "General");

  // Create a submesh with the grass material
  Ogre::SubMesh *sm = mesh->createSubMesh();
  const std::string grassMaterial = "GrassCustom";
  Ogre::MaterialPtr tmp = Ogre::MaterialManager::getSingleton().getByName(grassMaterial);

  UpdatePbrParams(tmp);
  UpdatePbrShadowReceiver(tmp);

  sm->setMaterialName(grassMaterial);
  sm->useSharedVertices = false;
  sm->vertexData = new Ogre::VertexData();
  sm->vertexData->vertexStart = 0;
  sm->vertexData->vertexCount = 8;
  sm->indexData->indexCount = 12;

  // specify a vertex format declaration for our mesh: 3 floats for position, 3 floats for normal, 2 floats for UV
  Ogre::VertexDeclaration *decl = sm->vertexData->vertexDeclaration;
  decl->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
  decl->addElement(0, sizeof(float) * 3, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
  decl->addElement(0, sizeof(float) * 6, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES, 0);

  // Create a vertex buffer
  Ogre::HardwareVertexBufferSharedPtr vb = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer
      (decl->getVertexSize(0), sm->vertexData->vertexCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

  auto *verts = (GrassVertex *) vb->lock(Ogre::HardwareBuffer::HBL_DISCARD);  // start filling in vertex data

  for (int i = 0; i < 2; i++)  // each grass mesh consists of 3 planes
  {
    // planes intersect along the Y axis with 60 degrees between them
    float x = Ogre::Math::Cos(Ogre::Degree(i * 60)) * heigh/2.0;
    float z = Ogre::Math::Sin(Ogre::Degree(i * 60)) * heigh/2.0;

    for (int j = 0; j < 4; j++)  // each plane has 4 vertices
    {
      GrassVertex &vert = verts[i * 4 + j];

      vert.x = j < 2 ? -x : x;
      vert.y = j % 2 ? 0 : heigh;
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

  // Create an index buffer
  sm->indexData->indexBuffer = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer
      (Ogre::HardwareIndexBuffer::IT_16BIT, sm->indexData->indexCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

  // start filling in index data
  Ogre::uint16 *indices = (Ogre::uint16 *) sm->indexData->indexBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);

  for (unsigned int i = 0; i < 2; i++)  // each grass mesh consists of 3 planes
  {
    unsigned int off = i * 4;  // each plane consists of 2 triangles

    *indices++ = 0 + off;
    *indices++ = 3 + off;
    *indices++ = 1 + off;

    *indices++ = 0 + off;
    *indices++ = 2 + off;
    *indices++ = 3 + off;
  }

  sm->indexData->indexBuffer->unlock(); // commit index changes
}
//----------------------------------------------------------------------------------------------------------------------
Forest::Forest() = default;

Forest::~Forest() {
  if (Ogre::MeshManager::getSingleton().getByName("grass", Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME))
    Ogre::MeshManager::getSingleton().remove("grass", Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
}
//----------------------------------------------------------------------------------------------------------------------
void Forest::GenerateGrass() {
  Ogre::SceneManager *scene = Ogre::Root::getSingleton().getSceneManager("Default");
  // create our grass mesh, and Create a grass entity from it
  CreateGrassMesh(0.5);

  static const Ogre::uint32 SUBMERGED_MASK = 0x0F0;
  static const Ogre::uint32 SURFACE_MASK = 0x00F;
  static const Ogre::uint32 WATER_MASK = 0xF00;

//  Ogre::Entity *farn = scene->createEntity("Farn", "farn1.mesh");
//  // Create a static geometry field, which we will populate with grass
//  mField = scene->createStaticGeometry("FarnField");
//  mField->setRegionDimensions(Ogre::Vector3(20));
//
//  const float bounds = 50.0f;
//  // add grass uniformly throughout the field, with some random variations
//  for (int i = 0; i < 1000; i++) {
//    Ogre::Vector3 pos(Ogre::Math::RangeRandom(-bounds, bounds),
//                      0,
//                      Ogre::Math::RangeRandom(-bounds, bounds));
//
//    Ogre::Quaternion ori(Ogre::Degree(Ogre::Math::RangeRandom(0, 359)), Ogre::Vector3::UNIT_Y);
//    Ogre::Vector3 scale(1, Ogre::Math::RangeRandom(0.85, 1.15), 1);
//    scale *= 0.1;
//    mField->addEntity(farn, pos, ori, scale);
//  }
//
//  mField->setVisibilityFlags(SUBMERGED_MASK);
//  mField->setRenderQueueGroup(Ogre::RENDER_QUEUE_6);
//  mField->build(); // build our static geometry (bake the grass into it)
//  mField->setCastShadows(false);

  auto *grass = scene->createEntity("Grass", "grass");
  // Create a static geometry field, which we will populate with grass
  mField = scene->createStaticGeometry("GrassField");
  UpdatePbrParams("GrassCustom");
  UpdatePbrShadowReceiver("GrassCustom");
  grass->setMaterialName("GrassCustom");
  mField->setRegionDimensions(Ogre::Vector3(20));

  // add grass uniformly throughout the field, with some random variations
  for (int i = 0; i < 10000; i++) {
    Ogre::Vector3 pos(Ogre::Math::RangeRandom(-50, 50), 0, Ogre::Math::RangeRandom(-50, 50));
    Ogre::Quaternion ori(Ogre::Degree(Ogre::Math::RangeRandom(0, 359)), Ogre::Vector3::UNIT_Y);
    Ogre::Vector3 scale(1, Ogre::Math::RangeRandom(0.85, 1.15), 1);
    scale *= 2.0f;
    mField->addEntity(grass, pos, ori, scale);
  }

  mField->setVisibilityFlags(SUBMERGED_MASK);
  mField->setRenderQueueGroup(Ogre::RENDER_QUEUE_6);
  mField->build(); // build our static geometry (bake the grass into it)
  mField->setCastShadows(false);
}
//----------------------------------------------------------------------------------------------------------------------
void Forest::Create() {
  UpdatePbrParams("3D-Diggers/farn01");
  UpdatePbrShadowReceiver("3D-Diggers/farn01");
  UpdatePbrParams("3D-Diggers/farn02");
  UpdatePbrParams("3D-Diggers/fir01");
  UpdatePbrParams("3D-Diggers/fir02");
  UpdatePbrParams("3D-Diggers/plant1");
  UpdatePbrParams("3D-Diggers/plant2");

//  GenerateTrees();
//  GeneratePlants();
//  generateBushes();
  GenerateGrass();
}
}