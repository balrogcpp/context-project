/*
MIT License

Copyright (c) 2020 Andrey Vasiliev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "Exception.h"
#include "NoCopy.h"

extern "C" {
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
}

#include <string>
#include <cinttypes>

namespace Context {
class Window : public NoCopy {
 public:
//----------------------------------------------------------------------------------------------------------------------
  Window() {
    Init_();
  }

  explicit Window(int32_t w, int32_t h) : w_(w), h_(h) {
    Init_();
  }
//----------------------------------------------------------------------------------------------------------------------
  virtual ~Window() {
    if (f_)
      SDL_SetWindowFullscreen(window_, SDL_FALSE);
    SDL_DestroyWindow(window_);
  }

 private:
//----------------------------------------------------------------------------------------------------------------------
  void Init_() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0)
      throw Exception("Failed to init SDL2");

    if (SDL_NumJoysticks() != 0) {
      for (int i = 0; i < SDL_NumJoysticks(); i++) {
        if (SDL_IsGameController(i)) {
          SDL_GameController *controller = nullptr;

          controller = SDL_GameControllerOpen(i);

          break;
        }
      }
    }

    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);

    screen_w_ = static_cast<int>(DM.w);
    screen_h_ = static_cast<int>(DM.h);

    bool invalid = (screen_w_ * screen_h_) == 0;

    if (invalid) {
      w_ = screen_w_;
      h_ = screen_h_;
      f_ = true;
    }

    bool factual_fullscreen = (w_ == screen_w_ && h_ == screen_h_);

    flags_ |= SDL_WINDOW_ALLOW_HIGHDPI;

    if (factual_fullscreen)
      flags_ |= SDL_WINDOW_BORDERLESS;

    if (f_) {
      flags_ |= SDL_WINDOW_ALWAYS_ON_TOP;
      flags_ |= SDL_WINDOW_INPUT_GRABBED;
      flags_ |= SDL_WINDOW_MOUSE_FOCUS;
    }

    window_ = SDL_CreateWindow(caption_.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w_, h_, flags_);

    if (window_) {
      SDL_SetRelativeMouseMode(SDL_TRUE);
    } else {
      throw Exception("Failed to Create SDL_Window");
    }

    if (gl_force_) {
      constexpr std::pair<int, int> versions[]{{4, 5}, {4, 4}, {4, 3}, {4, 2}, {4, 1},
                                               {4, 0}, {3, 3}, {3, 2}, {3, 1}, {3, 0}};

      if (gl_force_) {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor);

        context_ = SDL_GL_CreateContext(window_);

        if (!context_) {
          for (const auto &it : versions) {
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, it.first);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, it.second);

            context_ = SDL_GL_CreateContext(window_);

            if (!context_) {
              continue;
            } else {
              gl_major = it.first;
              gl_minor = it.second;
              break;
            }
          }
        }
      } else {
        for (const auto &it : versions) {
          SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
          SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, it.first);
          SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, it.second);

          context_ = SDL_GL_CreateContext(window_);

          if (!context_) {
            continue;
          } else {
            gl_major = it.first;
            gl_minor = it.second;
            break;
          }
        }
      }

      if (!context_)
        throw Exception("Failed to Create SDL_GL_Context");
    }

    if (window_ && f_) {
      SDL_SetWindowSize(window_, screen_w_, screen_h_);
      SDL_SetWindowFullscreen(window_, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
  }

//----------------------------------------------------------------------------------------------------------------------
  SDL_Window *window_ = nullptr;
  uint32_t flags_ = 0;
  SDL_GLContext context_ = nullptr;
  std::string caption_ = "My Demo";
  bool f_ = false;
  int32_t w_ = 1024;
  int32_t h_ = 768;
  int32_t screen_w_;
  int32_t screen_h_;
  bool gl_force_ = false;
  int32_t gl_minor = 3;
  int32_t gl_major = 3;

 public:
//----------------------------------------------------------------------------------------------------------------------
  inline std::string GetCaption() const noexcept {
    return caption_;
  }

  inline std::pair<uint32_t, uint32_t> GetSize() const noexcept {
    return std::make_pair(w_, h_);
  }

  inline float GetRatio() const noexcept {
    return static_cast<float>(w_) / h_;
  }

  inline bool GetFullscreen() const noexcept {
    return f_;
  }

  inline SDL_SysWMinfo GetInfo() const noexcept {
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    SDL_GetWindowWMInfo(window_, &info);
    return info;
  }

  inline void UpdateCursor(bool show, bool grab, bool relative) noexcept {
    SDL_ShowCursor(show);
    SDL_SetWindowGrab(window_, static_cast<SDL_bool>(grab));
    SDL_SetRelativeMouseMode(static_cast<SDL_bool>(relative));
  }

  inline void SwapBuffers() const noexcept {
    SDL_GL_SwapWindow(window_);
  }

  inline void Resize(int32_t w, int32_t h) noexcept {
    w_ = w;
    h_ = h;
    SDL_SetWindowPosition(window_, (screen_w_ - w_) / 2, (screen_h_ - h_) / 2);
    SDL_SetWindowSize(window_, w_, h_);
  }

  inline void Fullscreen(bool f) noexcept {
    f_ = f;
    if (f) {
      w_ = screen_w_;
      h_ = screen_h_;
      SDL_SetWindowSize(window_, screen_w_, screen_h_);
      SDL_SetWindowFullscreen(window_, flags_ | SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
      SDL_SetWindowFullscreen(window_, flags_);
      SDL_SetWindowSize(window_, w_, h_);
    }
  }
}; //class Window
}
