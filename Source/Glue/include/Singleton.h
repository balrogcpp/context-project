// This source file is part of "glue project". Created by Andrew Vasiliev

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
    if (Instanced) {
      std::string Message = std::string(typeid(T).name()) + " is a Singleton class. Creation of another instance is forbidden";
      Throw(Message);
    }

    Instanced = true;
    SingletonPtr = static_cast<T*>(this);
  }

  ///
  static T* GetInstancePtr() { return SingletonPtr; }

  ///
  static T& GetInstance() { return *SingletonPtr; }

  ///
  static bool IsInstanced() { return Instanced; }

 protected:
  /// Pointer to singleton instance
  inline static T* SingletonPtr = nullptr;
  /// Is this flag is up -- any call of "new" will throw exception
  inline static bool Instanced = false;
};

}  // namespace Glue
