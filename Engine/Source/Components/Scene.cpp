// This source file is part of Glue Engine. Created by Andrey Vasiliev

#include "PCHeader.h"
#include "Components/Scene.h"
#include "Hosek/ArHosekSkyModel.h"
#include "Caelum/Caelum.h"
#include "CameraMan.h"
#include "Engine.h"
#include "PagedGeometry/PagedGeometryAll.h"
#include "ShaderHelpers.h"
#include "SinbadCharacterController.h"
#include "SkyModel.h"
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

CameraMan &Scene::GetCameraMan() const { return *CameraManPtr; }

Ogre::Vector3 Scene::GetSunPosition() {
  auto *SunPtr = OgreScene->getLight("Sun");
  if (SunPtr)
    return -Vector3(SunPtr->getDerivedDirection().normalisedCopy());
  else
    return Vector3();
}

float Scene::GetHeight(float x, float z) {
  if (OgreTerrainList)
    return OgreTerrainList->getHeightAtWorldPosition(x, 1000, z);
  else
    return 0.0f;
}

void Scene::AddEntity(Ogre::Entity *EntityPtr) { AddEntityMaterial(EntityPtr); }

void Scene::AddMaterial(Ogre::MaterialPtr material) {
  const auto *Pass = material->getTechnique(0)->getPass(0);
  if (Pass->hasVertexProgram()) GpuVpParams.push_back(Pass->getVertexProgramParameters());
  if (Pass->hasFragmentProgram()) GpuFpParams.push_back(Pass->getFragmentProgramParameters());
}

void Scene::AddMaterial(const std::string &MaterialName) {
  auto MaterialPtr = MaterialManager::getSingleton().getByName(MaterialName);
  if (MaterialPtr) AddMaterial(MaterialPtr);
}

void Scene::AddCamera(Camera *OgreCameraPtr) {
  if (CameraManPtr->GetStyle() == CameraMan::FPS) {
    auto *Actor = OgreScene->createEntity("Actor", "Icosphere.mesh");
    auto *ParentNode = OgreCameraPtr->getParentSceneNode();

    Actor->setCastShadows(false);
    Actor->setVisible(false);
    ParentNode->attachObject(Actor);

    InputSequencer::GetInstance().RegObserver(CameraManPtr.get());

    btVector3 inertia(0, 0, 0);
    btRigidBody *RigidBody = nullptr;
    auto *entShape = BtOgre::createCapsuleCollider(Actor);
    // GetAudio().AddListener(ParentNode);
    float mass = 100.0;
    entShape->calculateLocalInertia(mass, inertia);
    auto *bodyState = new BtOgre::RigidBodyState(ParentNode);
    RigidBody = new btRigidBody(mass, bodyState, entShape, inertia);
    RigidBody->setAngularFactor(0);
    RigidBody->activate(true);
    RigidBody->forceActivationState(DISABLE_DEACTIVATION);
    RigidBody->setActivationState(DISABLE_DEACTIVATION);
    RigidBody->setFriction(1.0);
    RigidBody->setUserIndex(1);
    GetPhysics().AddRigidBody(RigidBody);
    CameraManPtr->SetRigidBody(RigidBody);
    CameraManPtr->AttachCamera(OgreCameraPtr);
  }
}

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
  auto colorspace = sRGB;
  float sunSize = 0.27f;
  float turbidity = 4.0f;
  auto groundAlbedo = Vector3(1.0);
  auto sunColor = Vector3(20000);
  auto sunDir = GetSunPosition();

  SkyModel sky;
  sky.SetupSky(sunDir, sunSize, sunColor, groundAlbedo, turbidity, colorspace);

  const ArHosekSkyModelState *States[3] = {sky.StateX, sky.StateY, sky.StateZ};

  for (int i = 0; i < 9; i++)
    for (int j = 0; j < 3; j++) HosekParams[i][j] = States[j]->configs[j][i];
  HosekParams[9] = Vector3(sky.StateX->radiances[0], sky.StateY->radiances[1], sky.StateZ->radiances[2]);

  auto SkyMaterial = MaterialManager::getSingleton().getByName("SkyBox");
  if (!SkyMaterial) return;

  auto FpParams = SkyMaterial->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
  if (FpParams) SkyBoxFpParams = FpParams;

  FpParams->setIgnoreMissingParams(true);
  for (int i = 0; i < 10; i++) FpParams->setNamedConstant(HosikParamList[i], HosekParams[i]);
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

  for (auto &it : GpuVpParams) it->setNamedConstant("uWorldViewProjPrev", MVPprev);

  if (SkyNeedsUpdate && SkyBoxFpParams)
    for (int i = 0; i < 10; i++) SkyBoxFpParams->setNamedConstant(HosikParamList[i], HosekParams[i]);
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
  if (CameraManPtr) CameraManPtr->SetStyle(CameraMan::ControlStyle::MANUAL);
  ResourceGroupManager::getSingleton().unloadResourceGroup(GroupName);
  GpuFpParams.clear();
  GpuFpParams.shrink_to_fit();
  GpuVpParams.clear();
  GpuVpParams.shrink_to_fit();
  SkyBoxFpParams.reset();
}

}  // namespace Glue
