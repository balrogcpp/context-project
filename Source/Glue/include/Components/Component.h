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
  static void SetConfig(Conf* Conf) { ConfPtr = Conf; }

 protected:
  inline static Conf* ConfPtr = nullptr;
  bool Paused = false;
};

template <typename T>
class Component : public ComponentI, public Singleton<T> {};

}  // namespace Glue
