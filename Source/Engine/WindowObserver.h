// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
extern "C" {
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
}

namespace Glue {

/// Interface for Window events listener listener
class WindowObserver {
 public:
  /// Callback called on any windows event
  /// @param event SDL_Event structure
  virtual void OnEvent(const SDL_Event &event) {}
  /// Callback called on Quit
  virtual void OnQuit() {}
  /// Callback called when Window is not in focus
  virtual void OnPause() {}
  /// Callback called when Window back into focus
  virtual void OnResume() {}
};

}  // namespace Glue
