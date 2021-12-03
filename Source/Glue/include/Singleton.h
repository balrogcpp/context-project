// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Exception.h"
#include "NoCopy.h"

namespace Glue {

/// Implement "Dynamic Singleton" pattern. Keeps flag that class was already instanced once.
/// Throws exception when tried to create second instance
template <typename T>
class Singleton : public NoCopy {
 public:
  /// Constructor
  Singleton() {
    if (instanced_) {
      std::string Type = typeid(T).name();
      std::string text = Type + " is DynamicSingleton class. Creation of another instance is forbidden";
      Throw(text);
    }

    instanced_ = true;
  }

 protected:
  /// Is this flag is up -- any call of "new" will throw exception
  inline static bool instanced_ = false;
};

/// Implements "Lazy DynamicSingleton" pattern. Creates instance only when called GetInstance
template <typename T>
class LazySingleton : public Singleton<T> {
 public:
  /// Create and return instance of class
  /// \return ref to instance
  static T& GetInstance() {
    static T t;
    return t;
  }

  /// Create and return instance of class
  /// \return raw pointer to instance
  static T* GetInstancePtr() {
    static T t;
    return &t;
  }
};

}  // namespace glue
