// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include <OgreMaterial.h>
#include <vector>

namespace Ogre {
class Camera;
}

namespace Glue {

static void FixTransparentShadowCaster(const Ogre::MaterialPtr &material);
static void FixTransparentShadowCaster(const std::string &material);

}  // namespace Glue
