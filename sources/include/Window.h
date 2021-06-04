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

#pragma once

#include "NoCopy.h"

extern "C" {
#include <SDL2/SDL_syswm.h>
}

#include <cinttypes>
#include <string>

namespace xio {
class Window : public NoCopy {
 public:
  explicit Window(int w, int h, bool f);
  virtual ~Window();

 public:
  std::string GetCaption() const;
  void SetCaption(const std::string &caption);
  float GetRatio() const;
  std::pair<int, int> GetSize() const;
  bool IsFullscreen() const;
  SDL_SysWMinfo GetInfo() const;
  void Grab(bool grab);
  void SwapBuffers() const;
  void Resize(int w, int h);
  void SetFullscreen(bool f);

 private:
  void InitGlContext_();

  SDL_Window *window_ = nullptr;
  SDL_GLContext gl_context_ = nullptr;

  uint32_t flags_ = 0;
  std::string caption_;
  int w_ = 1024;
  int h_ = 768;
  bool f_ = false;
  int screen_w_;
  int screen_h_;
};

}  // namespace xio
