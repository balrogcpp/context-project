// MIT License
//
// Copyright (c) 2021 Andrew Vasiliev
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <exception>
#include <stdexcept>
#include <string>
#include <utility>

namespace xio {

class Exception : public std::exception {
 public:
  Exception() = default;

  //----------------------------------------------------------------------------------------------------------------------
  explicit Exception(std::string description) : description(std::move(description)){};

  //----------------------------------------------------------------------------------------------------------------------
  ~Exception() noexcept override = default;

  //----------------------------------------------------------------------------------------------------------------------
  std::string getDescription() const noexcept { return description; }

  //----------------------------------------------------------------------------------------------------------------------
  const char *what() const noexcept override { return description.c_str(); }

 private:
  std::string description = "Description not specified";
};

/////Throw exception
// void Assert(bool result, std::string message) {
//   if (!result) {
//     throw Exception(std::move(message));
//   }
// }
//
/////Throw exception
// void Throw(std::string message) {
//   throw Exception(std::move(message));
// }
}  // namespace xio
