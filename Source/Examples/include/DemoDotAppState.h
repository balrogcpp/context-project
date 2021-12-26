// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "AppState.h"

namespace Demo {

class DemoDotAppState : public Glue::AppState {
 public:
  DemoDotAppState();
  virtual ~DemoDotAppState();

  void SetUp() override;
  void Cleanup() override;
  void Pause() override;
  void Resume() override;
  void Update(float time) override;

  void OnKeyDown(SDL_Keycode sym) override;

 private:
  //  Ogre::AnimationState *anim1 = nullptr;
  //  Ogre::AnimationState *anim2 = nullptr;
  bool context_menu_ = false;
};
}  // namespace Demo
