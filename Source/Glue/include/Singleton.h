// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include "Exception.h"
#include "BaseSingleton.h"

namespace Glue {

/// Implement "Dynamic Singleton" pattern. Keeps flag that class was already instanced once.
/// Throws exception when tried to create second instance
template <typename T>
class Singleton : public BaseSingleton<T> {
 public:
  /// Constructor
  Singleton() {
    SingletonPtr = static_cast<T*>(this);
  }

  ///
  static T* GetInstancePtr() { return SingletonPtr; }

  ///
  static T& GetInstance() { return *SingletonPtr; }

 protected:
  /// Pointer to singleton instance
  inline static T* SingletonPtr = nullptr;
};

}  // namespace Glue
