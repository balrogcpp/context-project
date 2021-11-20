// This source file is part of "glue project"
// Created by Andrew Vasiliev

#pragma once
#include <OgreLog.h>

namespace glue {

void LogMessage(const std::string& message, Ogre::LogMessageLevel lml = Ogre::LML_NORMAL, bool maskDebug = false);

}
