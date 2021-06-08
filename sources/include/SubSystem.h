// This source file is part of context-project
// Created by Andrew Vasiliev

#pragma once
#include "NoCopy.h"
#include "view_ptr.h"

namespace xio {
class Config;

class SubSystem : public NoCopy {
 public:
  SubSystem() {}
  virtual ~SubSystem() {}

  virtual void Update(float time) = 0;

 protected:
  inline static view_ptr<Config> conf_;

 public:
  static void SetConfig(view_ptr<Config> conf) { conf_ = conf; }
};

}  // namespace xio
