// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Components/Component.h"
#include "InputObserver.h"
#include "LazySingleton.h"
#include <vector>

namespace Glue {

class InputHandler : public InputObserver, public Singleton<InputHandler> {
 public:
  InputHandler();
  virtual ~InputHandler();

  void RegObserver(InputObserver* p);
  void UnregObserver(InputObserver* p);

  void Pause();
  void Resume();
  void Cleanup();
  void Update(float time);

  // These methods are used to turn off inputs in for some listeners by request

  // Keyboard
  void OnKeyDown(SDL_Keycode sym) override;
  void OnKeyUp(SDL_Keycode sym) override;
  void OnTextInput(const char* text) override;

  // Mouse
  void OnMouseMove(int dx, int dy) override;
  void OnMouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) override;
  void OnMouseWheel(int x, int y) override;
  void OnMouseLbDown(int x, int y) override;
  void OnMouseLbUp(int x, int y) override;
  void OnMouseRbDown(int x, int y) override;
  void OnMouseRbUp(int x, int y) override;
  void OnMouseMbDown(int x, int y) override;
  void OnMouseMbUp(int x, int y) override;

 protected:
  std::vector<InputObserver*> io_listeners;
  bool paused_ = false;
};

}  // namespace Glue
