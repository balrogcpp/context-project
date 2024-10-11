#include "SkyModel.h"
#include "ArHosekSkyModel.h"
#include "OgreTechnique.h"

namespace {
// Useful shader functions
inline float Deg2Rad(float deg) { return deg * (1.0f / 180.0f) * Ogre::Math::PI; }
inline float Rad2Deg(float rad) { return rad * (1.0f / Ogre::Math::PI) * 180.0f; }
inline float Clamp(float val, float min, float max) {
  assert(max >= min);
  val = val < min ? min : val > max ? max : val;
  return val;
}
inline float AngleBetween(const Ogre::Vector3 &dir0, const Ogre::Vector3 &dir1) { return std::acos(std::max(dir0.dotProduct(dir1), 0.00001f)); }
inline float Mix(float x, float y, float s) { return x + (y - x) * s; }
}  // namespace

std::vector<Ogre::Vector3f> getHosekParams(Ogre::Vector3f sunDir) {
  ArHosekSkyModelState *states[3];
  std::vector<Ogre::Vector3f> hosekParams(10);

  Ogre::Vector3f albedo = Ogre::Vector3f(1.0f);
  sunDir.y = Clamp(sunDir.y, 0.0, 1.0);
  sunDir.normalise();
  Ogre::Real turbidity = 3.0;
  albedo = Ogre::Vector3f(Clamp(albedo.x, 0.0, 1.0), Clamp(albedo.y, 0.0, 1.0), Clamp(albedo.z, 0.0, 1.0));

  float thetaS = AngleBetween(sunDir, Ogre::Vector3f(0, 1, 0));
  float elevation = Ogre::Math::HALF_PI - thetaS;
  for (int i = 0; i < 3; i++) states[i] = arhosek_rgb_skymodelstate_alloc_init(turbidity, albedo[i], elevation);

  for (int i = 0; i < 9; i++)
    for (int j = 0; j < 3; j++) hosekParams[i][j] = states[j]->configs[j][i];

  hosekParams[9] = Ogre::Vector3(states[0]->radiances[0], states[1]->radiances[1], states[2]->radiances[2]);

  return hosekParams;
}

void applyHosekParams(Ogre::Vector3f sunDir, const Ogre::MaterialPtr &material, const std::string &uniform) {
  static Ogre::Vector3f sunDirOld;

  if (sunDirOld != sunDir) {
    std::vector<Ogre::Vector3f> hosekParams = getHosekParams(sunDir);
    std::array<float, 30> hosekParamsArray{};
    for (int i = 0; i < 10; i++)
      for (int j = 0; j < 3; j++) hosekParamsArray[3 * i + j] = hosekParams[i][j];

    auto fp = material->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
    fp->setIgnoreMissingParams(true);
    fp->setNamedConstant(uniform, hosekParamsArray.data(), hosekParamsArray.size());
  }

  sunDirOld = sunDir;
}
