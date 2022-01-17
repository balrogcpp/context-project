// This source file is part of "glue project". Created by Andrey Vasiliev

#include "PCHeader.h"
#include "Log.h"
#include <fstream>
#include <iostream>
#include <thread>

using namespace std;

namespace Glue {

#ifdef DESKTOP

Log::Log(std::string LogFileName) {
  FileName = LogFileName;
  auto *logger = new Ogre::LogManager();
  FileStream.open(FileName);
  if (!FileStream.is_open()) EnableWriteToFile = false;

  logger->createLog(FileName, false, false, true);
  Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_BOREME);
  Ogre::LogManager::getSingleton().getDefaultLog()->addListener(this);
}

Log::~Log() { Ogre::LogManager::getSingleton().getDefaultLog()->removeListener(this); }

void Log::WriteLogToConsole(bool enable) { EnableWriteToConsole = enable; }

void Log::WriteLogToFile(bool enable) { EnableWriteToFile = enable; }

void Log::messageLogged(const string &message, Ogre::LogMessageLevel lml, bool maskDebug, const string &logName, bool &skipThisMessage) {
  if (EnableWriteToFile && FileStream.is_open()) FileStream << message << '\n';
  if (EnableWriteToConsole) cout << message << '\n';
}

void Log::Message(const Ogre::String &message, Ogre::LogMessageLevel lml, bool maskDebug) {
  static auto &OgreLogManager = Ogre::LogManager::getSingleton();
  OgreLogManager.logMessage(message, lml, maskDebug);
}

#endif  // DESKTOP

}  // namespace Glue
