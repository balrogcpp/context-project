// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "DynamicSingleton.h"

namespace Glue {

/// Implements "Lazy DynamicSingleton" pattern. Creates instance only when called GetInstance
template <typename T>
class LazySingleton : public DynamicSingleton<T> {
 public:
  /// Create and return instance of class
  /// \return ref to instance
  static T& GetInstance() {
    static T t;
    return t;
  }
};

}
