// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "LazySingleton.h"
#include <OgreLog.h>

namespace Glue {

class Log : public Ogre::LogListener, public Singleton<Log> {
 public:
  explicit Log(std::string LogFileName);
  virtual ~Log();

  void messageLogged(const std::string &message, Ogre::LogMessageLevel lml, bool maskDebug, const std::string &logName,
                     bool &skipThisMessage) override;

  void WriteLogToFile();
  void PrintLogToConsole();
  static void Message(const Ogre::String &message, Ogre::LogMessageLevel lml = Ogre::LML_NORMAL, bool maskDebug = false);

 protected:
  std::string LogBuffer;
  std::string LogFileName = "Launch.log";
  bool Verbose = false;
};

}  // namespace Glue
