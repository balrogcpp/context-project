/// created by Andrey Vasiliev

#include "pch.h"
#include "ForestsManager.h"
#include "TerrainManager.h"
#include <PagedGeometry/PagedGeometryAll.h>

using namespace std;

namespace Glue {
ForestsManager::ForestsManager() { pagedGeometryList.reserve(200); }
ForestsManager::~ForestsManager() {}

void ForestsManager::OnSetUp() {
  ogreRoot = Ogre::Root::getSingletonPtr();
  ASSERTION(ogreRoot, "[ForestsManager] ogreRoot is not initialised");
  ogreSceneManager = ogreRoot->getSceneManager("Default");
  ASSERTION(ogreSceneManager, "[ForestsManager] ogreSceneManager is not initialised");
  ASSERTION(ogreSceneManager->hasCamera("Default"), "[ForestsManager] ogreCamera is not initialised");
  ogreCamera = ogreSceneManager->getCamera("Default");
}

void ForestsManager::RegPagedGeometry(Forests::PagedGeometry *pagedGeometry) {
  pagedGeometry->update(0);
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
  if (!_sleep) {
    for (auto it : pagedGeometryList) {
      it->update(time);
      it->setCastShadows(false);
    }
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
