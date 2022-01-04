// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Singleton.h"
#include <OgreLog.h>
#include <fstream>

namespace Glue {

#ifdef DESKTOP

class Log : public Ogre::LogListener, public Singleton<Log> {
 public:
  explicit Log(std::string LogFileName = "Runtime.log");
  virtual ~Log();

  void messageLogged(const std::string &message, Ogre::LogMessageLevel lml, bool maskDebug, const std::string &logName,
                     bool &skipThisMessage) override;

  void WriteLogToFile(bool enable);
  void WriteLogToConsole(bool enable);
  static void Message(const Ogre::String &message, Ogre::LogMessageLevel lml = Ogre::LML_NORMAL, bool maskDebug = false);

 protected:
  std::ofstream FileStream;
  std::string FileName = "Runtime.log";
  bool EnableWriteToFile = false;
  bool EnableWriteToConsole = false;
};

#endif // DESKTOP

}  // namespace Glue
