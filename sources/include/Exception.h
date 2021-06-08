// This source file is part of context-project
// Created by Andrew Vasiliev

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
