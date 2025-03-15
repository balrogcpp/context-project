#include "SkyModel.h"
#include "ArHosekSkyModel.h"
#include <OgreTechnique.h>
#include <cmath>

using namespace Ogre;

namespace {
    // Scale factor used for storing physical light units in fp16 floats (equal to 2^-10)
    static const float FP16Scale = 0.0009765625f;
    static const float FP16Max = 65000.0f;

    // Useful shader functions
    inline float Deg2Rad(float deg) { return deg * (1.0f / 180.0f) * Ogre::Math::PI; }
    inline float Rad2Deg(float rad) { return rad * (1.0f / Ogre::Math::PI) * 180.0f; }
    inline float Clamp(float val, float min, float max) { return val < min ? min : val > max ? max : val; }
    inline static float AngleBetween(const Ogre::Vector3f &dir0, const Ogre::Vector3f &dir1) {
      return std::acos(std::max(Clamp(dir0.dotProduct(dir1), -1.0, 1.0), 0.00001f));
    }
    inline float Mix(float x, float y, float s) { return x + (y - x) * s; }
}  // namespace

Ogre::Vector3f sampleSky(Ogre::Vector3f sampleDir, Ogre::Vector3f sunDir) {
  float gamma = AngleBetween(sampleDir, sunDir);
  float theta = AngleBetween(sampleDir, Ogre::Vector3f(0, 1, 0));

  Vector3f radiance;
  ArHosekSkyModelState *states[3];

  Ogre::Vector3f albedo = Ogre::Vector3f(1.0f);
  float turbidity = 3.0;
  albedo = Ogre::Vector3f(Clamp(albedo.x, 0.0, 1.0), Clamp(albedo.y, 0.0, 1.0), Clamp(albedo.z, 0.0, 1.0));

  float thetaS = AngleBetween(sunDir, Ogre::Vector3f(0, 1, 0));
  float elevation = Ogre::Math::HALF_PI - thetaS;

  for (int i = 0; i < 3; i++) states[i] = arhosek_rgb_skymodelstate_alloc_init(turbidity, albedo[i], elevation);
  for (int i = 0; i < 3; i++) radiance[i] = arhosek_tristim_skymodel_radiance(states[i], theta, gamma, i);
  for (int i = 0; i < 3; i++) arhosekskymodelstate_free(states[i]);

  for (int i = 0; i < 3; i++) radiance[i] = exp(-0.04 * radiance[i]);
  radiance = 2.0 / (1.0 + radiance) - 1.0;

  return radiance;
}

std::array<Ogre::Vector3f, 10> getHosekParams(Ogre::Vector3f sunDir) {
  ArHosekSkyModelState *states[3];
  std::array<Ogre::Vector3f, 10> hosekParams;

  Ogre::Vector3f albedo = Ogre::Vector3f(1.0f);
  float turbidity = 3.0;
  albedo = Ogre::Vector3f(Clamp(albedo.x, 0.0, 1.0), Clamp(albedo.y, 0.0, 1.0), Clamp(albedo.z, 0.0, 1.0));

  float thetaS = AngleBetween(sunDir, Ogre::Vector3f(0, 1, 0));
  float elevation = Ogre::Math::HALF_PI - thetaS;

  for (int i = 0; i < 3; i++) states[i] = arhosek_rgb_skymodelstate_alloc_init(turbidity, albedo[i], elevation);

  for (int i = 0; i < 9; i++)
    for (int j = 0; j < 3; j++) hosekParams[i][j] = states[j]->configs[j][i];

  hosekParams[9] = Ogre::Vector3(states[0]->radiances[0], states[1]->radiances[1], states[2]->radiances[2]);
  for (int i = 0; i < 3; i++) arhosekskymodelstate_free(states[i]);

  return hosekParams;
}

void applyHosekParams(Ogre::Vector3f sunDir, const Ogre::MaterialPtr &material, const std::string &uniform) {
  static Ogre::Vector3f sunDirOld;
  sunDir.normalise();

  if (sunDirOld != sunDir) {
    std::array<Ogre::Vector3f, 10> hosekParams = getHosekParams(sunDir);
    std::array<Ogre::Vector3f, 3> hosekValues;
    hosekValues[0] = sampleSky(sunDir, sunDir);
    hosekValues[1] = sampleSky(Vector3f(1, 1, 0).normalisedCopy(), sunDir);
    hosekValues[2] = sampleSky(Vector3f(1, -1, 0).normalisedCopy(), sunDir);

    std::array<float, 30> hosekParamsArray{};
    std::array<float, 9> hosekValuesArray{};

    for (int i = 0; i < 10; i++)
      for (int j = 0; j < 3; j++) hosekParamsArray[3 * i + j] = hosekParams[i][j];

    for (int i = 0; i < 3; i++)
      for (int j = 0; j < 3; j++) hosekValuesArray[3 * i + j] = hosekValues[i][j];

    auto fp = material->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
    fp->setIgnoreMissingParams(true);
    fp->setNamedConstant(uniform, hosekParamsArray.data(), hosekParamsArray.size());
    fp->setNamedConstant("HosekSamples", hosekValuesArray.data(), hosekValuesArray.size());
  }

  sunDirOld = sunDir;
}
