// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "NoCopy.h"
#include <cinttypes>
#include <string>
extern "C" {
#include <SDL2/SDL_syswm.h>
}

namespace Glue {
class Window : public NoCopy {
 public:
  explicit Window(int w, int h, bool f);
  virtual ~Window();

  std::string GetCaption() const;
  void SetCaption(const std::string &caption);
  float GetRatio() const;
  std::pair<int, int> GetSize() const;
  bool IsFullscreen() const;
  SDL_SysWMinfo GetSDLInfo() const;
  void Grab(bool grab);
  void SwapBuffers() const;
  void Resize(int Width, int Height);
  void SetFullscreen(bool f);

 protected:
  void InitGLContext();

  SDL_Window *SDLWindowPtr = nullptr;
  SDL_GLContext SDLGLContextPtr = nullptr;
  uint32_t SDLWindowFlags = 0;
  std::string caption_;
  int WindowWidth = 1024;
  int WindowHeight = 768;
  bool FullScreen = false;
  int ScreenWidth = 0;
  int ScreenHeight = 0;
};

}  // namespace Glue
