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

#include "PagedForestManager.hpp"

#include "DotSceneLoaderB.hpp"

const bool g_enablePagedGeometry = true;
const bool g_enableFog = true;
const bool g_saveTerrain = false;

const float g_worldSize = 500.0f;
const float g_worldBounds = (g_worldSize / 2.0f);
const float g_numberOfTrees = 100.0f;
const float g_treeDistance = 100.0f;
const float g_treeImposterDistance = 90.0f;
const float g_treeWindFactor = 20.0f;
const float g_grassDistance = 100.0f;
const float g_plantWindFactor = 3.0f;
const float g_plantDistance = 50.0f;
const float g_numberOfPlants = 200;
const float g_pageSize_Grass = 50.0f;
const float g_pageSize_Plants = 50.0f;
const float g_pageSize_Trees = 50.0f;

static float heightFunction(float x, float z, void *userData) {
//    return WorldTerrain::getSingleton().heightFunction(x, z, userData);
  return 0;
}

namespace Context {

PagedForestManager PagedForestManager::pagedForestManagerSingleton;

//----------------------------------------------------------------------------------------------------------------------
void PagedForestManager::Create() {
  trees = new Forests::PagedGeometry(ogre_camera_, g_pageSize_Trees);
  trees->addDetailLevel<Forests::WindBatchPage>(g_treeDistance * 0.9f, 60);
  trees->addDetailLevel<Forests::BatchPage>(g_treeDistance, 20);
//  trees->addDetailLevel<Forests::ImpostorPage>(g_treeImposterDistance, 20);

  auto *treeLoader = new Forests::TreeLoader3D(trees, Forests::TBounds(-g_worldBounds, -g_worldBounds, g_worldBounds,
                                                                       g_worldBounds));
  trees->setPageLoader(treeLoader);
//    trees->setShadersEnabled(true);

  Ogre::Entity *fir1EntPtr = ogre_scene_manager_->createEntity("fir1", "fir05_30.mesh");
  Ogre::Entity *fir2EntPtr = ogre_scene_manager_->createEntity("fir2", "fir06_30.mesh");
  Ogre::Entity *fir3EntPtr = ogre_scene_manager_->createEntity("fir3", "fir14_25.mesh");
//    fir1EntPtr->setCastShadows(false);
//    fir2EntPtr->setCastShadows(false);
//    fir3EntPtr->setCastShadows(false);
//    Ogre::Entity *farnEntPtr = sceneMgr->createOgre::Entity("farn2.mesh");

  trees->setCustomParam("fir1", "windFactorX", g_treeWindFactor);
  trees->setCustomParam("fir2", "windFactorX", g_treeWindFactor);
  trees->setCustomParam("fir3", "windFactorX", g_treeWindFactor);


//Add plants
  float x = 0, y = 0, z = 0, yaw, scale;
  for (int i = 0; i < g_numberOfTrees; i++) {
    yaw = Ogre::Math::RangeRandom(0, 360);
    if (Ogre::Math::RangeRandom(0, 1) <= 0.8f) {
//Clump trees together occasionally
      x = Ogre::Math::RangeRandom(-g_worldBounds, g_worldBounds);
      z = Ogre::Math::RangeRandom(-g_worldBounds, g_worldBounds);
      if (x < -g_worldBounds) x = -g_worldBounds; else if (x > g_worldBounds) x = g_worldBounds;
      if (z < -g_worldBounds) z = -g_worldBounds; else if (z > g_worldBounds) z = g_worldBounds;
    } else {
      x = Ogre::Math::RangeRandom(-g_worldBounds, g_worldBounds);
      z = Ogre::Math::RangeRandom(-g_worldBounds, g_worldBounds);
    }
//        y = WorldTerrain::getSingleton().getY(x, z);
    scale = Ogre::Math::RangeRandom(0.09f, 0.1f);
//        Ogre::SceneManager::getRootSceneNode()->createChildSceneNode();
    Ogre::Quaternion quat;
    quat.FromAngleAxis(Ogre::Degree(yaw), Ogre::Vector3::UNIT_Y);

    if (i % 2 == 0) {
      treeLoader->addTree(fir1EntPtr, Ogre::Vector3(x, y, z), Ogre::Degree(yaw), scale);
    } else if (i % 3 == 0) {
      treeLoader->addTree(fir2EntPtr, Ogre::Vector3(x, y, z), Ogre::Degree(yaw), scale);
    } else {
      treeLoader->addTree(fir3EntPtr, Ogre::Vector3(x, y, z), Ogre::Degree(yaw), scale);
    }
  }

  //Add plants
  Ogre::Entity *plant1EntPtr = ogre_scene_manager_->createEntity("plant1", "plant1.mesh");
  Ogre::Entity *plant2EntPtr = ogre_scene_manager_->createEntity("plant2", "plant2.mesh");
  Ogre::Entity *plant3EntPtr = ogre_scene_manager_->createEntity("plant3", "farn2.mesh");

  plants = new Forests::PagedGeometry(ogre_camera_, g_pageSize_Plants);
  plants->addDetailLevel<Forests::WindBatchPage>(g_plantDistance * 0.9f, 60);
  plants->addDetailLevel<Forests::BatchPage>(g_plantDistance, 60);
//    plants->addDetailLevel<Forests::ImpostorPage>(g_plantsImposterDistance, 0);

  auto *plantsLoader = new Forests::TreeLoader3D(plants, Forests::TBounds(-g_worldBounds, -g_worldBounds, g_worldBounds,
                                                                          g_worldBounds));

  plants->setCustomParam("plant1", "windFactorX", g_plantWindFactor);
  plants->setCustomParam("plant2", "windFactorX", g_plantWindFactor);
  plants->setCustomParam("plant3", "windFactorX", g_plantWindFactor);
//    plantsLoader->
//    plants->

  plants->setPageLoader(plantsLoader);
//    plants->setC

  for (int i = 0; i < g_numberOfPlants; i++) {
    yaw = Ogre::Math::RangeRandom(0, 360);
    if (Ogre::Math::RangeRandom(0, 1) <= 0.8f) {
//Clump trees together occasionally
      x = Ogre::Math::RangeRandom(-g_worldBounds, g_worldBounds);
      z = Ogre::Math::RangeRandom(-g_worldBounds, g_worldBounds);
      if (x < -g_worldBounds) x = -g_worldBounds; else if (x > g_worldBounds) x = g_worldBounds;
      if (z < -g_worldBounds) z = -g_worldBounds; else if (z > g_worldBounds) z = g_worldBounds;
    } else {
      x = Ogre::Math::RangeRandom(-g_worldBounds, g_worldBounds);
      z = Ogre::Math::RangeRandom(-g_worldBounds, g_worldBounds);
    }
//        y = WorldTerrain::getSingleton().getY(x, z);
    y = DotSceneLoaderB::GetHeigh(x, z);
    scale = Ogre::Math::RangeRandom(0.09f, 0.1f);

    if (i % 2 == 0) {
      plantsLoader->addTree(plant1EntPtr, Ogre::Vector3(x, y, z), Ogre::Degree(yaw), scale);
    } else if (i % 3 == 0) {
      plantsLoader->addTree(plant2EntPtr, Ogre::Vector3(x, y, z), Ogre::Degree(yaw), scale);
    } else {
      plantsLoader->addTree(plant3EntPtr, Ogre::Vector3(x, y, z), Ogre::Degree(yaw), scale);
    }

  }

//    //Grass
  grass = new Forests::PagedGeometry(ogre_camera_, g_pageSize_Grass);
  grass->addDetailLevel<Forests::GrassPage>(g_grassDistance);//Draw grass up to 100

  auto *grassLoader = new Forests::GrassLoader(grass);
  grass->setPageLoader(grassLoader);
  grassLoader->setHeightFunction(&heightFunction);
  Forests::GrassLayer *layer = grassLoader->addLayer("grass");

  layer->setMinimumSize(0.6f, 0.6);
  layer->setMaximumSize(1, 1);
  layer->setAnimationEnabled(true);
  layer->setSwayDistribution(7.0f);
  layer->setSwayLength(0.5f);
  layer->setMaxSlope(0.1f);
  layer->setSwaySpeed(0.4f);
  layer->setDensity(0.05);
  layer->setRenderTechnique(Forests::GRASSTECH_CROSSQUADS);
  layer->setFadeTechnique(Forests::FADETECH_GROW);

  layer->setMapBounds(Forests::TBounds(-g_worldBounds, -g_worldBounds, g_worldBounds, g_worldBounds));
//    layer->setColorMap("grass_green-01_diffusespecular.dds");
//    layer->setColorMap(WorldTerrain::getSingleton().getMTerrainGroup()->getTerrain(0, 0)->getLightmap());
//    layer->setColorMap("terrain_lightmap.jpg");

//    layer->setLightingEnabled(true);
//    layer->setMaterialName("grass");
//    layer->setDensityMap("densitymap.png");

}
//----------------------------------------------------------------------------------------------------------------------
bool PagedForestManager::frameRenderingQueued(const Ogre::FrameEvent &evt) {
  if (grass)
    grass->update();
  if (trees)
    trees->update();
  if (plants)
    plants->update();

  return true;
}
//----------------------------------------------------------------------------------------------------------------------
void PagedForestManager::Reset() {
}

} //namespace Context