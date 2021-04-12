//MIT License
//
//Copyright (c) 2021 Andrew Vasiliev
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#include "InputHandler.h"
#include "ComponentLocator.h"

using namespace std;

namespace xio {

InputHandler::InputHandler() {
  io_listeners.reserve(32);
}

//----------------------------------------------------------------------------------------------------------------------
InputHandler::~InputHandler() {

}

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::Pause() {
  paused_ = true;
}

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::Resume() {
  paused_ = false;
}

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::Cleanup() {

}

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::Update(float time) {

}

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::RegObserver(view_ptr<MutedInputObserver> p) {
  io_listeners.push_back(p);
}

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::UnregObserver(view_ptr<MutedInputObserver> p) {
  auto it = find(io_listeners.begin(), io_listeners.end(), p);

  if (it!=io_listeners.end()) {
	iter_swap(it, prev(io_listeners.end()));
	io_listeners.pop_back();
  }
}


//----------------------------------------------------------------------------------------------------------------------
void InputHandler::OnKeyDown(SDL_Keycode sym) {
  //if(!paused_)
  for_each(io_listeners.begin(), io_listeners.end(), [=](auto it){it->OnKeyDown(sym);});
}

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::OnKeyUp(SDL_Keycode sym) {
  //if(!paused_)
  for_each(io_listeners.begin(), io_listeners.end(), [=](auto it){it->OnKeyUp(sym);});
}

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::OnTextInput(const char *text) {

}

//Mouse

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::OnMouseMove(int dx, int dy) {
  if(!paused_)
  for_each(io_listeners.begin(), io_listeners.end(), [=](auto it){it->OnMouseMove(dx,dy);});
}

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::OnMouseMove(int x, int y, int dx, int dy, bool left, bool right, bool middle) {
  if(!paused_)
  for_each(io_listeners.begin(), io_listeners.end(), [=](auto it){it->OnMouseMove(x, y, dx,dy, left, right, middle);});
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T> static int sign(T val) {
  return (T(0) < val) - (val < T(0));
}

void InputHandler::OnMouseWheel(int x, int y) {
  if(!paused_)
  for_each(io_listeners.begin(), io_listeners.end(), [=](auto it){it->OnMouseWheel(x, y);});
}

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::OnMouseLbDown(int x, int y) {
  if(!paused_)
  for_each(io_listeners.begin(), io_listeners.end(), [=](auto it){it->OnMouseLbDown(x, y);});
}

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::OnMouseLbUp(int x, int y) {
  if(!paused_)
  for_each(io_listeners.begin(), io_listeners.end(), [=](auto it){it->OnMouseLbUp(x, y);});
}

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::OnMouseRbDown(int x, int y) {
  if(!paused_)
  for_each(io_listeners.begin(), io_listeners.end(), [=](auto it){it->OnMouseRbDown(x, y);});
}

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::OnMouseRbUp(int x, int y) {
  for_each(io_listeners.begin(), io_listeners.end(), [=](auto it){it->OnMouseRbUp(x, y);});
}

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::OnMouseMbDown(int x, int y) {
  if(!paused_)
  for_each(io_listeners.begin(), io_listeners.end(), [=](auto it){it->OnMouseMbDown(x, y);});
}

//----------------------------------------------------------------------------------------------------------------------
void InputHandler::OnMouseMbUp(int x, int y) {
  if(!paused_)
  for_each(io_listeners.begin(), io_listeners.end(), [=](auto it){it->OnMouseMbUp(x, y);});
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

} //namespace
