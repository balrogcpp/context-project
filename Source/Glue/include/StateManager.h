// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "AppState.h"
#include "LazySingleton.h"

namespace Glue {

class StateManager : public DynamicSingleton<StateManager> {
 public:
  StateManager();
  virtual ~StateManager();

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
