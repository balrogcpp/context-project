//
// Created by fours on 02.06.2021.
//

#pragma once
#include <OgreLog.h>

#include <string>

namespace xio {
void LogMessage(const std::string& message, Ogre::LogMessageLevel lml = Ogre::LML_NORMAL, bool maskDebug = false);
}
