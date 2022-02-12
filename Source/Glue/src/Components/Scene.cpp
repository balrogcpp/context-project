// This source file is part of "glue project". Created by Andrey Vasiliev

#include "PCHeader.h"
#include "Components/Scene.h"
#include "Engine.h"
#include "Objects/CameraMan.h"
#include "Objects/SinbadCharacterController.h"
#include "PagedGeometry/PagedGeometryAll.h"
#include "ShaderHelpers.h"
#include "SkyModel.h"
#include "ArHosekSkyModel.h"
#include "Caelum.h"
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

Ogre::Vector3 Scene::GetSunPosition() {
  auto *SunPtr = OgreScene->getLight("Sun");
  if (SunPtr)
    return Vector3(SunPtr->getDerivedDirection().normalisedCopy());
  else
    return Vector3();
}

float Scene::GetHeight(float x, float z) {
  if (OgreTerrainList)
    return OgreTerrainList->getHeightAtWorldPosition(x, 1000, z);
  else
    return 0.0f;
}

void Scene::AddEntity(Ogre::Entity *EntityPtr) { FixEntityMaterial(EntityPtr); }

void Scene::AddMaterial(Ogre::MaterialPtr material) {
  for (int i = 0; i < material->getNumTechniques(); i++) {
    if (!material->getTechnique(i)->getPass(0)->hasVertexProgram() || !material->getTechnique(i)->getPass(0)->hasFragmentProgram()) {
      return;
    }

    gpu_vp_params_.push_back(material->getTechnique(i)->getPass(0)->getVertexProgramParameters());
    gpu_fp_params_.push_back(material->getTechnique(i)->getPass(0)->getFragmentProgramParameters());
  }
}

void Scene::AddMaterial(const std::string &MaterialName) {
  auto MaterialPtr = MaterialManager::getSingleton().getByName(MaterialName);
  if (MaterialPtr) AddMaterial(MaterialPtr);
}

void Scene::AddCamera(Camera *OgreCameraPtr) {}

void Scene::AddSinbad(Camera *OgreCameraPtr) {
  Sinbad = make_unique<SinbadCharacterController>(OgreCameraPtr);
  InputSequencer::GetInstance().RegObserver(Sinbad.get());
}

void Scene::AddForests(PagedGeometry *PGPtr, const std::string &MaterialName) {
  PagedGeometryList.push_back(unique_ptr<PagedGeometry>(PGPtr));
  if (!MaterialName.empty()) AddMaterial(MaterialName);
}

void Scene::AddTerrain(TerrainGroup *TGP) { OgreTerrainList.reset(TGP); }

void Scene::AddSkyBox() {
  auto colorspace = ACEScg;
  float sunSize = 0.27f;
  float turbidity = 4.0f;
  auto groundAlbedo = Vector3(0.18);
  auto sunColor = Vector3(20000);
  auto sunDir = GetSunPosition();

  SkyModel sky;
  sky.SetupSky(sunDir, sunSize, sunColor, groundAlbedo, turbidity, colorspace);

  const static array<string, 10> ParamList{"A", "B", "C", "D", "E", "F", "G", "H", "I", "Z"};

  for (int i = 0; i < 9; i++)
    for (int j = 0; j < 3; j++) HosekParams[i][j] = sky.StateX->configs[j][i];
  HosekParams[9] = Vector3(1.0);

  auto SkyMaterial = MaterialManager::getSingleton().getByName("SkyBox");
  if (!SkyMaterial) return;

  auto FpParams = SkyMaterial->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
  if (FpParams) SkyBoxFpParams = FpParams;

  for (int i = 0; i < 10; i++) FpParams->setNamedConstant(ParamList[i], HosekParams[i]);
}

void Scene::OnUpdate(float PassedTime) {
  if (Paused) return;
  if (CameraManPtr) CameraManPtr->Update(PassedTime);
  if (Sinbad) Sinbad->Update(PassedTime);
  for (auto &it : PagedGeometryList) it->update();

  static Camera *CameraPtr = Root::getSingleton().getSceneManager("Default")->getCamera("Default");
  static Matrix4 MVP;
  static Matrix4 MVPprev;

  MVPprev = MVP;
  MVP = CameraPtr->getProjectionMatrixWithRSDepth() * CameraPtr->getViewMatrix();

  for (auto &it : gpu_vp_params_) it->setNamedConstant("cWorldViewProjPrev", MVPprev);

  if (SkyNeedsUpdate) {
    const static array<string, 10> ParamList{"A", "B", "C", "D", "E", "F", "G", "H", "I", "Z"};
    if (SkyBoxFpParams)
      for (int i = 0; i < 10; i++) SkyBoxFpParams->setNamedConstant(ParamList[i], HosekParams[i]);
  }
}

void Scene::OnClean() {
  InputSequencer::GetInstance().UnRegObserver(Sinbad.get());
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
  gpu_fp_params_.clear();
  gpu_fp_params_.shrink_to_fit();
  gpu_vp_params_.clear();
  gpu_vp_params_.shrink_to_fit();
  SkyBoxFpParams.reset();
}

}  // namespace Glue
