#pragma once

#include <OgrePrerequisites.h>
#include <vector>

std::vector<float> getHosekParams(Ogre::Vector3f sunDir);
void applyHosekParams(Ogre::Vector3f sunDir, const std::string& material);
