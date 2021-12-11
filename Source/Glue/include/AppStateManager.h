// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "AppState.h"
#include "LazySingleton.h"

namespace Glue {

class AppStateManager : public Singleton<AppStateManager> {
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
