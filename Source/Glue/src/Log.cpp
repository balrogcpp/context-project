// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Log.h"
#include <iostream>
#include <fstream>

using namespace std;

namespace Glue {

Log::Log(std::string LogFileName) {
#ifdef DESKTOP
  auto *logger = new Ogre::LogManager();

#ifdef DEBUG
  logger->createLog(LogFileName, true, true, true);
  Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_BOREME);
#else
  logger->createLog(LogFileName, false, false, true);
  Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_NORMAL);
#endif

  Ogre::LogManager::getSingleton().getDefaultLog()->addListener(this);

#endif
}

Log::~Log() {
#ifdef DESKTOP
  Ogre::LogManager::getSingleton().getDefaultLog()->removeListener(this);
#endif
}

void Log::WriteLogToFile() {
  if (!Verbose) {
    return;
  }

  ofstream f;
  f.open(LogFileName);

  if (f.is_open()) {
    f << LogBuffer;
  }
}
void Log::PrintLogToConsole() { cout << LogBuffer << flush; }

void Log::messageLogged(const string &message, Ogre::LogMessageLevel lml, bool maskDebug, const string &logName, bool &skipThisMessage) {
  LogBuffer.append(message);
  LogBuffer.append("\n");

#ifdef DEBUG
  if (Verbose) cout << message << '\n';
#endif
}

void Log::Message(const Ogre::String &message, Ogre::LogMessageLevel lml, bool maskDebug) {
  Ogre::LogManager::getSingleton().logMessage(message, lml, maskDebug);
}

}  // namespace Glue
