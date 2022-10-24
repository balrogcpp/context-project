/// created by Andrey Vasiliev

#pragma once

#include "Platform.h"
#include <SDL2/SDL_messagebox.h>
#include <cassert>
#include <stdexcept>

inline int ErrorWindow(const char *caption, const char *text) {
  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, caption, text, nullptr);
#ifdef EMSCRIPTEN
  emscripten_pause_main_loop();
#endif
  return -1;
}
