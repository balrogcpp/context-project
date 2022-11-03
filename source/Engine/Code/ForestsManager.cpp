/// created by Andrey Vasiliev

#include "pch.h"
#include "ForestsManager.h"
#include <Ogre.h>

using namespace std;

namespace Glue {
ForestsManager::ForestsManager() {}
ForestsManager::~ForestsManager() {}

void ForestsManager::OnSetUp() {
  ogreRoot = Ogre::Root::getSingletonPtr();
  ASSERTION(ogreRoot, "[ForestsManager] ogreRoot is not initialised");
  ogreSceneManager = ogreRoot->getSceneManager("Default");
  ASSERTION(ogreSceneManager, "[ForestsManager] ogreSceneManager is not initialised");
  ASSERTION(ogreSceneManager->hasCamera("Default"), "[ForestsManager] ogreCamera is not initialised");
  ogreCamera = ogreSceneManager->getCamera("Default");
}

void ForestsManager::AddGrass() {
  const float bound = 95;

  auto *grass = new Forests::PagedGeometry(ogreCamera, 15);
  grass->addDetailLevel<Forests::GrassPage>(60, 0);
  auto *grassLoader = new Forests::GrassLoader(grass);
  grass->setPageLoader(grassLoader);
  //grassLoader->setHeightFunction([](float x, float z, void *) { return GetScene().GetHeight(x, z); });

  Forests::GrassLayer *layer = grassLoader->addLayer("GrassCustom");
  layer->setFadeTechnique(Forests::FADETECH_ALPHA);
  layer->setRenderTechnique(Forests::GRASSTECH_CROSSQUADS);
  layer->setMapBounds(Forests::TBounds(-bound, -bound, bound, bound));
  //layer->setDensityMap();
  //layer->setColorMap();
  layer->setMaximumSize(2.0f, 2.0f);
  layer->setDensity(0.5f);

  grass->update();

  pagedGeometryList.push_back(grass);

  float x = 0, y = 0, z = 0, yaw, scale = 1.0;
  auto *trees = new Forests::PagedGeometry(ogreCamera, 50);

  trees->addDetailLevel<Forests::WindBatchPage>(125, 0);
  trees->addDetailLevel<Forests::ImpostorPage>(400, 0);

  auto *treeLoader = new Forests::TreeLoader3D(trees, Forests::TBounds(-bound, -bound, bound, bound));

  trees->setPageLoader(treeLoader);
  Ogre::Entity *fir2EntPtr = ogreSceneManager->createEntity("fir1", "fir06_30.mesh");
  Ogre::Entity *fir1EntPtr = ogreSceneManager->createEntity("fir2", "fir05_30.mesh");

  trees->setCustomParam(fir1EntPtr->getName(), "windFactorX", 30);
  trees->setCustomParam(fir1EntPtr->getName(), "windFactorY", 0.01);
  trees->setCustomParam(fir2EntPtr->getName(), "windFactorX", 15);
  trees->setCustomParam(fir2EntPtr->getName(), "windFactorY", 0.01);

  for (int i = 0; i < 50; i++) {
    yaw = Ogre::Math::RangeRandom(0, 360);
    Ogre::Quaternion quat;

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

    //y = GetScene().GetHeight(x, z);

    scale = Ogre::Math::RangeRandom(0.9f, 1.1f);
    quat.FromAngleAxis(Ogre::Degree(yaw), Ogre::Vector3::UNIT_Y);

    treeLoader->addTree(fir1EntPtr, Ogre::Vector3(x, y, z), Ogre::Degree(yaw), scale * 0.2);
  }

  trees->update();
  trees->setCastsShadows(false);
}

void ForestsManager::RegPagedGeometry(Forests::PagedGeometry *pagedGeometry) {
  pagedGeometry->update();
  if (find(pagedGeometryList.begin(), pagedGeometryList.end(), pagedGeometry) == pagedGeometryList.end()) {
    pagedGeometryList.push_back(pagedGeometry);
  }
}

void ForestsManager::UnregPagedGeometry(Forests::PagedGeometry *pagedGeometry) {
  auto it = find(pagedGeometryList.begin(), pagedGeometryList.end(), pagedGeometry), end = pagedGeometryList.end();
  if (it != end) {
    swap(it, --end);
    pagedGeometryList.pop_back();
  }
}

void ForestsManager::OnUpdate(float time) {
  for (auto it : pagedGeometryList) {
    it->update();
  }
}
void ForestsManager::OnClean() {
  for (vector<Forests::PagedGeometry *>::reverse_iterator it = pagedGeometryList.rbegin(); it != pagedGeometryList.rend(); ++it) {
    delete *it;
  }

  pagedGeometryList.clear();
  pagedGeometryList.shrink_to_fit();
}
}  // namespace Glue
