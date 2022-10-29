/// created by Andrey Vasiliev

#pragma once
#include "Assertion.h"

namespace Glue {
/// Deletes copy constructor
class NoCopy {
 public:
  NoCopy() = default;
  NoCopy(const NoCopy &) = delete;
  NoCopy &operator=(const NoCopy &) = delete;
  NoCopy(NoCopy &&) = default;
  NoCopy &operator=(NoCopy &&) = default;
  virtual ~NoCopy() = default;
};

/// Implement "Dynamic Singleton" pattern. Keeps flag that class was already instanced once.
/// Throws exception when tried to create second instance
template <typename T>
class DynamicSingleton : public NoCopy {
 public:
  /// Constructor
  DynamicSingleton() {
    OgreAssert(!instanced, "[DynamicSingleton] Creation of another singleton instance is forbidden");
    instanced = true;
  }

  ///
  static bool IsInstanced() { return instanced; }

 protected:
  /// Is this flag is up -- any call of "new" will abort program
  inline static bool instanced = false;
};

/// Implement "Dynamic Singleton" pattern. Keeps flag that class was already instanced once.
/// Throws exception when tried to create second instance
template <typename T>
class Singleton : public DynamicSingleton<T> {
 public:
  /// Constructor
  Singleton() { instance = static_cast<T *>(this); }

  ///
  static T *GetInstancePtr() { return instance; }

  ///
  static T &GetInstance() { return *instance; }

 protected:
  /// Pointer to singleton instance
  inline static T *instance = nullptr;
};

/// Implements "Lazy DynamicSingleton" pattern. Creates instance only when called GetInstance
template <typename T>
class LazySingleton : public DynamicSingleton<T> {
 public:
  /// Create and return instance of class
  /// \return ref to instance
  static T &GetInstance() {
    static T t;
    return t;
  }
};
}  // namespace Glue
