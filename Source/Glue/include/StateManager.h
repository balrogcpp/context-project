// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "AppState.h"
#include "Singleton.h"

namespace Glue {

class StateManager : public Singleton<StateManager> {
 public:
  StateManager();
  virtual ~StateManager();

  void InitCurState();
  void InitNextState();
  void SetInitialState(std::unique_ptr<AppState> &&next_state);
  void Update(float time);
  bool IsActive() const;
  bool IsDirty() const;
  void Pause();
  void Resume();

 protected:
  std::unique_ptr<AppState> cur_state;
};

}  // namespace glue
