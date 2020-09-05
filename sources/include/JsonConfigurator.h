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
  explicit JsonConfigurator(const std::string &file = "config.json") {
    Load(file);
  }

  JsonConfigurator(const JsonConfigurator &) = delete;
  JsonConfigurator &operator=(const JsonConfigurator &) = delete;
  virtual ~JsonConfigurator() {}

  void Load(const std::string &file) {
    std::ifstream ifs(file);
    rapidjson::IStreamWrapper isw(ifs);
    document_.ParseStream(isw);

    if (ifs.is_open())
      ifs.close();
    else
      throw JsonParserException("Error during parsing of " + file + " : can't open file");

    if (!document_.IsObject())
      throw JsonParserException("Error during parsing of " + file + " : file is empty or incorrect");
  }

 private:
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
  template<typename T>
  T Get(const std::string &str) {
    return document_[str].Get<T>();
  }
};
}