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

#include "Exception.h"

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <filesystem>
#include <string>
#include <iostream>

namespace Context {

class ConfiguratorJson {
 public:
//----------------------------------------------------------------------------------------------------------------------
  static ConfiguratorJson &GetSingleton() {
    static ConfiguratorJson ConfigManagerSingleton;
    return ConfigManagerSingleton;
  }

  ConfiguratorJson() {
    Init_();
  }

  ConfiguratorJson(const ConfiguratorJson &) = delete;
  ConfiguratorJson &operator=(const ConfiguratorJson &) = delete;
  ConfiguratorJson(Singleton &&) = delete;
  ConfiguratorJson &operator=(ConfiguratorJson &&) = delete;
  ~ConfiguratorJson() = default;

//----------------------------------------------------------------------------------------------------------------------
  void Print() {
    if (GetBool("global_verbose_enable")) {

      std::cout << "Global config variables:\n";
      std::cout << "--------------------------------------------\n";

      int counter = 0;
      for (auto it = document_.Begin(); it != document_.End(); ++it) {
        if (counter == 0) {
          std::cout << "variable: ";
        }

        const char true_string[] = "True";
        const char false_string[] = "False";

        if (it->IsString()) {
          std::cout << it->GetString();
        } else if (it->IsBool()) {
          std::cout << (it->GetBool() ? true_string : false_string);
        } else if (it->IsInt()) {
          std::cout << it->GetInt();
        } else if (it->IsInt64()) {
          std::cout << it->GetInt64();
        } else if (it->IsFloat()) {
          std::cout << it->GetFloat();
        } else if (it->IsDouble()) {
          std::cout << it->GetDouble();
        }

        if (counter == 0) {
          std::cout << " | value: ";
          counter++;
        } else if (counter == 1) {
          std::cout << '\n';
          counter = 0;
        }
      }

      std::cout << '\n';
    }
  }

 private:
  //----------------------------------------------------------------------------------------------------------------------
  void Init_() {
//Because Android cland does not support std filesystem
#ifndef __ANDROID__
    if (!std::filesystem::exists(file_name_)) {
      file_name_ = std::string("../") + file_name_;

      if (!std::filesystem::exists(file_name_)) {
        throw Exception("Error during parsing of " + file_name_ + " : file not exist");
      }
    }
#endif

    std::ifstream ifs(file_name_);
    rapidjson::IStreamWrapper isw(ifs);
    document_.ParseStream(isw);

    if (ifs.is_open())
      ifs.close();
    else
      throw Exception("Error during parsing of " + file_name_ + " : can't open file");

    if (!document_.IsObject())
      throw Exception("Error during parsing of " + file_name_ + " : file is empty or incorrect");
  }

  std::string file_name_ = "config.json";
  rapidjson::Document document_;

 public:
//----------------------------------------------------------------------------------------------------------------------
  template<typename T>
  void AddMember(const char *name, T &&value) {
    static auto &allocator = document_.GetAllocator();
    if (!document_.HasMember(name)) {
      document_.AddMember(static_cast<rapidjson::GenericStringRef<char>>(name), value, allocator);
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  template<typename T>
  void AddMember(char *name, T &&value) {
    static auto &allocator = document_.GetAllocator();
    if (!document_.HasMember(name)) {
      document_.AddMember(static_cast<rapidjson::GenericStringRef<char>>(name), value, allocator);
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  template<typename T>
  void AddMember(const std::string &name, T &&value) {
    static auto &allocator = document_.GetAllocator();
    if (!document_.HasMember(name)) {
      document_.AddMember(static_cast<rapidjson::GenericStringRef<char>>(name.c_str()), value, allocator);
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  std::string GetConfigFileName() const {
    return file_name_;
  }
//----------------------------------------------------------------------------------------------------------------------
  void SetConfigFileName(const std::string &config_file_name) {
    file_name_ = config_file_name;
  }
//----------------------------------------------------------------------------------------------------------------------
  bool HasMember(const char *parameter) {
    return document_.HasMember(parameter);
  }
//----------------------------------------------------------------------------------------------------------------------
  int GetInt(const char *parameter) {
    if (document_[parameter].IsInt())
      return document_[parameter].GetInt();
    else
      return 0;
  }
//----------------------------------------------------------------------------------------------------------------------
  bool GetBool(const char *parameter) {
    if (document_[parameter].IsBool())
      return document_[parameter].GetBool();
    else
      return false;
  }
//----------------------------------------------------------------------------------------------------------------------
  std::string GetString(const char *parameter) {
    if (document_[parameter].IsString())
      return document_[parameter].GetString();
    else
      return std::string();
  }
//----------------------------------------------------------------------------------------------------------------------
  float GetFloat(const char *parameter) {
    if (document_[parameter].IsFloat())
      return document_[parameter].GetFloat();
    else
      return 0;
  }
//----------------------------------------------------------------------------------------------------------------------
  double GetDouble(const char *parameter) {
    if (document_[parameter].IsDouble())
      return document_[parameter].GetDouble();
    else
      return 0;
  }
//----------------------------------------------------------------------------------------------------------------------
  static bool Assign(bool &value, const char *parameter) {
    bool exists = GetSingleton().HasMember(parameter);

    if (GetSingleton().HasMember(parameter))
      value = GetSingleton().GetBool(parameter);

    return exists;
  }
//----------------------------------------------------------------------------------------------------------------------
  static bool Assign(int &value, const char *parameter) {
    bool exists = GetSingleton().HasMember(parameter);

    if (exists)
      value = GetSingleton().GetInt(parameter);

    return exists;
  }
//----------------------------------------------------------------------------------------------------------------------
  static bool Assign(std::string &value, const char *parameter) {
    bool exists = GetSingleton().HasMember(parameter);

    if (exists)
      value = GetSingleton().GetString(parameter);

    return exists;
  }
//----------------------------------------------------------------------------------------------------------------------
  static bool Assign(float &value, const char *parameter) {
    bool exists = GetSingleton().HasMember(parameter);

    if (exists)
      value = GetSingleton().GetFloat(parameter);

    return exists;
  }
//----------------------------------------------------------------------------------------------------------------------
  static bool Assign(double &value, const char *parameter) {
    bool exists = GetSingleton().HasMember(parameter);

    if (exists)
      value = GetSingleton().GetDouble(parameter);

    return exists;
  }
}; //class ConfigManager
} //namespace Context
