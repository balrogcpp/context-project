// This source file is part of Glue Engine. Created by Andrey Vasiliev

#include "PCHeader.h"
#include "Log.h"
#include <fstream>
#include <iostream>
#include <thread>

using namespace std;

namespace Glue {

Log::Log(std::string LogFileName) {
#ifdef DESKTOP
  FileName = LogFileName;
  auto *logger = new Ogre::LogManager();
  FileStream.open(FileName);
  if (!FileStream.is_open()) EnableWriteToFile = false;

  logger->createLog(FileName, false, false, true);
  Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_BOREME);
  Ogre::LogManager::getSingleton().getDefaultLog()->addListener(this);
#endif
}

Log::~Log() {
#ifdef DESKTOP
  Ogre::LogManager::getSingleton().getDefaultLog()->removeListener(this);
#endif
}

void Log::WriteLogToConsole(bool enable) {
#ifdef DESKTOP
  EnableWriteToConsole = enable;
#endif
}

void Log::WriteLogToFile(bool enable){
#ifdef DESKTOP
  EnableWriteToFile = enable;
#endif
}

void Log::messageLogged(const string &message, Ogre::LogMessageLevel lml, bool maskDebug, const string &logName, bool &skipThisMessage) {
#ifdef DESKTOP
  if (EnableWriteToFile && FileStream.is_open()) FileStream << message << '\n';
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
