// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "NoCopy.h"
#include "ViewPtr.h"

namespace glue {
class Config;

class SubSystem : public NoCopy {
 public:
  SubSystem() {}
  virtual ~SubSystem() {}

  virtual void Update(float time) = 0;
};

}  // namespace glue
