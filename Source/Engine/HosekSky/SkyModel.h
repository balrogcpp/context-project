#pragma once

#include <OgrePrerequisites.h>
#include <array>

std::array<Ogre::Vector3f, 10> getHosekParams(Ogre::Vector3f sunDir);
void applyHosekParams(Ogre::Vector3f sunDir, const Ogre::MaterialPtr& material, const std::string& uniform);
