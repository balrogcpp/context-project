/// created by Andrey Vasiliev

#pragma once
#include <stdexcept>

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
