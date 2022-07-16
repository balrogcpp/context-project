// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include "Singleton.h"

namespace Glue {

/// @class ComponentI
/// Component interface. Components are
class ComponentI {
 public:
  ComponentI();
  virtual ~ComponentI();

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

}  // namespace Glue
