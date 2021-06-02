//
// Created by fours on 02.06.2021.
//

#include "Log.h"

#include "pcheader.h"

using namespace std;

namespace xio {
void LogMessage(const std::string& message, Ogre::LogMessageLevel lml,
                bool maskDebug) {
  Ogre::LogManager::getSingleton().logMessage(message, lml, maskDebug);
}
}  // namespace xio