// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include <exception>
#include <string>
#include <utility>

namespace glue {

class Exception : public std::exception {
 public:
  Exception() = default;
  explicit Exception(std::string description) : description(std::move(description)){};
  ~Exception() noexcept override = default;

  std::string GetDescription() const noexcept { return description; }
  const char *what() const noexcept override { return description.c_str(); }

 protected:
  std::string description = "Description not specified";
};

/// Throw exception
void Assert(bool result, std::string message);

/// Throw exception
void Throw(std::string message);

}  // namespace glue
