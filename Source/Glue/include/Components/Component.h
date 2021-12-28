// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Singleton.h"

namespace Glue {
class Conf;
}

namespace Glue {

class ComponentI {
 public:
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

}  // namespace Glue
