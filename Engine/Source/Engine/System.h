// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include "Singleton.h"

namespace Glue {

/// @class ComponentI
/// Component interface. Components are
class SystemI {
 public:
  virtual void OnSetUp() {}
  virtual void OnPause() { Paused = true; }
  virtual void OnResume() { Paused = false; }
  virtual void OnUpdate(float PassedTime) {}
  virtual void OnClean() {}

  bool IsPaused() { return Paused; }

 protected:
  bool Paused = false;
};

template <typename T>
class System : public SystemI, public Singleton<T> {};

/// Template helper function
/// @return pointer to component instance (all components are Singletons)
template <typename T>
T* GetComponentPtr() {
  static T* ptr = System<T>::GetInstancePtr();
  return ptr;
}

/// Template helper function
/// @return reference to component instance (all components are Singletons)
template <typename T>
T& GetComponent() {
  static T& ref = *System<T>::GetInstancePtr();
  return ref;
}

}  // namespace Glue
