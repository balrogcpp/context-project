// This source file is part of "glue project". Created by Andrey Vasiliev

#include "PCHeader.h"
#include "Input/InputObserverImpl.h"

namespace Glue {

void InputObserverImpl::OnKeyDown(SDL_Keycode sym) {
  // TODO fill this
}

void InputObserverImpl::OnKeyUp(SDL_Keycode sym) {
  // TODO fill this
}

void InputObserverImpl::OnMouseMove(int dx, int dy) {
  // TODO fill this
}

void InputObserverImpl::OnMouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) {
  // TODO fill this
}

void InputObserverImpl::OnMouseWheel(int x, int y) {
  // TODO fill this
}

void InputObserverImpl::OnMouseLbDown(int x, int y) {
  // TODO fill this
}

void InputObserverImpl::OnMouseLbUp(int x, int y) {
  // TODO fill this
}

void InputObserverImpl::OnMouseRbDown(int x, int y) {
  // TODO fill this
}

void InputObserverImpl::OnMouseRbUp(int x, int y) {
  // TODO fill this
}

void InputObserverImpl::OnMouseMbDown(int x, int y) {
  // TODO fill this
}

void InputObserverImpl::OnMouseMbUp(int x, int y) {
  // TODO fill this
}

void InputObserverImpl::OnTextInput(const char *text) {
  // TODO fill this
}

void InputObserverImpl::OnGamepadAxis(int which, int axis, int value) {
  // TODO fill this
}

void InputObserverImpl::OnGamepadBtDown(int which, int button) {
  // TODO fill this
}

void InputObserverImpl::OnGamepadBtUp(int which, int button) {
  // TODO fill this
}

void InputObserverImpl::OnGamepadHat(int which, int hat, int value) {
  // TODO fill this
}

void InputObserverImpl::OnGamepadBall(int which, int ball, int xrel, int yrel) {
  // TODO fill this
}

}  // namespace Glue
