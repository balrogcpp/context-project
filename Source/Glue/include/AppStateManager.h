// This source file is part of "glue project". Created by Andrey Vasiliev

#pragma once
#include "AppState.h"

namespace Glue {

class AppStateManager {
 public:
  AppStateManager();
  virtual ~AppStateManager();

  void InitCurState();
  void InitNextState();
  void SetInitialState(std::unique_ptr<AppState> &&InitialState);
  void Update(float PassedTime);
  bool IsActive() const;
  bool IsDirty() const;
  void Pause();
  void Resume();

 protected:
  std::unique_ptr<AppState> CurrentState;
};

}  // namespace Glue
