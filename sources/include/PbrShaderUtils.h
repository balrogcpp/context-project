// This source file is part of "glue project"
// Created by Andrew Vasiliev

#pragma once
#include <OgreMaterial.h>
#include <vector>

#define OGRE_MAX_SIMULTANEOUS_SHADOW_TEXTURES 4

namespace Ogre {
class Camera;
}

namespace glue {

class Pbr {
 public:
  static void UpdatePbrShadowCaster(const Ogre::MaterialPtr &material);
  static void UpdatePbrParams(const Ogre::MaterialPtr &material);
  static void UpdatePbrShadowReceiver(const Ogre::MaterialPtr &material);
  static void UpdatePbrIbl(const Ogre::MaterialPtr &material, bool active = false);
  static void UpdatePbrParams(const std::string &material);
  static void UpdatePbrIbl(const std::string &material, bool realtime);
  static void UpdatePbrShadowReceiver(const std::string &material);
  static void UpdatePbrShadowCaster(const std::string &material);
  static void Cleanup();
  static void Update(float time);
};

}  // namespace glue
