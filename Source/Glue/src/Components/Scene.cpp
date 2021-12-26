// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Components/Scene.h"
#include "BtOgre/BtOgre.h"
#include "Components/ComponentsAll.h"
#include "Engine.h"
#include "Objects/CameraMan.h"
#include "Objects/SinbadCharacterController.h"
#include "PBR.h"
#include "PagedGeometry/PagedGeometryAll.h"
#include "XmlParser.h"
#ifdef OGRE_BUILD_COMPONENT_MESHLODGENERATOR
#include <MeshLodGenerator/OgreLodConfig.h>
#include <MeshLodGenerator/OgreMeshLodGenerator.h>
#include <OgreDistanceLodStrategy.h>
#include <OgrePixelCountLodStrategy.h>
#endif
#ifdef OGRE_BUILD_COMPONENT_TERRAIN
#include "Components/TerrainMaterialGeneratorB.h"
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainAutoUpdateLod.h>
#include <Terrain/OgreTerrainGroup.h>
#include <Terrain/OgreTerrainMaterialGeneratorA.h>
#include <Terrain/OgreTerrainQuadTreeNode.h>
#endif
#ifdef OGRE_BUILD_COMPONENT_PAGING
#include <Paging/OgrePage.h>
#include <Paging/OgrePaging.h>
#endif

using namespace std;
using namespace Ogre;
using namespace Forests;

namespace Glue {

Scene::Scene() {
  CameraManPtr = make_unique<CameraMan>();
  OgreRoot = Root::getSingletonPtr();
  OgreScene = OgreRoot->getSceneManager("Default");
  RootNode = OgreScene->getRootSceneNode();
}

Scene::~Scene() {}

CameraMan &Scene::GetCamera() const { return *CameraManPtr; }

float Scene::GetHeight(float x, float z) {
  if (OgreTerrainList)
    return OgreTerrainList->getHeightAtWorldPosition(x, 1000, z);
  else
    return 0.0f;
}

void Scene::AddCamera(Camera *OgreCameraPtr) {}

void Scene::AddSinbad(Camera *OgreCameraPtr) { Sinbad = make_unique<SinbadCharacterController>(OgreCameraPtr); }

void Scene::AddForests(PagedGeometry *PGPtr) { PagedGeometryList.push_back(unique_ptr<PagedGeometry>(PGPtr)); }

void Scene::AddTerrain(TerrainGroup *TGP) { OgreTerrainList.reset(TGP); }

void Scene::OnUpdate(float PassedTime) {
  if (Paused) return;
  if (CameraManPtr) CameraManPtr->Update(PassedTime);
  if (Sinbad) Sinbad->Update(PassedTime);
  for (auto &it : PagedGeometryList) it->update();
}

void Scene::OnClean() {
  Sinbad.reset();
  PagedGeometryList.clear();
  if (OgreTerrainList) OgreTerrainList->removeAllTerrains();
  OgreTerrainList.reset();
  auto *TGO = Ogre::TerrainGlobalOptions::getSingletonPtr();
  if (TGO) delete TGO;
  if (OgreScene) OgreScene->setShadowTechnique(SHADOWTYPE_NONE);
  if (OgreScene) OgreScene->clearScene();
  if (CameraManPtr) CameraManPtr->SetStyle(CameraMan::Style::MANUAL);
  ResourceGroupManager::getSingleton().unloadResourceGroup(GroupName);
}

}  // namespace Glue
