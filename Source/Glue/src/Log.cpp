// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Log.h"
#include <fstream>
#include <iostream>
#include <thread>

using namespace std;

namespace Glue {

Log::Log(std::string LogFileName) {
#ifdef DESKTOP
  auto *logger = new Ogre::LogManager();
  f.open(LogFileName);
  if (!f.is_open()) EnableWriteToFile = false;

  logger->createLog(LogFileName, false, false, true);
  Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_BOREME);

  Ogre::LogManager::getSingleton().getDefaultLog()->addListener(this);

#endif
}

Log::~Log() {
#ifdef DESKTOP
  Ogre::LogManager::getSingleton().getDefaultLog()->removeListener(this);
#endif
}

void Log::WriteLogToConsole(bool enable) { EnableWriteToConsole = enable; }

void Log::WriteLogToFile(bool enable) { EnableWriteToFile = enable; }

void Log::messageLogged(const string &message, Ogre::LogMessageLevel lml, bool maskDebug, const string &logName, bool &skipThisMessage) {
#ifdef DESKTOP
  if (EnableWriteToFile && f.is_open()) f << message << '\n';
  if (EnableWriteToConsole) cout << message << '\n';
#endif
}

void Log::Message(const Ogre::String &message, Ogre::LogMessageLevel lml, bool maskDebug) {
#ifdef DESKTOP
  static auto &OgreLogManager = Ogre::LogManager::getSingleton();
  OgreLogManager.logMessage(message, lml, maskDebug);
#endif
}

}  // namespace Glue
