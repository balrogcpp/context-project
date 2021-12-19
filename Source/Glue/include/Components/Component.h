// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Singleton.h"

namespace Glue {
class Conf;
}

namespace Glue {

class Component : public NoCopy {
 public:
  Component();
  virtual ~Component();

  virtual void Pause() { Paused = true; }
  virtual void Resume() { Paused = false; }
  virtual void Update(float PassedTime) {}
  virtual void Cleanup() {}

  bool IsPaused() { return Paused; }
  static void SetConfig(Conf* Conf) { ConfPtr = Conf; }

 protected:
  inline static Conf* ConfPtr = nullptr;
  bool Paused = false;
};

}  // namespace Glue
