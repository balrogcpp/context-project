// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "NoCopy.h"
#include <map>
#include <string>

namespace glue {
class DesktopIcon : public NoCopy {
 public:
  DesktopIcon();
  virtual ~DesktopIcon();

  void SetUp();
  void Save(const std::string &icon_name);

 protected:
  std::string run_dir_;
  std::string exec_;
  std::string icon_;
  std::string version_;
  std::string name_;
  std::map<std::string, std::string> properties_;
  std::string output_;
  const std::string skeleton_;
};

}  // namespace glue
