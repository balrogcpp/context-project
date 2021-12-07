// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "LazySingleton.h"
#include "Exception.h"
#include <nlohmann/json.hpp>

namespace Glue {

class Config : public DynamicSingleton<Config> {
 public:
  explicit Config(const std::string &FileName);

  virtual ~Config();

  void Load(const std::string &FileName);

  template <typename T>
  void AddMember(const std::string &Key, T &&Value) {
    Document[Key] = Value;
  }

  template <typename T>
  T Get(const std::string &Key) {
    T t{};

    if (Document.find(Key.c_str()) != Document.end()) {
      t = static_cast<T>(Document[Key.c_str()]);
    }

    return t;
  }

  template <typename T>
  bool Get(const std::string &Key, T &Value) {
    return Get(Key.c_str(), Value);
  }

  template <typename T>
  bool Get(const char *Key, T &Value) {
    if (Document.find(Key) != Document.end()) {
      Value = static_cast<T>(Document[Key]);
      return true;
    } else {
      return false;
    }
  }

 protected:
  nlohmann::json Document;
};

}  // namespace Glue
