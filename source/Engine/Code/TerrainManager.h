/// created by Andrey Vasiliev

#pragma once
#include "System.h"

namespace Glue {

class TerrainManager : public System<TerrainManager> {
 public:
  void OnSetUp() override;
  void OnUpdate(float time) override;
  void OnClean() override;
};

}
