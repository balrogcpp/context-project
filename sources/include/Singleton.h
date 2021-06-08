// This source file is part of context-project
// Created by Andrew Vasiliev

#pragma once
#include "Exception.h"
#include "NoCopy.h"
namespace xio {

template <typename T>
class Singleton : public NoCopy {
 public:
  Singleton() {
    if (instanced_) throw Exception("Only one instance of Singleton can be created!\n");

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

}  // namespace xio
