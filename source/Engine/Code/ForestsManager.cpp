/// created by Andrey Vasiliev

#include "pch.h"
#include "ForestsManager.h"
#include "TerrainManager.h"
#include <PagedGeometry/PagedGeometryAll.h>

using namespace std;

namespace gge {
ForestsManager::ForestsManager() {}
ForestsManager::~ForestsManager() {}

void ForestsManager::OnSetUp() {
  ogreRoot = Ogre::Root::getSingletonPtr();
  ASSERTION(ogreRoot, "[ForestsManager] ogreRoot is not initialised");
  ogreSceneManager = ogreRoot->getSceneManager("Default");
  ASSERTION(ogreSceneManager, "[ForestsManager] ogreSceneManager is not initialised");
  ASSERTION(ogreSceneManager->hasCamera("Camera"), "[ForestsManager] ogreCamera is not initialised");
  ogreCamera = ogreSceneManager->getCamera("Camera");
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

void ForestsManager::OnUpdate(Ogre::Real time) {
  for (auto it : pagedGeometryList) {
    it->update(time);
  }
}
void ForestsManager::OnClean() {
  for (vector<Forests::PagedGeometry *>::reverse_iterator it = pagedGeometryList.rbegin(); it != pagedGeometryList.rend(); ++it) {
    delete *it;
  }

  pagedGeometryList.clear();
  pagedGeometryList.shrink_to_fit();
}
}  // namespace gge
