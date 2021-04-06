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
#include <nlohmann/json.hpp>
#include <filesystem>
#include <string>

namespace xio {

class Configurator : public NoCopy {
 public:

  explicit Configurator(const std::string &file_name);

  virtual ~Configurator();

  void Load(const std::string &file_name);

//----------------------------------------------------------------------------------------------------------------------
  template<typename T>
  void AddMember(const std::string &key, T &&value) {
	document_[key] = value;
  }

//----------------------------------------------------------------------------------------------------------------------
  template<typename T>
  T Get(const std::string &key) {
	T t{};

	if (document_.find(key)!=document_.end())
	  t = static_cast<T>(document_[key]);

	return t;
  }

//----------------------------------------------------------------------------------------------------------------------
  template<typename T>
  bool Get(const std::string &key, T &t_) {

	if (document_.find(key)!=document_.end()) {
	  t_ = static_cast<T>(document_[key]);
	  return true;
	} else {
	  return false;
	}

  }

 private:
  nlohmann::json document_;
};

} //namespace
