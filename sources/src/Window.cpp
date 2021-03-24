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

#include "pcheader.h"
#include "Window.h"
#include "Exception.h"
#include "SDL2.hpp"
#include <string>

using namespace std;

namespace xio {
//----------------------------------------------------------------------------------------------------------------------
Window::Window(int w, int h, bool f)
	: w_(w), h_(h), f_(f) {
  Init_();
}
//----------------------------------------------------------------------------------------------------------------------
Window::~Window() {
  SDL_SetWindowFullscreen(window_, SDL_FALSE);
  SDL_DestroyWindow(window_);
}

//----------------------------------------------------------------------------------------------------------------------
void Window::Init_() {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
	throw Exception("Failed to init SDL2");
  }

  /* Open the first available controller. */
  SDL_GameController *controller = nullptr;
  for (int i = 0; i < SDL_NumJoysticks(); ++i) {
	if (SDL_IsGameController(i)) {
	  controller = SDL_GameControllerOpen(i);
	  assert(controller & "Could not open gamecontroller");
	}
  }

#else
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_SENSOR) < 0) {
	throw Exception("Failed to init SDL2");
  }
#endif

  SDL_DisplayMode DM;
  SDL_GetCurrentDisplayMode(0, &DM);

  screen_w_ = static_cast<int>(DM.w);
  screen_h_ = static_cast<int>(DM.h);

  if (w_*h_==0.0) {
	f_ = true;
  }

  flags_ |= SDL_WINDOW_ALLOW_HIGHDPI;

  if (w_==screen_w_ && h_==screen_h_) {
	flags_ |= SDL_WINDOW_BORDERLESS;
	f_ = true;
  }

  if (f_) {
	flags_ |= SDL_WINDOW_INPUT_GRABBED;
	flags_ |= SDL_WINDOW_MOUSE_FOCUS;
	flags_ |= SDL_WINDOW_FULLSCREEN_DESKTOP;

	w_ = screen_w_;
	h_ = screen_h_;
  }

  window_ = SDL_CreateWindow(caption_.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w_, h_, flags_);

  if (window_) {
	SDL_SetRelativeMouseMode(SDL_TRUE);
  } else {
	throw Exception("Failed to Init SDL_Window");
  }
}

//----------------------------------------------------------------------------------------------------------------------
string Window::GetCaption() const noexcept {
  return caption_;
}

void Window::SetCaption(const string &caption) {
  caption_ = caption;
  SDL_SetWindowTitle(window_, caption.c_str());
}

pair<int, int> Window::GetSize() const noexcept {
  return make_pair(w_, h_);
}

float Window::GetRatio() const noexcept {
  return static_cast<float>(w_)/h_;
}

bool Window::IsFullscreen() const noexcept {
  return f_;
}

SDL_SysWMinfo Window::GetInfo() const noexcept {
  SDL_SysWMinfo info;
  SDL_VERSION(&info.version);
  SDL_GetWindowWMInfo(window_, &info);
  return info;
}

void Window::SetCursorStatus(bool show, bool grab, bool relative) noexcept {
  SDL_ShowCursor(show);
  SDL_SetWindowGrab(window_, static_cast<SDL_bool>(grab));
  SDL_SetRelativeMouseMode(static_cast<SDL_bool>(relative));
}

void Window::SwapBuffers() const noexcept {
  SDL_GL_SwapWindow(window_);
}

void Window::Resize(int w, int h) noexcept {
  w_ = w;
  h_ = h;
  SDL_SetWindowPosition(window_, (screen_w_ - w_)/2, (screen_h_ - h_)/2);
  SDL_SetWindowSize(window_, w_, h_);
}

void Window::SetFullscreen(bool f) noexcept {
  f_ = f;

  if (f) {
	SDL_SetWindowFullscreen(window_, flags_ | SDL_WINDOW_FULLSCREEN_DESKTOP);
  } else {
	SDL_SetWindowSize(window_, w_, h_);
  }
}

} //namespace
