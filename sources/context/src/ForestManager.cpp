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

#include "ForestManager.hpp"

#include "DotSceneLoaderB.hpp"
#include "ShaderResolver.hpp"

namespace Context {

ForestManager ForestManager::staticForestManagerSingleton;

//----------------------------------------------------------------------------------------------------------------------
void ForestManager::GenerateTrees() {
  ContextManager &context = Context::ContextManager::GetSingleton();
  Ogre::Camera *ogreCamera = context.GetOgreCamera();
  Ogre::SceneNode *cameraNode = context.GetCameraNode();
  auto ogreRoot = context.GetOgreRootPtr();
  Ogre::SceneManager *sceneMgr = context.GetOgreScenePtr();

  Ogre::Entity *fir1EntPtr = sceneMgr->createEntity("fir1", "fir06_30.mesh");
  Ogre::Entity *fir2EntPtr = sceneMgr->createEntity("fir2", "fir06_30.mesh");
  Ogre::Entity *fir3EntPtr = sceneMgr->createEntity("fir3", "fir14_25.mesh");

  DotSceneLoaderB::FixPbrParams(Ogre::MaterialManager::getSingleton().getByName("3D-Diggers/farn01"));
  DotSceneLoaderB::FixPbrParams(Ogre::MaterialManager::getSingleton().getByName("3D-Diggers/farn02"));
  DotSceneLoaderB::FixPbrParams(Ogre::MaterialManager::getSingleton().getByName("3D-Diggers/fir01"));
  DotSceneLoaderB::FixPbrParams(Ogre::MaterialManager::getSingleton().getByName("3D-Diggers/fir02"));
  DotSceneLoaderB::FixPbrParams(Ogre::MaterialManager::getSingleton().getByName("3D-Diggers/plant1"));
  DotSceneLoaderB::FixPbrParams(Ogre::MaterialManager::getSingleton().getByName("3D-Diggers/plant2"));

  auto mesh = fir1EntPtr->getMesh();
  Ogre::MeshLodGenerator().generateAutoconfiguredLodLevels(mesh);
  mesh = fir2EntPtr->getMesh();
  Ogre::MeshLodGenerator().generateAutoconfiguredLodLevels(mesh);
  mesh = fir3EntPtr->getMesh();
  Ogre::MeshLodGenerator().generateAutoconfiguredLodLevels(mesh);

  auto mesh1 = fir1EntPtr->getMesh();
  auto mesh2 = fir2EntPtr->getMesh();

  auto *trees = sceneMgr->createStaticGeometry("Forest");

  float x = 0, y = 0, z = 0, yaw, scale;
  const float bound = 50;

  for (int i = 0; i < 50; i++) {
    yaw = Ogre::Math::RangeRandom(0, 360);
    if (Ogre::Math::RangeRandom(0, 1) <= 0.8f) {
      //Clump trees together occasionally
      x = Ogre::Math::RangeRandom(-bound, bound);
      z = Ogre::Math::RangeRandom(-bound, bound);
      if (x < -bound) x = -bound; else if (x > bound) x = bound;
      if (z < -bound) z = -bound; else if (z > bound) z = bound;
    } else {
      x = Ogre::Math::RangeRandom(-bound, bound);
      z = Ogre::Math::RangeRandom(-bound, bound);
    }
    y = DotSceneLoaderB::GetHeigh(x, z);
    scale = Ogre::Math::RangeRandom(0.45f, 0.55f);
    scale *= 0.2f;
    Ogre::Quaternion quat;
    quat.FromAngleAxis(Ogre::Degree(yaw), Ogre::Vector3::UNIT_Y);

    if (i % 2 == 0) {
      trees->addEntity(fir1EntPtr, Ogre::Vector3(x, y - 0.1, z), quat, Ogre::Vector3(scale));
    } else if (i % 3 == 0) {
      trees->addEntity(fir2EntPtr, Ogre::Vector3(x, y - 0.1, z), quat, Ogre::Vector3(scale));
    } else {
      trees->addEntity(fir3EntPtr, Ogre::Vector3(x, y - 0.1, z), quat, Ogre::Vector3(scale));
    }
  }

  trees->setRegionDimensions(Ogre::Vector3(20));
  trees->setCastShadows(true);
  trees->build();
  trees->setVisibilityFlags(0x0F0);
  trees->setRenderingDistance(100);
  trees->setVisible(true);
}
//----------------------------------------------------------------------------------------------------------------------
void ForestManager::GeneratePlants() {
  ContextManager &context = Context::ContextManager::GetSingleton();
  Ogre::Camera *ogreCamera = context.GetOgreCamera();
  Ogre::SceneNode *cameraNode = context.GetCameraNode();
  auto ogreRoot = context.GetOgreRootPtr();
  //Ogre::Viewport *viewport = context.GetOgreViewport();
  Ogre::SceneManager *sceneMgr = context.GetOgreScenePtr();

  Ogre::Entity *plant1EntPtr = sceneMgr->createEntity("plant1", "plant1.mesh");
  Ogre::Entity *plant2EntPtr = sceneMgr->createEntity("plant2", "plant2.mesh");
  Ogre::Entity *plant3EntPtr = sceneMgr->createEntity("plant3", "farn2.mesh");

  Ogre::StaticGeometry *plants = sceneMgr->createStaticGeometry("Plants");

  float x = 0, y = 0, z = 0, yaw, scale, bounds = 50;
  for (int i = 0; i < 500; i++) {
    yaw = Ogre::Math::RangeRandom(0, 360);
    if (Ogre::Math::RangeRandom(0, 1) <= 0.8f) {
      //Clump trees together occasionally
      x = Ogre::Math::RangeRandom(-bounds, bounds);
      z = Ogre::Math::RangeRandom(-bounds, bounds);
      if (x < -bounds) x = -bounds; else if (x > bounds) x = bounds;
      if (z < -bounds) z = -bounds; else if (z > bounds) z = bounds;
    } else {
      x = Ogre::Math::RangeRandom(-bounds, bounds);
      z = Ogre::Math::RangeRandom(-bounds, bounds);
    }
    y = DotSceneLoaderB::GetHeigh(x, z);
    scale = Ogre::Math::RangeRandom(0.9f, 1.1f);
    scale *= 0.1;

    Ogre::Quaternion quat;
    quat.FromAngleAxis(Ogre::Degree(yaw), Ogre::Vector3::UNIT_Y);

    if (i % 2 == 0) {
      plants->addEntity(plant1EntPtr, Ogre::Vector3(x, y, z), quat, Ogre::Vector3(scale));
    } else if (i % 3 == 0) {
      plants->addEntity(plant2EntPtr, Ogre::Vector3(x, y, z), quat, Ogre::Vector3(scale));
    } else {
      plants->addEntity(plant3EntPtr, Ogre::Vector3(x, y, z), quat, Ogre::Vector3(scale));
    }

  }

  plants->setCastShadows(false);
  plants->build();
  plants->setRegionDimensions(Ogre::Vector3(20));
}
//----------------------------------------------------------------------------------------------------------------------
void ForestManager::generateBushes() {
//  ContextManager &context = Context::ContextManager::GetSingleton();
//  Ogre::Camera *ogreCamera = context.GetOgreCamera();
//  Ogre::SceneNode *cameraNode = context.GetCameraNode();
//  auto ogreRoot = context.GetOgreRootPtr();
//  Ogre::SceneManager *sceneMgr = context.GetOgreScenePtr();
//
//  Ogre::Entity *plant1EntPtr = sceneMgr->createEntity("plant1", "plant1.mesh");
//  Ogre::Entity *plant2EntPtr = sceneMgr->createEntity("plant2", "plant2.mesh");
//  Ogre::Entity *plant3EntPtr = sceneMgr->createEntity("plant3", "farn2.mesh");
//
//  Ogre::StaticGeometry *plants = sceneMgr->createStaticGeometry("Trees");
//  float x = 0, y = 0, z = 0, yaw, scale;
//  for (int i = 0; i < g_numberOfTrees; i++) {
//    yaw = Ogre::Math::RangeRandom(0, 360);
//    if (Ogre::Math::RangeRandom(0, 1) <= 0.8f) {
//      //Clump trees together occasionally
//      x = Ogre::Math::RangeRandom(-g_worldBounds, g_worldBounds);
//      z = Ogre::Math::RangeRandom(-g_worldBounds, g_worldBounds);
//      if (x < -g_worldBounds) x = -g_worldBounds; else if (x > g_worldBounds) x = g_worldBounds;
//      if (z < -g_worldBounds) z = -g_worldBounds; else if (z > g_worldBounds) z = g_worldBounds;
//    } else {
//      x = Ogre::Math::RangeRandom(-g_worldBounds, g_worldBounds);
//      z = Ogre::Math::RangeRandom(-g_worldBounds, g_worldBounds);
//    }
//    y = DotSceneLoaderB::GetHeigh(x, z);
//    scale = Ogre::Math::RangeRandom(0.9f, 1.1f);
//    //        Ogre::SceneManager::getRootSceneNode()->createChildSceneNode();
//    Ogre::Quaternion quat;
//    quat.FromAngleAxis(Ogre::Degree(yaw), Ogre::Vector3::UNIT_Y);
//
//    if (i % 2 == 0) {
//      plants->addEntity(plant1EntPtr, Ogre::Vector3(x, y, z), quat, Ogre::Vector3(scale));
//    } else if (i % 3 == 0) {
//      plants->addEntity(plant2EntPtr, Ogre::Vector3(x, y, z), quat, Ogre::Vector3(scale));
//    } else {
//      plants->addEntity(plant3EntPtr, Ogre::Vector3(x, y, z), quat, Ogre::Vector3(scale));
//    }
//
//  }
//
//  plants->setCastShadows(false);
//
//  plants->setVisible(true);
//
//  plants->build();

}
//----------------------------------------------------------------------------------------------------------------------
#pragma pack(push, 1)
struct GrassVertex {
  float x, y, z;
  float nx, ny, nz;
  float u, v;
};
#pragma pack(pop)
//----------------------------------------------------------------------------------------------------------------------
void ForestManager::createGrassMesh() {
  Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().createManual("grass", "General");

  // Create a submesh with the grass material
  Ogre::SubMesh *sm = mesh->createSubMesh();
  const std::string grassMaterial = "GrassCustom";
  Ogre::MaterialPtr tmp = Ogre::MaterialManager::getSingleton().getByName(grassMaterial);
  DotSceneLoaderB::FixPbrParams(tmp);
  DotSceneLoaderB::FixPbrShadow(tmp);

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
void ForestManager::GenerateGrass() {
  ContextManager &context = Context::ContextManager::GetSingleton();
  Ogre::SceneManager *sceneMgr = context.GetOgreScenePtr();
  // create our grass mesh, and Create a grass entity from it
  createGrassMesh();

  Ogre::Entity *grass = sceneMgr->createEntity("Grass", "grass");

  // Create a static geometry field, which we will populate with grass
  mField = sceneMgr->createStaticGeometry("GrassField");
  mField->setRegionDimensions(Ogre::Vector3(20));

  const float step = 0.5f;
  const float bounds = 20.0f;
  // add grass uniformly throughout the field, with some random variations
  for (float x = -bounds; x < bounds; x += step) {
    for (float z = -bounds; z < bounds; z += step) {
      Ogre::Vector3 pos(x + Ogre::Math::RangeRandom(-step, step),
                        DotSceneLoaderB::GetHeigh(x, z),
                        z + Ogre::Math::RangeRandom(-step, step));
      Ogre::Quaternion ori(Ogre::Degree(Ogre::Math::RangeRandom(0, 359)), Ogre::Vector3::UNIT_Y);
      Ogre::Vector3 scale(1, Ogre::Math::RangeRandom(0.85, 1.15), 1);

      mField->addEntity(grass, pos, ori, 2 * scale);
    }
  }

  mField->setRenderQueueGroup(Ogre::RENDER_QUEUE_6);
  mField->build(); // build our static geometry (bake the grass into it)
  mField->setCastShadows(false);
}
//----------------------------------------------------------------------------------------------------------------------
void ForestManager::Create() {
  GenerateTrees();
  GeneratePlants();
//  generateBushes();
  GenerateGrass();
}
//----------------------------------------------------------------------------------------------------------------------
bool ForestManager::frameRenderingQueued(const Ogre::FrameEvent &evt) {
  float ellapsed = evt.timeSinceLastFrame;

  const float fadeRange = 100.0f;
  static float waveCount = 4.0f;
  const float animFreq = 1.0f;
  const float animSpeed = 0.4f;
  const auto windDir = Ogre::Vector3::UNIT_X;
  const float animMag = 0.2f;

//  if (mField) {
    waveCount += ellapsed * (animSpeed * Ogre::Math::PI);
    if (waveCount > Ogre::Math::PI * 2) waveCount -= Ogre::Math::PI * 2;

    auto params = Ogre::MaterialManager::getSingleton().getByName("GrassCustom")->\
            getTechnique(0)->getPass(0)->getVertexProgramParameters();

//    params->setNamedConstant("uTime", waveCount);
//    params->setNamedConstant("frequency", animFreq);
    params->setNamedConstant("fadeRange", fadeRange);
    Ogre::Vector3 direction = windDir * animMag;
    params->setNamedConstant("uWaveDirection", Ogre::Vector4(direction.x, direction.y, direction.z, 0));

    params = Ogre::MaterialManager::getSingleton().getByName("3D-Diggers/farn01")->\
            getTechnique(0)->getPass(0)->getVertexProgramParameters();
//    params->setNamedConstant("uTime", waveCount);
//    params->setNamedConstant("frequency", animFreq);
    params->setNamedConstant("fadeRange", fadeRange);
    params->setNamedConstant("uWaveDirection", Ogre::Vector4(direction.x, direction.y, direction.z, 0));

    params = Ogre::MaterialManager::getSingleton().getByName("3D-Diggers/farn02")->\
            getTechnique(0)->getPass(0)->getVertexProgramParameters();
//    params->setNamedConstant("uTime", waveCount);
//    params->setNamedConstant("frequency", animFreq);
    params->setNamedConstant("fadeRange", fadeRange);
    params->setNamedConstant("uWaveDirection", Ogre::Vector4(direction.x, direction.y, direction.z, 0));

    params = Ogre::MaterialManager::getSingleton().getByName("3D-Diggers/plant1")->\
            getTechnique(0)->getPass(0)->getVertexProgramParameters();
//    params->setNamedConstant("uTime", waveCount);
//    params->setNamedConstant("frequency", animFreq);
    params->setNamedConstant("fadeRange", fadeRange);
    params->setNamedConstant("uWaveDirection", Ogre::Vector4(direction.x, direction.y, direction.z, 0));

    params = Ogre::MaterialManager::getSingleton().getByName("3D-Diggers/plant2")->\
            getTechnique(0)->getPass(0)->getVertexProgramParameters();
//    params->setNamedConstant("uTime", waveCount);
//    params->setNamedConstant("frequency", animFreq);
    params->setNamedConstant("fadeRange", fadeRange);
    params->setNamedConstant("uWaveDirection", Ogre::Vector4(direction.x, direction.y, direction.z, 0));

    params = Ogre::MaterialManager::getSingleton().getByName("3D-Diggers/fir01")->\
            getTechnique(0)->getPass(0)->getVertexProgramParameters();
//    params->setNamedConstant("uTime", waveCount);
//    params->setNamedConstant("frequency", animFreq);
    params->setNamedConstant("fadeRange", 500.0f);
    params->setNamedConstant("uWaveDirection", Ogre::Vector4(direction.x, direction.y, direction.z, 0));
//  }

  return true;
}
//----------------------------------------------------------------------------------------------------------------------
void ForestManager::Reset() {
  if (Ogre::MeshManager::getSingleton().getByName("grass",
                                                  Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME)) {
    Ogre::MeshManager::getSingleton().remove("grass", Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
  }
}

} //namespace Context