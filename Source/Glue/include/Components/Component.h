// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "NoCopy.h"

namespace Glue {
class Config;
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
  static void SetConfig(Config* Conf) { ConfPtr = Conf; }

 protected:
  inline static Config* ConfPtr = nullptr;
  bool Paused = false;
};

}  // namespace Glue
