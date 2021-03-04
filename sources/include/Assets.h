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

#include <Ogre.h>

#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include <tuple>

namespace xio {
class StorageException : public std::exception {
 public:
  StorageException() = default;

  explicit StorageException(std::string description)
	  : description(std::move(description)) {};

  ~StorageException() noexcept override = default;

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

class Assets {

  static bool StringSanityCheck(const std::string &str);

  static void LeftTrim(std::string &s);

  static void RightTrim(std::string &s);

  static void TrimString(std::string &s);

  static void PrintPathList(const std::vector<std::tuple<std::string, std::string, std::string>> &path_list);

  static void PrintStringList(const std::vector<std::string> &string_list);
 public:

  static void LoadResources();

  std::vector<std::tuple<std::string, std::string, std::string>>
  static InitGeneralResources(const std::vector<std::string> &path_list,
							  const std::string &resource_file = "",
							  bool verbose = false);
};
}