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
  virtual void Update(float time) = 0;
  virtual void Cleanup() = 0;

  static void SetConfig(Config* conf) { ConfPtr = conf; }

 protected:
  inline static Config* ConfPtr = nullptr;
  bool Paused = false;
};

}  // namespace Glue
