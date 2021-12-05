// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Window.h"
#include "Exception.h"

using namespace std;

namespace Glue {

void Window::InitGLContext() {
  constexpr array<pair<int, int>, 10> ver = {make_pair(4, 5), make_pair(4, 4), make_pair(4, 3), make_pair(4, 2),
                                             make_pair(4, 1), make_pair(4, 0), make_pair(3, 3), make_pair(3, 2),
                                             make_pair(3, 1), make_pair(3, 0)};

  if (!SDLGLContextPtr) {
    for (const auto &it : ver) {
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, it.first);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, it.second);

      SDLGLContextPtr = SDL_GL_CreateContext(SDLWindowPtr);

      if (SDLGLContextPtr) {
        break;
      }
    }
  }

  if (!SDLGLContextPtr) {
    throw Exception("Failed to Create SDL_GL_Context");
  }
}

Window::Window(int w, int h, bool f) : WindowWidth(w), WindowHeight(h), FullScreen(f) {
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
  ScreenWidth = static_cast<int>(DM.w);
  ScreenHeight = static_cast<int>(DM.h);

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID

  if (WindowWidth == ScreenWidth && WindowHeight == ScreenHeight) {
    SDLWindowFlags |= SDL_WINDOW_BORDERLESS;
  }

  if (FullScreen) {
    SDLWindowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    SDLWindowFlags |= SDL_WINDOW_BORDERLESS;
    WindowWidth = ScreenWidth;
    WindowHeight = ScreenHeight;
  }

  SDLWindowPtr = SDL_CreateWindow(caption_.c_str(), SDL_WINDOWPOS_UNDEFINED_DISPLAY(0), SDL_WINDOWPOS_UNDEFINED_DISPLAY(0),
                            WindowWidth, WindowHeight, SDLWindowFlags);

#else

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

  SDLWindowFlags |= SDL_WINDOW_BORDERLESS;
  SDLWindowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
  SDLWindowFlags |= SDL_WINDOW_OPENGL;

  WindowWidth = ScreenWidth;
  WindowHeight = ScreenHeight;

  SDLWindowPtr = SDL_CreateWindow(nullptr, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ScreenWidth, ScreenHeight, SDLWindowFlags);
  SDLGLContextPtr = SDL_GL_CreateContext(SDLWindowPtr);

#endif
}

Window::~Window() {
  SDL_SetWindowFullscreen(SDLWindowPtr, SDL_FALSE);
  SDL_DestroyWindow(SDLWindowPtr);
}

string Window::GetCaption() const { return caption_; }

void Window::SetCaption(const string &caption) {
  caption_ = caption;
  SDL_SetWindowTitle(SDLWindowPtr, caption.c_str());
}

pair<int, int> Window::GetSize() const { return make_pair(WindowWidth, WindowHeight); }

float Window::GetRatio() const { return static_cast<float>(WindowWidth) / static_cast<float>(WindowHeight); }

bool Window::IsFullscreen() const { return FullScreen; }

SDL_SysWMinfo Window::GetSDLInfo() const {
  SDL_SysWMinfo info;
  SDL_VERSION(&info.version);
  SDL_GetWindowWMInfo(SDLWindowPtr, &info);
  return info;
}

void Window::Grab(bool grab) {
  // This breaks input on > Android 9.0
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  SDL_ShowCursor(!grab);
  SDL_SetWindowGrab(SDLWindowPtr, static_cast<SDL_bool>(grab));
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE
  // osx workaround: mouse motion events are gone otherwise
  SDL_SetRelativeMouseMode(static_cast<SDL_bool>(grab));
#endif
#endif
}

void Window::SwapBuffers() const { SDL_GL_SwapWindow(SDLWindowPtr); }

void Window::Resize(int Width, int Height) {
  WindowWidth = Width;
  WindowHeight = Height;
  SDL_SetWindowPosition(SDLWindowPtr, (ScreenWidth - WindowWidth) / 2, (ScreenHeight - WindowHeight) / 2);
  SDL_SetWindowSize(SDLWindowPtr, WindowWidth, WindowHeight);
}

void Window::SetFullscreen(bool f) {
  FullScreen = f;

  if (f) {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
    SDL_SetWindowFullscreen(SDLWindowPtr, SDLWindowFlags | SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN_DESKTOP);
#else
    SDL_SetWindowFullscreen(SDLWindowPtr, SDLWindowFlags | SDL_WINDOW_FULLSCREEN);
#endif
  } else {
    SDL_SetWindowSize(SDLWindowPtr, WindowWidth, WindowHeight);
  }
}

}  // namespace Glue
