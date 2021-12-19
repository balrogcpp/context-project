// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Exception.h"
#include "LazySingleton.h"
#include "NoCopy.h"
#include <exception>
#include <set>
#include <string>
extern "C" {
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
}

namespace Glue {
class InputSequencer;
class InputObserver;
class WindowObserver;
}  // namespace Glue

namespace Glue {

/// This Singleton class is main part of Observer implementation
/// Keeps listeners list, capture inputs and send messages every frame
class InputSequencer : public LazySingleton<InputSequencer> {
 public:
  InputSequencer();
  virtual ~InputSequencer();

  /// Register physical input listener
  void RegObserver(InputObserver *p);

  /// Un-Register physical input listener
  void UnregObserver(InputObserver *p);

  /// Register SDLWindowPtr input listener
  void RegWinObserver(WindowObserver *p);

  /// Un-Register SDLWindowPtr input listener
  void UnregWinObserver(WindowObserver *p);

  /// Called once per frame, sent callback message to listeners
  void Capture();

 protected:
  /// Listeners list (physical input)
  std::set<InputObserver *> io_listeners;
  /// Listeners list (SDLWindowPtr input)
  std::set<WindowObserver *> win_listeners;
  /// Required for Android/IOS
  int HandleAppEvents(void *userdata, SDL_Event *event);
};

}  // namespace Glue
