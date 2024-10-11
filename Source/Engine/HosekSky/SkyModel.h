#pragma once

#include <OgrePrerequisites.h>
#include <vector>

std::vector<Ogre::Vector3f> getHosekParams(Ogre::Vector3f sunDir);
void applyHosekParams(Ogre::Vector3f sunDir, const Ogre::MaterialPtr& material, const std::string& uniform);
