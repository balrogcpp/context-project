// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include <exception>
#include <string>

namespace Glue {

/// Exception class
class Exception : public std::exception {
 public:
  Exception() = default;
  explicit Exception(std::string Description) : Description(std::move(Description)){};
  explicit Exception(const char *Description) : Description(Description){};
  ~Exception() noexcept override = default;

  std::string GetDescription() const noexcept { return Description; }
  const char *what() const noexcept override { return Description.c_str(); }

 protected:
  std::string Description = "Not specified";
};

/// Throw exception
void Assert(bool Result, std::string Message);

/// Throw exception
void Throw(std::string Message);

/// Creates pop-up window with exception description
int ErrorWindow(const std::string &WindowCaption, const std::string &MessageText);

}  // namespace Glue
