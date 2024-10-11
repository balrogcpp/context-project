/// created by Andrey Vasiliev

#pragma once
#include "Singleton.h"
#include <OgreFrameListener.h>

namespace gge {

/// @class ComponentI
/// Component interface. Components are
class ComponentI {
 public:
  virtual void OnSetUp() = 0;
  virtual void OnUpdate(float time) = 0;
  virtual void OnClean() = 0;
  virtual void SetSleep(bool sleep) { _sleep = sleep; }
  virtual bool IsSleeping() { return _sleep; }

 protected:
  bool _sleep = false;
};

template <typename T>
class Component : public ComponentI, public Singleton<T> {};

/// Template helper function
/// @return pointer to component instance (all components are Singletons)
template <typename T>
T* GetComponentPtr() {
  static T* ptr = Component<T>::GetInstancePtr();
  return ptr;
}

/// Template helper function
/// @return reference to component instance (all components are Singletons)
template <typename T>
T& GetComponent() {
  static T& ref = *Component<T>::GetInstancePtr();
  return ref;
}

}  // namespace gge
