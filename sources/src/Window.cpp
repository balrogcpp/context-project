// MIT License
//
// Copyright (c) 2021 Andrew Vasiliev
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "Window.h"

#include "Exception.h"
#include "pcheader.h"

using namespace std;

namespace xio {

//----------------------------------------------------------------------------------------------------------------------
void Window::InitGlContext_() {
  constexpr array<pair<int, int>, 10> ver = {make_pair(4, 5), make_pair(4, 4), make_pair(4, 3), make_pair(4, 2),
                                             make_pair(4, 1), make_pair(4, 0), make_pair(3, 3), make_pair(3, 2),
                                             make_pair(3, 1), make_pair(3, 0)};

  if (!gl_context_) {
    for (const auto &it : ver) {
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, it.first);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, it.second);

      gl_context_ = SDL_GL_CreateContext(window_);

      if (gl_context_) {
        break;
      }
    }
  }

  if (!gl_context_) {
    throw Exception("Failed to Create SDL_GL_Context");
  }
}

//----------------------------------------------------------------------------------------------------------------------
Window::Window(int w, int h, bool f) : w_(w), h_(h), f_(f) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
    throw Exception("Failed to init SDL2");
  }

  for (int i = 0; i < SDL_NumJoysticks(); ++i) {
    if (SDL_IsGameController(i)) {
      SDL_GameControllerOpen(i);
    }
  }

  SDL_DisplayMode DM;
  SDL_GetDesktopDisplayMode(0, &DM);
  screen_w_ = static_cast<int>(DM.w);
  screen_h_ = static_cast<int>(DM.h);

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID

  if (w_ == screen_w_ && h_ == screen_h_) {
    flags_ |= SDL_WINDOW_BORDERLESS;
  }

  if (f_) {
    flags_ |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    flags_ |= SDL_WINDOW_BORDERLESS;
    w_ = screen_w_;
    h_ = screen_h_;
  }

  window_ = SDL_CreateWindow(caption_.c_str(), SDL_WINDOWPOS_UNDEFINED_DISPLAY(0), SDL_WINDOWPOS_UNDEFINED_DISPLAY(0),
                             w_, h_, flags_);

#else

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

  flags_ |= SDL_WINDOW_BORDERLESS;
  flags_ |= SDL_WINDOW_FULLSCREEN_DESKTOP;
  flags_ |= SDL_WINDOW_OPENGL;

  w_ = screen_w_;
  h_ = screen_h_;

  window_ = SDL_CreateWindow(nullptr, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_w_, screen_h_, flags_);
  gl_context_ = SDL_GL_CreateContext(window_);

#endif
}

//----------------------------------------------------------------------------------------------------------------------
Window::~Window() {
  SDL_SetWindowFullscreen(window_, SDL_FALSE);
  SDL_DestroyWindow(window_);
}

//----------------------------------------------------------------------------------------------------------------------
string Window::GetCaption() const { return caption_; }

//----------------------------------------------------------------------------------------------------------------------
void Window::SetCaption(const string &caption) {
  caption_ = caption;
  SDL_SetWindowTitle(window_, caption.c_str());
}

//----------------------------------------------------------------------------------------------------------------------
pair<int, int> Window::GetSize() const { return make_pair(w_, h_); }

//----------------------------------------------------------------------------------------------------------------------
float Window::GetRatio() const { return static_cast<float>(w_) / static_cast<float>(h_); }

//----------------------------------------------------------------------------------------------------------------------
bool Window::IsFullscreen() const { return f_; }

//----------------------------------------------------------------------------------------------------------------------
SDL_SysWMinfo Window::GetInfo() const {
  SDL_SysWMinfo info;
  SDL_VERSION(&info.version);
  SDL_GetWindowWMInfo(window_, &info);
  return info;
}

//----------------------------------------------------------------------------------------------------------------------
void Window::Grab(bool grab) {
  // This breaks input on > Android 9.0
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  SDL_ShowCursor(!grab);
  SDL_SetWindowGrab(window_, static_cast<SDL_bool>(grab));
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE
  // osx workaround: mouse motion events are gone otherwise
  SDL_SetRelativeMouseMode(static_cast<SDL_bool>(grab));
#endif
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void Window::SwapBuffers() const { SDL_GL_SwapWindow(window_); }

//----------------------------------------------------------------------------------------------------------------------
void Window::Resize(int w, int h) {
  w_ = w;
  h_ = h;
  SDL_SetWindowPosition(window_, (screen_w_ - w_) / 2, (screen_h_ - h_) / 2);
  SDL_SetWindowSize(window_, w_, h_);
}

//----------------------------------------------------------------------------------------------------------------------
void Window::SetFullscreen(bool f) {
  f_ = f;

  if (f) {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
    SDL_SetWindowFullscreen(window_, flags_ | SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN_DESKTOP);
#else
    SDL_SetWindowFullscreen(window_, flags_ | SDL_WINDOW_FULLSCREEN);
#endif
  } else {
    SDL_SetWindowSize(window_, w_, h_);
  }
}

}  // namespace xio
