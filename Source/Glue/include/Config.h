// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Singleton.h"
#include <string>

class INIReader;

namespace Glue {

class Config : public Singleton<Config> {
 public:
  explicit Config(const std::string &FileName = "Config.ini");
  virtual ~Config();

  void Reload(const std::string &FileName);

  std::string Get(std::string name, std::string default_value = "") const;
  std::string GetString(std::string name, std::string default_value = "") const;
  long GetInt(std::string name, long default_value = 0) const;
  double GetReal(std::string name, double default_value = 0.0) const;
  float GetFloat(std::string name, float default_value = 0.0f) const;
  bool GetBool(std::string name, bool default_value = false) const;

  const std::string &GetDefaultSection() const;
  void SetDefaultSection(const std::string &defaultSection);

 protected:
  std::unique_ptr<INIReader> Document;
  std::string ConfFileName;
  std::string DefaultSection = "DEFAULT";
};

}  // namespace Glue
