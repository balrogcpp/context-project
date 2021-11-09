// This source file is part of "glue project"
// Created by Andrew Vasiliev

#pragma once
#include "Exception.h"
#include "NoCopy.h"
namespace glue {

template <typename T>
class Singleton : public NoCopy {
 public:
  Singleton() {
    if (instanced_) {
      std::string type = typeid(T).name();
      std::string text = std::string(type) + " is Singleton class. Creation of another instance is forbidden";

      throw Exception(text);
    }

    instanced_ = true;
  }

 protected:
  inline static bool instanced_ = false;
};

template <typename T>
class LazySingleton : public Singleton<T> {
 public:
  static T& GetInstance() {
    static T t;
    return t;
  }
};

}  // namespace glue
