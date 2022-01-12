// This source file is part of "glue project". Created by Andrey Vasiliev

#pragma once

#include "Platform.h"

#ifdef DESKTOP

#include "NoCopy.h"
#include <map>
#include <string>

namespace Glue {
class DesktopIcon : public NoCopy {
 public:
  DesktopIcon();
  virtual ~DesktopIcon();

  void SaveToFile(const std::string &icon_name);

 protected:
  std::string root_directory;
  std::string executable;
  std::string icon;
  std::string version;
  std::string icon_name;
  std::map<std::string, std::string> properties;
  std::string output;
  std::string skeleton;
};

}  // namespace Glue

#endif
