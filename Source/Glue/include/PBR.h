// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include <OgreMaterial.h>
#include <vector>

namespace Ogre {
class Camera;
}

namespace Glue {

void FixTransparentShadowCaster(const Ogre::MaterialPtr &material);
void FixTransparentShadowCaster(const std::string &material);

}  // namespace Glue
