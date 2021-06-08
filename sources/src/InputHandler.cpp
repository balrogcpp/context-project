// This source file is part of context-project
// Created by Andrew Vasiliev

#include "InputHandler.h"

#include "ComponentLocator.h"

using namespace std;

namespace xio {

//----------------------------------------------------------------------------------------------------------------------
InputHandler::InputHandler() { io_listeners.reserve(32); }

//----------------------------------------------------------------------------------------------------------------------
InputHandler::~InputHandler() {}

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::Pause() { paused_ = true; }

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::Resume() { paused_ = false; }

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::Cleanup() {}

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::Update(float time) {}

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::RegObserver(view_ptr<MutedInputObserver> p) { io_listeners.push_back(p); }

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::UnregObserver(view_ptr<MutedInputObserver> p) {
  auto it = find(io_listeners.begin(), io_listeners.end(), p);

  if (it != io_listeners.end()) {
    iter_swap(it, prev(io_listeners.end()));
    io_listeners.pop_back();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::OnKeyDown(SDL_Keycode sym) {
  if (!paused_) {
    for_each(io_listeners.begin(), io_listeners.end(), [=](auto it) { it->OnKeyDown(sym); });
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::OnKeyUp(SDL_Keycode sym) {
  for_each(io_listeners.begin(), io_listeners.end(), [=](auto it) { it->OnKeyUp(sym); });
}

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::OnTextInput(const char *text) {}

// Mouse

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::OnMouseMove(int dx, int dy) {
  if (!paused_) {
    for_each(io_listeners.begin(), io_listeners.end(), [=](auto it) { it->OnMouseMove(dx, dy); });
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::OnMouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) {
  if (!paused_) {
    for_each(io_listeners.begin(), io_listeners.end(),
             [=](auto it) { it->OnMouseMove(x, y, dx, dy, left, right, middle); });
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::OnMouseWheel(int x, int y) {
  if (!paused_) {
    for_each(io_listeners.begin(), io_listeners.end(), [=](auto it) { it->OnMouseWheel(x, y); });
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::OnMouseLbDown(int x, int y) {
  if (!paused_) {
    for_each(io_listeners.begin(), io_listeners.end(), [=](auto it) { it->OnMouseLbDown(x, y); });
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::OnMouseLbUp(int x, int y) {
  if (!paused_) {
    for_each(io_listeners.begin(), io_listeners.end(), [=](auto it) { it->OnMouseLbUp(x, y); });
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::OnMouseRbDown(int x, int y) {
  if (!paused_) {
    for_each(io_listeners.begin(), io_listeners.end(), [=](auto it) { it->OnMouseRbDown(x, y); });
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::OnMouseRbUp(int x, int y) {
  if (!paused_) {
    for_each(io_listeners.begin(), io_listeners.end(), [=](auto it) { it->OnMouseRbUp(x, y); });
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::OnMouseMbDown(int x, int y) {
  if (!paused_) {
    for_each(io_listeners.begin(), io_listeners.end(), [=](auto it) { it->OnMouseMbDown(x, y); });
  }
}

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::OnMouseMbUp(int x, int y) {
  if (!paused_) {
    for_each(io_listeners.begin(), io_listeners.end(), [=](auto it) { it->OnMouseMbUp(x, y); });
  }
}

//----------------------------------------------------------------------------------------------------------------------
MutedInputObserver::MutedInputObserver() {
  static auto &ref = GetIo();
  ref.RegObserver(this);
}

//----------------------------------------------------------------------------------------------------------------------
MutedInputObserver::~MutedInputObserver() {
  static auto &ref = GetIo();
  ref.UnregObserver(this);
}

}  // namespace xio
