// This source file is part of context-project
// Created by Andrew Vasiliev

#pragma once
#include <map>
#include <string>

#include "NoCopy.h"

namespace xio {
class DesktopIcon : public NoCopy {
 public:
  DesktopIcon();
  virtual ~DesktopIcon();

  void Init();
  void Save(const std::string &icon_name);

 private:
  std::string run_dir_;
  std::string exec_;
  std::string icon_;
  std::string version_;
  std::string name_;
  std::map<std::string, std::string> properties;
  std::string output_;
  const std::string skeleton_;
};

}  // namespace xio
