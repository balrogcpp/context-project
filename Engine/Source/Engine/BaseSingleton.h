// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include "NoCopy.h"
#include <stdexcept>

namespace Glue {

/// Implement "Dynamic Singleton" pattern. Keeps flag that class was already instanced once.
/// Throws exception when tried to create second instance
template <typename T>
class BaseSingleton : public NoCopy {
 public:
  /// Constructor
  BaseSingleton() {
    if (Instanced) throw std::runtime_error(std::string(typeid(T).name()) + " is a Singleton class. Creation of another instance is forbidden");
    Instanced = true;
  }

  ///
  static bool IsInstanced() { return Instanced; }

 protected:
  /// Is this flag is up -- any call of "new" will throw exception
  inline static bool Instanced = false;
};

}  // namespace Glue
