/// created by Andrey Vasiliev

#pragma once
#include "System.h"

namespace Glue {

class SkyManager : public System<SkyManager> {
 public:
  void OnSetUp() override;
  void OnUpdate(float time) override;
  void OnClean() override;
};

}  // namespace Glue
