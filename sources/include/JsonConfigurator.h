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

#include <filesystem>
#include <string>
#include <exception>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

namespace Context {
class JsonParserException : public std::exception {
 public:
  JsonParserException() = default;

  explicit JsonParserException(std::string description)
      : description(std::move(description)) {};

  ~JsonParserException() noexcept override = default;

 public:
  std::string getDescription() const noexcept {
    return description;
  }

  const char *what() const noexcept override {
    return description.c_str();
  }

 protected:
  std::string description = std::string("Description not specified");
  size_t code = 0;
};

class JsonConfigurator {
 public:
//----------------------------------------------------------------------------------------------------------------------
  JsonConfigurator() {
//Because Android cland does not support std filesystem
#ifndef __ANDROID__
    if (!std::filesystem::exists(file_)) {
      file_ = std::string("../") + file_;

      if (!std::filesystem::exists(file_)) {
        throw JsonParserException("Error during parsing of " + file_ + " : file not exist");
      }
    }
#endif

    std::ifstream ifs(file_);
    rapidjson::IStreamWrapper isw(ifs);
    document_.ParseStream(isw);

    if (ifs.is_open())
      ifs.close();
    else
      throw JsonParserException("Error during parsing of " + file_ + " : can't open file");

    if (!document_.IsObject())
      throw JsonParserException("Error during parsing of " + file_ + " : file is empty or incorrect");
  }

  JsonConfigurator(const JsonConfigurator &) = delete;
  JsonConfigurator &operator=(const JsonConfigurator &) = delete;
  virtual ~JsonConfigurator() {};

 private:
//----------------------------------------------------------------------------------------------------------------------
  std::string file_ = "config.json";
  rapidjson::Document document_;

 public:
//----------------------------------------------------------------------------------------------------------------------
  template<typename T>
  void AddMember(const std::string &name, T &&value) {
    static auto &allocator = document_.GetAllocator();
    if (!document_.HasMember(name)) {
      document_.AddMember(static_cast<rapidjson::GenericStringRef<char>>(name.c_str()), value, allocator);
    }
  }
//----------------------------------------------------------------------------------------------------------------------
  std::string GetFileName() const {
    return file_;
  }
//----------------------------------------------------------------------------------------------------------------------
  void SetFileName(const std::string &file) {
    file_ = file;
  }
//----------------------------------------------------------------------------------------------------------------------
  bool HasMember(const std::string &parameter) {
    return document_.HasMember(parameter);
  }
//----------------------------------------------------------------------------------------------------------------------
  template<typename T>
  T as(const std::string &str) {
    return document_[str].Get<T>();
  }
//----------------------------------------------------------------------------------------------------------------------
  int GetInt(const std::string &parameter) {
    if (document_[parameter].IsInt())
      return document_[parameter].GetInt();
    else
      return 0;
  }
//----------------------------------------------------------------------------------------------------------------------
  bool GetBool(const std::string &parameter) {
    if (document_[parameter].IsBool())
      return document_[parameter].GetBool();
    else
      return false;
  }
//----------------------------------------------------------------------------------------------------------------------
  std::string GetString(const std::string &parameter) {
    if (document_[parameter].IsString())
      return document_[parameter].GetString();
    else
      return std::string();
  }
//----------------------------------------------------------------------------------------------------------------------
  float GetFloat(const std::string &parameter) {
    if (document_[parameter].IsFloat())
      return document_[parameter].GetFloat();
    else
      return 0;
  }
//----------------------------------------------------------------------------------------------------------------------
  double GetDouble(const std::string &parameter) {
    if (document_[parameter].IsDouble())
      return document_[parameter].GetDouble();
    else
      return 0;
  }
}; //class ConfigManager
} //namespace Context
