// This source file is part of "glue project"
// Created by Andrew Vasiliev

#pragma once

#include "Input.h"
#include "NoCopy.h"

namespace glue {
class StateManager;

class AppState : public Ogre::RenderTargetListener, public Ogre::FrameListener, public InputObserver {
 public:
  AppState();
  virtual ~AppState();

  void ChangeState(std::unique_ptr<AppState> &&app_state);
  void ChangeState();
  void SetNextState(std::unique_ptr<AppState> &&next_state);
  void LoadFromFile(const std::string &file_name, const std::string &group = Ogre::RGN_DEFAULT);
  bool IsDirty() const;

  virtual void Init() = 0;
  virtual void Cleanup() = 0;
  virtual void Pause() = 0;
  virtual void Resume() = 0;
  virtual void Update(float time) = 0;

 protected:
  friend class StateManager;
  std::unique_ptr<AppState> next_;
  bool dirty_ = false;
};

}  // namespace glue
