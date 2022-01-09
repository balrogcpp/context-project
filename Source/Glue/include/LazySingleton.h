// This source file is part of "glue project". Created by Andrey Vasiliev

#pragma once
#include "BaseSingleton.h"

namespace Glue {

/// Implements "Lazy DynamicSingleton" pattern. Creates instance only when called GetInstance
template <typename T>
class LazySingleton : public BaseSingleton<T> {
 public:
  /// Create and return instance of class
  /// \return ref to instance
  static T& GetInstance() {
    static T t;
    return t;
  }
};

}  // namespace Glue
