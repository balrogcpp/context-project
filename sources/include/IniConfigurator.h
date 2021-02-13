//MIT License
//
//Copyright (c) 2020 Andrey Vasiliev
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#pragma once

#include "NoCopy.h"
#include <INIReader.h>
#include <string>
#include <memory>
#include <type_traits>

namespace xio {
class IniParserException : public std::exception {
 public:
  IniParserException() = default;

  explicit IniParserException(std::string description)
      : description(std::move(description)) {}

  virtual ~IniParserException() {}

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

class IniConfigurator : public NoCopy {
 public:
  explicit IniConfigurator(const std::string &file = "config.yaml") {
    Load(file);
  }

  IniConfigurator(const IniConfigurator &) = delete;
  IniConfigurator &operator=(const IniConfigurator &) = delete;
  virtual ~IniConfigurator() {}

  void Load(const std::string &file) {
    document_ = std::make_unique<INIReader>(file);
  }
 private:
  std::unique_ptr<INIReader> document_;

 public:
//----------------------------------------------------------------------------------------------------------------------
  template<typename T>
  inline T Get(const std::string &str) {
    const std::string section = "Default";

    T t{};

    try {
      if (std::is_floating_point<T>::value) {
        t = document_->GetReal(section, str, 0.0);
      } else if (std::is_same<T, bool>::value) {
        t = document_->GetBoolean(section, str, false);
      } else if (std::is_integral<T>::value) {
        t = document_->GetInteger(section, str, 0);
      } else if (std::is_same<T, std::string>::value) {
        //t = document_->GetString(section, str, "");
      }
    }
    catch (std::exception &e) {
      //ignore
    }

    return t;
  }
};
}