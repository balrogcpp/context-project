// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Platform.h"
#include <OgreResourceGroupManager.h>
#include <string>

namespace Glue {

#ifdef DESKTOP

void AddLocation(const std::string &Path, const std::string &GroupName = Ogre::RGN_DEFAULT, const std::string &ResourceFile = "");

#endif

}  // namespace Glue
