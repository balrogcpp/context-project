// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "PhysicalInput/InputHandler.h"

using namespace std;

namespace Glue {

InputHandler::InputHandler() { io_listeners.reserve(32); }

InputHandler::~InputHandler() {}

void InputHandler::Pause() { paused_ = true; }

void InputHandler::Resume() { paused_ = false; }

void InputHandler::Cleanup() {}

void InputHandler::Update(float time) {}

void InputHandler::RegObserver(InputObserver *p) { io_listeners.push_back(p); }

void InputHandler::UnregObserver(InputObserver *p) {
  auto it = find(io_listeners.begin(), io_listeners.end(), p);

  if (it != io_listeners.end()) {
    iter_swap(it, prev(io_listeners.end()));
    io_listeners.pop_back();
  }
}

void InputHandler::OnKeyDown(SDL_Keycode sym) {
  if (!paused_) {
    for (auto &it : io_listeners) it->OnKeyDown(sym);
  }
}

void InputHandler::OnKeyUp(SDL_Keycode sym) {
  for (auto &it : io_listeners) it->OnKeyUp(sym);
}

void InputHandler::OnTextInput(const char *text) {}

// Mouse

void InputHandler::OnMouseMove(int dx, int dy) {
  if (!paused_) {
    for (auto &it : io_listeners) it->OnMouseMove(dx, dy);
  }
}

void InputHandler::OnMouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) {
  if (!paused_) {
    for (auto &it : io_listeners) it->OnMouseMove(x, y, dx, dy, left, right, middle);
  }
}

void InputHandler::OnMouseWheel(int x, int y) {
  if (!paused_) {
    for (auto &it : io_listeners) it->OnMouseWheel(x, y);
  }
}

void InputHandler::OnMouseLbDown(int x, int y) {
  if (!paused_) {
    for (auto &it : io_listeners) it->OnMouseLbDown(x, y);
  }
}

void InputHandler::OnMouseLbUp(int x, int y) {
  if (!paused_) {
    for (auto &it : io_listeners) it->OnMouseLbUp(x, y);
  }
}

void InputHandler::OnMouseRbDown(int x, int y) {
  if (!paused_) {
    for (auto &it : io_listeners) it->OnMouseRbDown(x, y);
  }
}

void InputHandler::OnMouseRbUp(int x, int y) {
  if (!paused_) {
    for (auto &it : io_listeners) it->OnMouseRbUp(x, y);
  }
}

void InputHandler::OnMouseMbDown(int x, int y) {
  if (!paused_) {
    for (auto &it : io_listeners) it->OnMouseMbDown(x, y);
  }
}

void InputHandler::OnMouseMbUp(int x, int y) {
  if (!paused_) {
    for (auto &it : io_listeners) it->OnMouseMbUp(x, y);
  }
}

}  // namespace Glue
