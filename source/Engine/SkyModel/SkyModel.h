#ifndef SKYMODEL_H
#define SKYMODEL_H

#include "Astronomy.h"
#include <OgreVector3.h>
#include <cmath>

enum class ColorSpace { XYZ, sRGB, ACEScg, ACES2065_1 };

// Scale factor used for storing physical light units in fp16 floats (equal to 2^-10)
constexpr float FP16Scale = 0.0009765625f;
constexpr float FP16Max = 65000.0f;

// Useful shader functions
inline float Deg2Rad(float deg) { return deg * (1.0f / 180.0f) * M_PI; }
inline float Rad2Deg(float rad) { return rad * (1.0f / M_PI) * 180.0f; }
inline float Clamp(float val, float min, float max) { assert(max >= min); val = val < min ? min : val > max ? max : val; return val; }
inline static float AngleBetween(const Ogre::Vector3 &dir0, const Ogre::Vector3 &dir1) { return std::acos(std::max(dir0.dotProduct(dir1), 0.00001f)); }
inline float Mix(float x, float y, float s) { return x + (y - x) * s; }

inline Ogre::Vector3 XYZ_to_ACES2065_1(Ogre::Vector3 color)
{
  Ogre::Vector3 out = Ogre::Vector3(0);
  out.x = color.x * 1.0498110175f + color.y * 0.0000000000f + color.z * -0.0000974845f;
  out.y = color.x * -0.4959030231f + color.y * 1.3733130458f + color.z * 0.0982400361f;
  out.z = color.x * 0.0000000000f + color.y * 0.0000000000f + color.z *  0.9912520182f;

  return out;
}

inline Ogre::Vector3 ACES2065_1_to_ACEScg(Ogre::Vector3 color)
{
  Ogre::Vector3 out = Ogre::Vector3(0);
  out.x = color.x * 1.4514393161f + color.y * -0.2365107469f + color.z * -0.2149285693f;
  out.y = color.x * -0.0765537733f + color.y * 1.1762296998f + color.z * -0.0996759265f;
  out.z = color.x * 0.0083161484f + color.y * -0.0060324498f + color.z *  0.9977163014f;

  return out;
}

inline Ogre::Vector3 ACES2065_1_to_sRGB(Ogre::Vector3 color)
{
  Ogre::Vector3 out = Ogre::Vector3(0);
  out.x = color.x * 2.5216494298f + color.y * -1.1368885542f + color.z * -0.3849175932f;
  out.y = color.x * -0.2752135512f + color.y * 1.3697051510f + color.z * -0.0943924508f;
  out.z = color.x * -0.0159250101f + color.y * -0.1478063681f + color.z * 1.1638058159f;

  return out;
}

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
  ColorSpace Colorspace;

  void Shutdown();
  ~SkyModel() { Shutdown(); };
  void SetupSky(const Ogre::Vector3 &_sunDir, float _sunSize, Ogre::Vector3 _sunRenderColor, const Ogre::Vector3 _groundAlbedo, float _turbidity,
                ColorSpace _colorspace);
  bool Initialized() const { return StateX != nullptr; }
  Ogre::Vector3 Sample(Ogre::Vector3 _sampleDir, bool _bEnableSun, Ogre::Vector3 _skyTint, Ogre::Vector3 _sunTint) const;
};

#endif
