// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include "Component.h"

namespace Glue {

class ComponentImpl : public Component<ComponentImpl> {
 public:
  ComponentImpl();
  virtual ~ComponentImpl();

  void OnPause() override;
  void OnResume() override;
  void OnUpdate(float PassedTime);
  void OnClean() override;
};

}  // namespace Glue
