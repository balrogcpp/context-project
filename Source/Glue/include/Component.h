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

  virtual void Pause() { paused = true; }
  virtual void Resume() { paused = false; }
  virtual void Update(float time) = 0;
  virtual void Cleanup() = 0;

 protected:
  inline static Config* config = nullptr;
  bool paused = false;

 public:
  static void SetConfig(Config* conf) { config = conf; }
};

}  // namespace Glue
