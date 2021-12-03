// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include <OgreLog.h>

namespace Glue {

void LogMessage(const std::string& message, Ogre::LogMessageLevel lml = Ogre::LML_NORMAL, bool maskDebug = false);

}
