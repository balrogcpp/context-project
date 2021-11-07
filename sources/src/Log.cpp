//
// Created by fours on 02.06.2021.
//

#include "Log.h"

#include "pcheader.h"

using namespace std;

namespace glue {
void LogMessage(const std::string& message, Ogre::LogMessageLevel lml, bool maskDebug) {
  Ogre::LogManager::getSingleton().logMessage(message, lml, maskDebug);
}
}  // namespace glue
