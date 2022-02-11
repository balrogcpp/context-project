// This source file is part of "glue project". Created by Andrey Vasiliev

#pragma once
#include "Components/Component.h"

namespace Glue {

class Sky final : public Component<Sky> {
 public:
  Sky();
  virtual ~Sky();

  void OnSetUp() override;
  void OnPause() override;
  void OnResume() override;
  void OnUpdate(float PassedTime);
  void OnClean() override;
};

}  // namespace Glue
