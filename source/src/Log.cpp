// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Log.h"

using namespace std;

namespace glue {
void LogMessage(const std::string& message, Ogre::LogMessageLevel lml, bool maskDebug) {
  Ogre::LogManager::getSingleton().logMessage(message, lml, maskDebug);
}
}  // namespace glue
