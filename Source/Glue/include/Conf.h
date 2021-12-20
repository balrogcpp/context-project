// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Exception.h"
#include "Singleton.h"
#include <fstream>
#include <inih/INIReader.h>
#include <iostream>
#include <string>

namespace Glue {

class Conf : public Singleton<Conf> {
 public:
  explicit Conf(const std::string &FileName) { Reload(FileName); }

  virtual ~Conf() {}

  void Reload(const std::string &FileName) {
    if (FileName.empty()) return;
    std::ifstream ifs(FileName);
    if (!ifs.is_open()) {
      Throw(FileName + " does not exists");
    }

    Document.reset();
    Document = std::make_unique<INIReader>(FileName);
    if (Document->ParseError()) {
      Throw(FileName + std::to_string(Document->ParseError()));
    }
  }

  inline std::string Get(std::string name, std::string default_value = "") const { return Document->Get(DefaultSection, name, default_value); }

  inline std::string GetString(std::string name, std::string default_value = "") const { return Document->Get(DefaultSection, name, default_value); }

  inline long GetInt(std::string name, long default_value = 0) const { return Document->GetInteger(DefaultSection, name, default_value); }

  inline double GetReal(std::string name, double default_value = 0.0) const { return Document->GetReal(DefaultSection, name, default_value); }

  inline float GetFloat(std::string name, float default_value = 0.0f) const { return Document->GetFloat(DefaultSection, name, default_value); }

  inline bool GetBool(std::string name, bool default_value = false) const { return Document->GetBoolean(DefaultSection, name, default_value); }

 protected:
  std::unique_ptr<INIReader> Document;
  const std::string DefaultSection = "DEFAULT";
};

}  // namespace Glue
