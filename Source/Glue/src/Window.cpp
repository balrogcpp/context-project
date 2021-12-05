// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Window.h"
#include "Exception.h"

using namespace std;

namespace Glue {

void Window::InitGlContext() {
  constexpr array<pair<int, int>, 10> ver = {make_pair(4, 5), make_pair(4, 4), make_pair(4, 3), make_pair(4, 2),
                                             make_pair(4, 1), make_pair(4, 0), make_pair(3, 3), make_pair(3, 2),
                                             make_pair(3, 1), make_pair(3, 0)};

  if (!gl_context) {
    for (const auto &it : ver) {
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, it.first);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, it.second);

      gl_context = SDL_GL_CreateContext(window);

      if (gl_context) {
        break;
      }
    }
  }

  if (!gl_context) {
    throw Exception("Failed to Create SDL_GL_Context");
  }
}

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

  window = SDL_CreateWindow(caption_.c_str(), SDL_WINDOWPOS_UNDEFINED_DISPLAY(0), SDL_WINDOWPOS_UNDEFINED_DISPLAY(0),
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

  window = SDL_CreateWindow(nullptr, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_w_, screen_h_, flags_);
  gl_context = SDL_GL_CreateContext(window);

#endif
}

Window::~Window() {
  SDL_SetWindowFullscreen(window, SDL_FALSE);
  SDL_DestroyWindow(window);
}

string Window::GetCaption() const { return caption_; }

void Window::SetCaption(const string &caption) {
  caption_ = caption;
  SDL_SetWindowTitle(window, caption.c_str());
}

pair<int, int> Window::GetSize() const { return make_pair(w_, h_); }

float Window::GetRatio() const { return static_cast<float>(w_) / static_cast<float>(h_); }

bool Window::IsFullscreen() const { return f_; }

SDL_SysWMinfo Window::GetInfo() const {
  SDL_SysWMinfo info;
  SDL_VERSION(&info.version);
  SDL_GetWindowWMInfo(window, &info);
  return info;
}

void Window::Grab(bool grab) {
  // This breaks input on > Android 9.0
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  SDL_ShowCursor(!grab);
  SDL_SetWindowGrab(window, static_cast<SDL_bool>(grab));
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE
  // osx workaround: mouse motion events are gone otherwise
  SDL_SetRelativeMouseMode(static_cast<SDL_bool>(grab));
#endif
#endif
}

void Window::SwapBuffers() const { SDL_GL_SwapWindow(window); }

void Window::Resize(int w, int h) {
  w_ = w;
  h_ = h;
  SDL_SetWindowPosition(window, (screen_w_ - w_) / 2, (screen_h_ - h_) / 2);
  SDL_SetWindowSize(window, w_, h_);
}

void Window::SetFullscreen(bool f) {
  f_ = f;

  if (f) {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
    SDL_SetWindowFullscreen(window, flags_ | SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN_DESKTOP);
#else
    SDL_SetWindowFullscreen(window, flags_ | SDL_WINDOW_FULLSCREEN);
#endif
  } else {
    SDL_SetWindowSize(window, w_, h_);
  }
}

}  // namespace Glue
