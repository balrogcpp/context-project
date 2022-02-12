#ifndef SKYMODEL_H
#define SKYMODEL_H

#include <OgreVector3.h>

enum ColorSpace { XYZ, sRGB, ACEScg, ACES2065_1 };

// Hosek Sky Model forward declare
struct ArHosekSkyModelState;

class SkyModel {
 public:
  ArHosekSkyModelState *StateX = nullptr;
  ArHosekSkyModelState *StateY = nullptr;
  ArHosekSkyModelState *StateZ = nullptr;

  Ogre::Vector3f SunDir;
  Ogre::Vector3f SunRadiance;
  Ogre::Vector3f SunIrradiance;
  Ogre::Vector3f SunRenderColor;
  float SunSize = 0.0f;
  float Turbidity = 0.0f;
  Ogre::Vector3f Albedo;
  float Elevation = 0.0f;
  ColorSpace Colorspace;

  void Shutdown();
  ~SkyModel() { Shutdown(); };
  void SetupSky(const Ogre::Vector3f &_sunDir, float _sunSize, Ogre::Vector3f _sunRenderColor, const Ogre::Vector3f _groundAlbedo, float _turbidity,
                ColorSpace _colorspace);
  bool Initialized() const { return StateX != nullptr; }
  Ogre::Vector3f Sample(Ogre::Vector3f _sampleDir, bool _bEnableSun, Ogre::Vector3f _skyTint, Ogre::Vector3f _sunTint) const;
};

#endif
