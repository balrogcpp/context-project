/// created by Andrey Vasiliev

#include "pch.h"
#include "SkyManager.h"
#include "ArHosekSkyModel.h"
#include "Platform.h"

namespace gge {
SkyManager::SkyManager() : needsUpdate(false) {}
SkyManager::~SkyManager() {}

Ogre::Vector3 SkyManager::GetSunPosition() {
  if (sceneManager->hasLight("Sun")) {
    auto *SunPtr = sceneManager->getLight("Sun");
    return SunPtr ? -SunPtr->getDerivedDirection().normalisedCopy() : Ogre::Vector3::ZERO;
  } else {
    return Ogre::Vector3::ZERO;
  }
}

namespace {
// Useful shader functions
inline float Deg2Rad(float deg) { return deg * (1.0f / 180.0f) * Ogre::Math::PI; }
inline float Rad2Deg(float rad) { return rad * (1.0f / Ogre::Math::PI) * 180.0f; }
inline float Clamp(float val, float min, float max) {
  assert(max >= min);
  val = val < min ? min : val > max ? max : val;
  return val;
}
inline static float AngleBetween(const Ogre::Vector3 &dir0, const Ogre::Vector3 &dir1) {
  return std::acos(std::max(dir0.dotProduct(dir1), 0.00001f));
}
inline float Mix(float x, float y, float s) { return x + (y - x) * s; }
}

void SkyManager::SetUpSky() {
  ArHosekSkyModelState *states[3];
  Ogre::Vector3f sunDir = GetSunPosition();
  Ogre::Vector3f groundAlbedo = Ogre::Vector3f(0.1f);
  sunDir.y = Clamp(sunDir.y, 0.0, 1.0);
  sunDir.normalise();
  Ogre::Real turbidity = 4.0;
  groundAlbedo = Ogre::Vector3f(Clamp(groundAlbedo.x, 0.0, 1.0), Clamp(groundAlbedo.y, 0.0, 1.0), Clamp(groundAlbedo.z, 0.0, 1.0));

  float thetaS = AngleBetween(sunDir, Ogre::Vector3f(0, 1, 0));
  float elevation = Ogre::Math::HALF_PI - thetaS;
  states[0] = arhosek_rgb_skymodelstate_alloc_init(turbidity, groundAlbedo.x, elevation);
  states[1] = arhosek_rgb_skymodelstate_alloc_init(turbidity, groundAlbedo.y, elevation);
  states[2] = arhosek_rgb_skymodelstate_alloc_init(turbidity, groundAlbedo.z, elevation);

  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 3; j++) {
      hosekParams[i][j] = states[j]->configs[j][i];
      hosekParams[9] = Ogre::Vector3(states[0]->radiances[0], states[1]->radiances[1], states[2]->radiances[2]);
    }
  }

  auto skyMaterial = Ogre::MaterialManager::getSingleton().getByName("SkyBox");
  auto FpParams = skyMaterial->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
  if (FpParams) fpParams = FpParams;
  FpParams->setIgnoreMissingParams(true);
  needsUpdate = true;
  OnUpdate(0.0);
}

void SkyManager::OnSetUp() {
  ogreRoot = Ogre::Root::getSingletonPtr();
  ASSERTION(ogreRoot, "[SceneManager] ogreRoot is not initialised");
  sceneManager = ogreRoot->getSceneManager("Default");
  ASSERTION(sceneManager, "[SceneManager] sceneManager is not initialised");
  ASSERTION(sceneManager->hasCamera("Camera"), "[SceneManager] camera is not initialised");
  camera = sceneManager->getCamera("Camera");
}

void SkyManager::OnUpdate(float time) {
  if (needsUpdate && fpParams) {
    Ogre::Real hosekParamsArray[10 * 3];
    for (int i = 0; i < 10; i++) {
      hosekParamsArray[3 * i] = hosekParams[i].x;
      hosekParamsArray[3 * i + 1] = hosekParams[i].y;
      hosekParamsArray[3 * i + 2] = hosekParams[i].z;
    }
    fpParams->setNamedConstant("Params", hosekParamsArray, 10 * 3);
    needsUpdate = false;
  }
}

void SkyManager::OnClean() {}
}  // namespace gge
