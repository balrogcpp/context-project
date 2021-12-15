// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Log.h"

using namespace std;

namespace Glue {

void LogMessage(const Ogre::String& message, Ogre::LogMessageLevel lml, bool maskDebug) {
  Ogre::LogManager::getSingleton().logMessage(message, lml, maskDebug);
}

}  // namespace Glue
