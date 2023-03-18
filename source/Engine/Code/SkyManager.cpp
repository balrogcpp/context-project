/// created by Andrey Vasiliev

#include "pch.h"
#include "SkyManager.h"
#include "Platform.h"
#include <SkyModel/SkyModel.h>

namespace gge {
SkyManager::SkyManager() : needsUpdate(false), hosekParamList({"A", "B", "C", "D", "E", "F", "G", "H", "I", "Z"}) {}
SkyManager::~SkyManager() {}

Ogre::Vector3 SkyManager::GetSunPosition() {
  if (ogreSceneManager->hasLight("Sun")) {
    auto *SunPtr = ogreSceneManager->getLight("Sun");
    return SunPtr ? -SunPtr->getDerivedDirection().normalisedCopy() : Ogre::Vector3::ZERO;
  } else {
    return Ogre::Vector3::ZERO;
  }
}

void SkyManager::SetUpSky() {
  auto colorspace = ColorSpace::sRGB;
  float sunSize = 0.27f;
  float turbidity = 4.0f;
  auto groundAlbedo = Ogre::Vector3(1.0);
  auto sunColor = Ogre::Vector3(10000);
  auto sunDir = GetSunPosition();

  SkyModel skyModel;
  skyModel.SetupSky(sunDir, sunSize, sunColor, groundAlbedo, turbidity, colorspace);

  const ArHosekSkyModelState *States[3] = {skyModel.StateX, skyModel.StateY, skyModel.StateZ};

  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 3; j++) {
      hosekParams[i][j] = States[j]->configs[j][i];
      hosekParams[9] = Ogre::Vector3(skyModel.StateX->radiances[0], skyModel.StateY->radiances[1], skyModel.StateZ->radiances[2]);
    }
  }

  auto skyMaterial = Ogre::MaterialManager::getSingleton().getByName("SkyBox");
  auto FpParams = skyMaterial->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
  if (FpParams) fpParams = FpParams;

  FpParams->setIgnoreMissingParams(true);
  for (int i = 0; i < hosekParamList.size(); i++) {
    FpParams->setNamedConstant(hosekParamList[i], hosekParams[i]);
  }
}

void SkyManager::OnSetUp() {
  ogreRoot = Ogre::Root::getSingletonPtr();
  ASSERTION(ogreRoot, "[SceneManager] ogreRoot is not initialised");
  ogreSceneManager = ogreRoot->getSceneManager("Default");
  ASSERTION(ogreSceneManager, "[SceneManager] ogreSceneManager is not initialised");
  ASSERTION(ogreSceneManager->hasCamera("Default"), "[SceneManager] ogreCamera is not initialised");
  ogreCamera = ogreSceneManager->getCamera("Default");
}

void SkyManager::OnUpdate(float time) {
  if (needsUpdate && fpParams) {
    for (int i = 0; i < hosekParamList.size(); i++) {
      fpParams->setNamedConstant(hosekParamList[i], hosekParams[i]);
    }

    needsUpdate = false;
  }
}

void SkyManager::OnClean() {}
}  // namespace gge
