// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "NoCopy.h"
#include "Input/InputObserver.h"
#include <OgreFrameListener.h>
#include <OgreRenderTargetListener.h>
#include <OgreResourceGroupManager.h>
#include <memory>
#include <string>

namespace Glue {

class AppState;
using AppStateUPtr = std::unique_ptr<AppState>;

class AppState : public Ogre::RenderTargetListener, public Ogre::FrameListener, public InputObserver {
 public:
  AppState();
  virtual ~AppState();

  void ChangeState(std::unique_ptr<AppState> &&AppStatePtr);
  void ChangeState();
  void AppendNextState(std::unique_ptr<AppState> &&NextStatePtr);
  void LoadFromFile(const std::string &FileName, const std::string &GroupName = Ogre::RGN_DEFAULT);
  bool IsDirty() const;

  virtual void SetUp() = 0;
  virtual void Cleanup() = 0;
  virtual void Pause() = 0;
  virtual void Resume() = 0;
  virtual void Update(float TimePassed) = 0;

 protected:
  friend class AppStateManager;
  std::unique_ptr<AppState> NextState;
  bool Dirty = false;
};

}  // namespace Glue
