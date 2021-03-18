//MIT License
//
//Copyright (c) 2021 Andrew Vasiliev
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
#include "Exception.h"

#include <filesystem>
#include <string>
#include <fstream>
#include <exception>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

namespace xio {

class JsonConfigurator : public NoCopy {
 public:
  explicit JsonConfigurator(const std::string &file = "config.json");

//----------------------------------------------------------------------------------------------------------------------
  virtual ~JsonConfigurator();

//----------------------------------------------------------------------------------------------------------------------
  void Load(const std::string &file_name);

//----------------------------------------------------------------------------------------------------------------------
  template<typename T>
  void AddMember(const char *str, T &&value) {
    static auto &allocator = document_.GetAllocator();
    if (!document_.HasMember(str)) {
      document_.AddMember(static_cast<rapidjson::GenericStringRef<char>>(str), value, allocator);
    }
  }

//----------------------------------------------------------------------------------------------------------------------
  template<typename T>
  inline T Get(const char *str) {
    T t{};

    try {
      t = document_[str].Get<T>();
    } catch (std::exception &e) {
      //ignore
    }

    return t;
  }

 private:
  rapidjson::Document document_;
};

} //namespace
