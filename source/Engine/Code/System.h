/// created by Andrey Vasiliev

#pragma once
#include "Singleton.h"
#include <OgreFrameListener.h>

namespace Glue {
/// @class ComponentI
/// Component interface. Components are
class SystemI {
 public:
  virtual void OnSetUp() = 0;
  virtual void OnUpdate(float time) = 0;
  virtual void OnClean() = 0;
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
