// This source file is part of context-project
// Created by Andrew Vasiliev

#pragma once
#include <nlohmann/json.hpp>
#include <string>

#include "Exception.h"
#include "Singleton.h"

namespace glue {

class Config : public Singleton<Config> {
 public:
  explicit Config(const std::string &file_name);

  virtual ~Config();

  void Load(const std::string &file_name);

  //----------------------------------------------------------------------------------------------------------------------
  template <typename T>
  void AddMember(const std::string &key, T &&value) {
    document_[key] = value;
  }

  //----------------------------------------------------------------------------------------------------------------------
  template <typename T>
  T Get(const std::string &key) {
    T t{};

    if (document_.find(key.c_str()) != document_.end()) t = static_cast<T>(document_[key.c_str()]);

    return t;
  }

  //----------------------------------------------------------------------------------------------------------------------
  template <typename T>
  bool Get(const std::string &key, T &t_) {
    if (document_.find(key.c_str()) != document_.end()) {
      t_ = static_cast<T>(document_[key.c_str()]);
      return true;
    } else {
      return false;
    }
  }

 private:
  nlohmann::json document_;
};

}  // namespace glue
