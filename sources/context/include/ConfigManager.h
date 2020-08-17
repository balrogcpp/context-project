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

#include "Manager.h"

#include "ContextConfigStructures.h"

#include "rapidjson/document.h"

#include <string>

namespace Context {

class ConfigManager {
 public:
  static ConfigManager &GetSingleton() {
    static ConfigManager ConfigManagerSingleton;
    return ConfigManagerSingleton;
  }

  void Setup();
  void Reset() {};

  //----------------------------------------------------------------------------------------------------------------------
  std::string GetConfigFileName() const {
    return file_name_;
  }
//----------------------------------------------------------------------------------------------------------------------
  void SetConfigFileName(const std::string &config_file_name) {
    file_name_ = config_file_name;
  }
//----------------------------------------------------------------------------------------------------------------------
  bool HasMember(const std::string &parameter) {
    return document_.HasMember(parameter.c_str());
  }
//----------------------------------------------------------------------------------------------------------------------
  int GetInt(const std::string &parameter) {
    if (document_[parameter.c_str()].IsInt()) {
      return document_[parameter.c_str()].GetInt();
    } else {
      return 0;
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  bool GetBool(const std::string &parameter) {
    if (document_[parameter.c_str()].IsBool()) {
      return document_[parameter.c_str()].GetBool();
    } else {
      return false;
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  std::string GetString(const std::string &parameter) {
    if (document_[parameter.c_str()].IsString()) {
      return document_[parameter.c_str()].GetString();
    } else {
      return std::string();
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  float GetFloat(const std::string &parameter) {
    if (document_[parameter.c_str()].IsFloat()) {
      return document_[parameter.c_str()].GetFloat();
    } else {
      return 0;
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  double GetDouble(const std::string &parameter) {
    if (document_[parameter.c_str()].IsDouble()) {
      return document_[parameter.c_str()].GetDouble();
    } else {
      return 0;
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  static bool Assign(bool &value, const std::string &parameter) {
    bool exists = GetSingleton().HasMember(parameter);

    if (GetSingleton().HasMember(parameter))
      value = GetSingleton().GetBool(parameter);

    return exists;
  }
//----------------------------------------------------------------------------------------------------------------------
  static bool Assign(int &value, const std::string &parameter) {
    bool exists = GetSingleton().HasMember(parameter);

    if (exists)
      value = GetSingleton().GetInt(parameter);

    return exists;
  }
//----------------------------------------------------------------------------------------------------------------------
  static bool Assign(std::string &value, const std::string &parameter) {
    bool exists = GetSingleton().HasMember(parameter);

    if (exists)
      value = GetSingleton().GetString(parameter);

    return exists;
  }
//----------------------------------------------------------------------------------------------------------------------
  static bool Assign(float &value, const std::string &parameter) {
    bool exists = GetSingleton().HasMember(parameter);

    if (exists)
      value = GetSingleton().GetFloat(parameter);

    return exists;
  }
//----------------------------------------------------------------------------------------------------------------------
  static bool Assign(double &value, const std::string &parameter) {
    bool exists = GetSingleton().HasMember(parameter);

    if (exists)
      value = GetSingleton().GetDouble(parameter);

    return exists;
  }
//----------------------------------------------------------------------------------------------------------------------
  template<typename T>
  void Add(const char *name, T &&value) {
    if (!document_.HasMember(name))
      document_.AddMember(static_cast<rapidjson::GenericStringRef<char>>(name), value, document_.GetAllocator());
  }

 private:
  std::string file_name_ = "config.json";
  rapidjson::Document document_;
}; //class ConfigManager
} //namespace Context
