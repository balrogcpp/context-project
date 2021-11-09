// This source file is part of "glue project"
// Created by Andrew Vasiliev

#pragma once

#include <string>

#include "Exception.h"
#include "SubSystem.h"

namespace glue {

class Scene final : public SubSystem {
 public:
  Scene();
  virtual ~Scene();

  void Update(float time) override;

 private:
  std::string resource_group_;
};

}  // namespace glue
