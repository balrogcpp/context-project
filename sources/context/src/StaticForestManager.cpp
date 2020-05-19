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

#include "pcheader.hpp"

#include "StaticForestManager.hpp"

#include "DotSceneLoaderB.hpp"
#include "ShaderResolver.hpp"

namespace Context {

StaticForestManager StaticForestManager::staticForestManagerSingleton;

#pragma pack(push, 1)
struct GrassVertex {
  float x, y, z;
  float nx, ny, nz;
  float u, v;
};
#pragma pack(pop)
//----------------------------------------------------------------------------------------------------------------------
void StaticForestManager::CreateGrassMesh() {
  Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().createManual("grass", "General");

  // Create a submesh with the grass material
  Ogre::SubMesh *sm = mesh->createSubMesh();
  const std::string grassMaterial = "GrassCustom";
  Ogre::MaterialPtr tmp = Ogre::MaterialManager::getSingleton().getByName(grassMaterial);

  DotSceneLoaderB::FixPbrParams(tmp);
  DotSceneLoaderB::FixPbrShadowReceiver(tmp);

  sm->setMaterialName(grassMaterial);
  sm->useSharedVertices = false;
  sm->vertexData = new Ogre::VertexData();
  sm->vertexData->vertexStart = 0;
  sm->vertexData->vertexCount = 8; //12
  sm->indexData->indexCount = 12; //18

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
    float x = Ogre::Math::Cos(Ogre::Degree(i * 60)) * GRASS_WIDTH / 2;
    float z = Ogre::Math::Sin(Ogre::Degree(i * 60)) * GRASS_WIDTH / 2;

    for (int j = 0; j < 4; j++)  // each plane has 4 vertices
    {
      GrassVertex &vert = verts[i * 4 + j];

      vert.x = j < 2 ? -x : x;
      vert.y = j % 2 ? 0 : GRASS_HEIGHT;
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
void StaticForestManager::GenerateGrass() {
  ContextManager &context = Context::ContextManager::GetSingleton();
  Ogre::SceneManager *sceneMgr = context.GetOgreScenePtr();
  // create our grass mesh, and Create a grass entity from it
//  createGrassMesh();

  Ogre::Entity *grass = sceneMgr->createEntity("Grass", "farn1.mesh");

  // Create a static geometry field, which we will populate with grass
  mField = sceneMgr->createStaticGeometry("GrassField");
  mField->setRegionDimensions(Ogre::Vector3(20));

  const float step = 0.5f;
  const float bounds = 50.0f;
  // add grass uniformly throughout the field, with some random variations
  for (int i = 0; i < 1000; i++) {
    Ogre::Vector3 pos(Ogre::Math::RangeRandom(-bounds, bounds),
                      0,
                      Ogre::Math::RangeRandom(-bounds, bounds));

    pos.y += DotSceneLoaderB::GetSingleton().GetHeigh(pos.x, pos.z);

    Ogre::Quaternion ori(Ogre::Degree(Ogre::Math::RangeRandom(0, 359)), Ogre::Vector3::UNIT_Y);
    Ogre::Vector3 scale(1, Ogre::Math::RangeRandom(0.85, 1.15), 1);
    scale *= 0.1;
    mField->addEntity(grass, pos, ori, scale);
  }

//  mField->setRenderQueueGroup(Ogre::RENDER_QUEUE_6);
  mField->build(); // build our static geometry (bake the grass into it)
  mField->setCastShadows(false);
}
//----------------------------------------------------------------------------------------------------------------------
void StaticForestManager::Create() {
  DotSceneLoaderB::FixPbrParams("3D-Diggers/farn01");
  DotSceneLoaderB::FixPbrShadowReceiver("3D-Diggers/farn01");
  DotSceneLoaderB::FixPbrParams("3D-Diggers/farn02");
  DotSceneLoaderB::FixPbrParams("3D-Diggers/fir01");
  DotSceneLoaderB::FixPbrParams("3D-Diggers/fir02");
  DotSceneLoaderB::FixPbrParams("3D-Diggers/plant1");
  DotSceneLoaderB::FixPbrParams("3D-Diggers/plant2");

//  GenerateTrees();
//  GeneratePlants();
//  generateBushes();
  GenerateGrass();
}
//----------------------------------------------------------------------------------------------------------------------
bool StaticForestManager::frameRenderingQueued(const Ogre::FrameEvent &evt) {

  return true;
}
//----------------------------------------------------------------------------------------------------------------------
void StaticForestManager::Reset() {
  if (Ogre::MeshManager::getSingleton().getByName("grass",
                                                  Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME)) {
    Ogre::MeshManager::getSingleton().remove("grass", Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
  }
}

} //namespace Context