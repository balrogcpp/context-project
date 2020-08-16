/*
MIT License

Copyright (c) 2020 Andrey Vasiliev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "ManagerCommon.hpp"

#include "ContextConfigStructures.hpp"

#include "rapidjson/document.h"

#include <string>

namespace Context {

class ConfigManager : public ManagerCommon {
 public:

  void Setup() final;

  void Reset() final;

 public:
  std::string GetConfigFileName() const;
  void SetConfigFileName(const std::string &config_file_name);
  const rapidjson::Document &GetDocument() const;

  bool HasMember(const std::string &parameter);

  int GetInt(const std::string &parameter);
  bool GetBool(const std::string &parameter);
  std::string GetString(const std::string &parameter);
  float GetFloat(const std::string &parameter);
  double GetDouble(const std::string &parameter);

  static bool Assign(bool &value, const std::string &parameter);
  static bool Assign(int &value, const std::string &parameter);
  static bool Assign(std::string &value, const std::string &parameter);
  static bool Assign(float &value, const std::string &parameter);
  static bool Assign(double &value, const std::string &parameter);

  bool Add(const std::string &name, bool value);
  bool Add(const std::string &name, int value);
  bool Add(const std::string &name, float value);
  bool Add(const std::string &name, double value);
  bool Add(const std::string &name, const std::string &value);
  bool Add(const std::string &name, const char *value);
  bool Add(const std::string &name, char *value);

 private:
  std::string file_name_ = "config.json";

  rapidjson::Document document_;

 private:
  static ConfigManager ConfigManagerSingleton;

 public:
  static ConfigManager &GetSingleton() {
    return ConfigManagerSingleton;
  }

  static ConfigManager *GetSingletonPtr() {
    return &ConfigManagerSingleton;
  }
};

}
