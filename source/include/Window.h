// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "NoCopy.h"
#include <cinttypes>
#include <string>
extern "C" {
#include <SDL2/SDL_syswm.h>
}

namespace glue {
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

 protected:
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

}  // namespace glue
