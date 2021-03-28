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

#pragma once

#include "NoCopy.h"

extern "C" {
#include <SDL2/SDL_syswm.h>
}

#include <string>
#include <cinttypes>

namespace xio {
class Window : public NoCopy {
 public:
  explicit Window(int w, int h, bool f);
  virtual ~Window();

 private:

  SDL_Window *window_ = nullptr;
  uint32_t flags_ = 0;
  SDL_GLContext context_ = nullptr;
  std::string caption_;
  bool f_ = false;
  int w_ = 1024;
  int h_ = 768;
  int screen_w_;
  int screen_h_;

 public:
  std::string GetCaption() const;

  void SetCaption(const std::string &caption);

  float GetRatio() const;

  std::pair<int, int> GetSize() const;

  bool IsFullscreen() const;

  SDL_SysWMinfo GetInfo() const;

  void SetCursorStatus(bool show, bool grab, bool relative);

  void SwapBuffers() const;

  void Resize(int w, int h);

  void SetFullscreen(bool f);
};

} //namespace
