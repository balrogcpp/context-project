// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Exception.h"
#include "NoCopy.h"

namespace Glue {

/// Implement "Dynamic Singleton" pattern. Keeps flag that class was already instanced once.
/// Throws exception when tried to create second instance
template <typename T>
class DynamicSingleton : public NoCopy {
 public:
  /// Constructor
  DynamicSingleton() {
    if (Instanced) {
      std::string Type = typeid(T).name();
      std::string Message = Type + " is DynamicSingleton class. Creation of another instance is forbidden";
      Throw(Message);
    }

    Instanced = true;
  }

 protected:
  /// Is this flag is up -- any call of "new" will throw exception
  inline static bool Instanced = false;
};

}  // namespace Glue
