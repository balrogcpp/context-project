// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "NoCopy.h"
#include "ViewPtr.h"

namespace glue {
class Config;

class System : public NoCopy {
 public:
  System();
  virtual ~System();

  virtual void Cleanup() = 0;
  virtual void Pause() { paused = true; }
  virtual void Resume() { paused = false; }
  virtual void Update(float time) = 0;

 protected:
  inline static ViewPtr<Config> config = nullptr;
  bool paused = false;

 public:
  static void SetConfig(ViewPtr<Config> conf) { config = conf; }
};

}  // namespace glue
