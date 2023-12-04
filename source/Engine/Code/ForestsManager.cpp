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
  sceneManager = ogreRoot->getSceneManager("Default");
  ASSERTION(sceneManager, "[ForestsManager] sceneManager is not initialised");
  ASSERTION(sceneManager->hasCamera("Camera"), "[ForestsManager] camera is not initialised");
  camera = sceneManager->getCamera("Camera");
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
