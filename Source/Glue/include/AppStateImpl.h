// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "AppState.h"

namespace Glue {

class AppStateImpl : public AppState {
 public:
  void SetUp() override;
  void Cleanup() override;
  void Pause() override;
  void Resume() override;
  void Update(float TimePassed) override;
};

}  // namespace Glue
