//MIT License
//
//Copyright (c) 2020 Andrey Vasiliev
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

#include "Input.h"
#include <algorithm>

using namespace std;

namespace xio {
//----------------------------------------------------------------------------------------------------------------------
InputSequencer::InputSequencer() {
  if (instanced_) {
	throw Exception("Only one instance of InputSequencer is allowed!\n");
  }

  Reserve(128);
  instanced_ = true;
}

InputSequencer::~InputSequencer() {}

//----------------------------------------------------------------------------------------------------------------------
InputSequencer &InputSequencer::GetInstance() {
  static InputSequencer instance; // Guaranteed to be destroyed.
  // Instantiated on first use.
  return instance;
}

//----------------------------------------------------------------------------------------------------------------------
void InputSequencer::RegObserver(view_ptr<InputObserver> p) {
  io_listeners.push_back(p);
}
//----------------------------------------------------------------------------------------------------------------------
void InputSequencer::UnregObserver(view_ptr<InputObserver> p) {
  auto it = find(io_listeners.begin(), io_listeners.end(), p);

  if (it!=io_listeners.end()) {
	iter_swap(it, prev(io_listeners.end()));
	io_listeners.pop_back();
  }
}
//----------------------------------------------------------------------------------------------------------------------
void InputSequencer::RegWinObserver(view_ptr<WindowObserver> p) {
  win_listeners.push_back(p);
}
//----------------------------------------------------------------------------------------------------------------------
void InputSequencer::UnregWinObserver(view_ptr<WindowObserver> p) {
  auto it = find(win_listeners.begin(), win_listeners.end(), p);

  if (it!=win_listeners.end()) {
	iter_swap(it, prev(win_listeners.end()));
	win_listeners.pop_back();
  }
}
//----------------------------------------------------------------------------------------------------------------------
void InputSequencer::Clear() {
  io_listeners.clear();
  io_listeners.shrink_to_fit();
  win_listeners.clear();
  win_listeners.shrink_to_fit();
}
//----------------------------------------------------------------------------------------------------------------------
void InputSequencer::Reserve(size_t size) {
  io_listeners.reserve(size);
  win_listeners.reserve(size);
}
//----------------------------------------------------------------------------------------------------------------------
void InputSequencer::Capture() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {

	switch (event.type) {
	  case SDL_KEYUP: {
		for (auto it : io_listeners)
		  it->OnKeyUp(event.key.keysym.sym);
		break;
	  }
	  case SDL_KEYDOWN: {
		for (auto it : io_listeners)
		  it->OnKeyDown(event.key.keysym.sym);
		break;
	  }
	  case SDL_MOUSEMOTION: {
		for (auto it : io_listeners) {
		  it->OnMouseMove(event.motion.x, event.motion.y,
						  event.motion.xrel, event.motion.yrel,
						  (event.motion.state & SDL_BUTTON(SDL_BUTTON_LEFT))!=0,
						  (event.motion.state & SDL_BUTTON(SDL_BUTTON_RIGHT))!=0,
						  (event.motion.state & SDL_BUTTON(SDL_BUTTON_MIDDLE))!=0);
		  it->OnMouseMove(event.motion.xrel, event.motion.yrel);
		}
		break;
	  }
	  case SDL_MOUSEBUTTONDOWN: {
		switch (event.button.button) {
		  case SDL_BUTTON_LEFT: {
			for (auto it : io_listeners)
			  it->OnMouseLbDown(event.button.x, event.button.y);
			break;
		  }
		  case SDL_BUTTON_RIGHT: {
			for (auto it : io_listeners)
			  it->OnMouseRbDown(event.button.x, event.button.y);
			break;
		  }
		  case SDL_BUTTON_MIDDLE: {
			for (auto it : io_listeners)
			  it->OnMouseMbDown(event.button.x, event.button.y);
			break;
		  }
		}
		break;
	  }
	  case SDL_MOUSEBUTTONUP: {
		switch (event.button.button) {
		  case SDL_BUTTON_LEFT: {
			for (auto it : io_listeners)
			  it->OnMouseLbUp(event.button.x, event.button.y);
			break;
		  }
		  case SDL_BUTTON_RIGHT: {
			for (auto it : io_listeners)
			  it->OnMouseRbUp(event.button.x, event.button.y);
			break;
		  }
		  case SDL_BUTTON_MIDDLE: {
			for (auto it : io_listeners)
			  it->OnMouseMbUp(event.button.x, event.button.y);
			break;
		  }
		}
		break;
	  }

	  case SDL_MOUSEWHEEL: {
		for (auto it : io_listeners)
		  it->OnMouseWheel(event.wheel.x, event.wheel.y);
		break;
	  }

	  case SDL_JOYAXISMOTION: {
		for (auto it : io_listeners)
		  it->OnJoystickAxis(event.jaxis.which, event.jaxis.axis, event.jaxis.value);
		break;
	  }
	  case SDL_JOYBALLMOTION: {
		for (auto it : io_listeners)
		  it->OnJoystickBall(event.jball.which, event.jball.ball, event.jball.xrel, event.jball.yrel);
		break;
	  }
	  case SDL_JOYHATMOTION: {
		for (auto it : io_listeners)
		  it->OnJoystickHat(event.jhat.which, event.jhat.hat, event.jhat.value);
		break;
	  }
	  case SDL_JOYBUTTONDOWN: {
		for (auto it : io_listeners)
		  it->OnJoystickBtDown(event.jbutton.which, event.jbutton.button);
		break;
	  }
	  case SDL_JOYBUTTONUP: {
		for (auto it : io_listeners) {
		  if (it)
			it->OnJoystickBtUp(event.jbutton.which, event.jbutton.button);
		}
		break;
	  }
	  case SDL_QUIT: {
		for (auto it : win_listeners)
		  it->Quit();
		break;
	  }
	  default: {
		for (auto it : win_listeners)
		  it->Event(event);
	  }
	}
  }
}

InputObserver::InputObserver() {
  static auto &ref = InputSequencer::GetInstance();
  ref.RegObserver(this);
}

InputObserver::~InputObserver() {
  static auto &ref = InputSequencer::GetInstance();
  ref.UnregObserver(this);
}

WindowObserver::WindowObserver() {
  static auto &ref = InputSequencer::GetInstance();
  ref.RegWinObserver(this);
}

WindowObserver::~WindowObserver() {
  static auto &ref = InputSequencer::GetInstance();
  ref.UnregWinObserver(this);
}

} //namespace
