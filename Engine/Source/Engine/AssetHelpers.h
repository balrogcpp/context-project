// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include "Platform.h"
#ifdef DESKTOP
#include <OgreResourceGroupManager.h>
#include <string>

namespace Glue {

/// Helper function to add location *WITH ALL SUBDIRECTORIES* to RG
/// @param Path path to add
/// @param GroupName Ogre resource group name
void AddLocation(const std::string &Path, const std::string &GroupName = Ogre::RGN_DEFAULT);

/// Helper function to add locations stored in resource file
/// @param Path path to add
/// @param GroupName Ogre resource group name
/// @param ResourceFile resource file
void AddResourceFile(const std::string &Path, const std::string &GroupName = Ogre::RGN_DEFAULT, const std::string &ResourceFile = "");

}  // namespace Glue
#endif // DESKTOP
