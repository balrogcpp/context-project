#pragma once
#include "ArHosekSkyModel.h"
#include <OgreVector.h>


// Useful shader functions
inline float Deg2Rad(float deg) { return deg * (1.0f / 180.0f) * Ogre::Math::PI; }
inline float Rad2Deg(float rad) { return rad * (1.0f / Ogre::Math::PI) * 180.0f; }
inline float Clamp(float val, float min, float max) { assert(max >= min); val = val < min ? min : val > max ? max : val; return val; }
inline static float AngleBetween(const Ogre::Vector3 &dir0, const Ogre::Vector3 &dir1) { return std::acos(std::max(dir0.dotProduct(dir1), 0.00001f)); }
inline float Mix(float x, float y, float s) { return x + (y - x) * s; }

// Hosek Sky Model forward declare
struct ArHosekSkyModelState;

class SkyModel {
 public:
  ArHosekSkyModelState *StateX = nullptr;
  ArHosekSkyModelState *StateY = nullptr;
  ArHosekSkyModelState *StateZ = nullptr;

  Ogre::Vector3 SunDir;
  Ogre::Vector3 SunRadiance;
  Ogre::Vector3 SunIrradiance;
  Ogre::Vector3 SunRenderColor;
  float SunSize = 0.0f;
  float Turbidity = 0.0f;
  Ogre::Vector3 Albedo;
  float Elevation = 0.0f;

  void Shutdown();
  ~SkyModel() { Shutdown(); };
  void SetupSky(const Ogre::Vector3 &_sunDir, float _sunSize, Ogre::Vector3 _sunRenderColor, const Ogre::Vector3 _groundAlbedo, float _turbidity);
  bool Initialized() const { return StateX != nullptr; }
  Ogre::Vector3 Sample(Ogre::Vector3 _sampleDir, bool _bEnableSun, Ogre::Vector3 _skyTint, Ogre::Vector3 _sunTint) const;
};
