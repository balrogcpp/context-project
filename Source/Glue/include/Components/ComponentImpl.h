// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Component.h"

namespace Glue {

class ComponentImpl : public Component<ComponentImpl> {
 public:
  void OnPause() override;
  void OnResume() override;
  void OnUpdate(float PassedTime);
  void OnClean() override;
};

}  // namespace Glue
