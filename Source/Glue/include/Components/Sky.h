// This source file is part of "glue project". Created by Andrey Vasiliev

#pragma once
#include "Components/Component.h"

namespace Glue {

class Sky final : public Component<Sky> {
 public:
  Sky();
  virtual ~Sky();
};

}  // namespace Glue
