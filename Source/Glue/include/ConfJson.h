// This source file is part of "glue project". Created by Andrey Vasiliev

#pragma once
#include "Singleton.h"
#include "Exception.h"
#include <nlohmann/json.hpp>

namespace Glue {

class ConfJson : public Singleton<ConfJson> {
 public:
  explicit ConfJson(const std::string &FileName) {
    if (FileName.empty()) return;
    Reload(FileName);
  }

  virtual ~ConfJson() {}

  void Reload(const std::string &FileName) {
    assert(!FileName.empty() && "Provided conf file name is empty");
    Document.clear();
    std::ifstream ifs(FileName);

    if (!ifs.is_open()) {
      Throw(FileName + " does not exists");
    }

    ifs >> Document;
  }

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
