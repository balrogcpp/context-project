// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Exception.h"
#include "Singleton.h"
#include <nlohmann/json.hpp>

namespace glue {

class Config : public Singleton<Config> {
 public:
  explicit Config(const std::string &file_name);

  virtual ~Config();

  void Load(const std::string &file_name);

  template <typename T>
  void AddMember(const std::string &key, T &&value) {
    document[key] = value;
  }

  template <typename T>
  T Get(const std::string &key) {
    T t{};

    if (document.find(key.c_str()) != document.end()) {
      t = static_cast<T>(document[key.c_str()]);
    }

    return t;
  }

  template <typename T>
  bool Get(const std::string &key, T &t) {
    return Get(key.c_str(), t);
  }

  template <typename T>
  bool Get(const char *key, T &t) {
    if (document.find(key) != document.end()) {
      t = static_cast<T>(document[key]);
      return true;
    } else {
      return false;
    }
  }

 protected:
  nlohmann::json document;
};

}  // namespace glue
