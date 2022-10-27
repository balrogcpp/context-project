/// created by Andrey Vasiliev

#pragma once

#include <OgreException.h>
#include <OgreLogManager.h>
#include <SDL2/SDL_messagebox.h>
#include <cassert>
#include <stdexcept>

inline int ErrorWindow(const char *caption, const char *text) {
  Ogre::LogManager::getSingleton().logError(std::string("[") + caption + "] " + text);
  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, caption, text, nullptr);
#ifdef __EMSCRIPTEN__
  emscripten_pause_main_loop();
#endif
  return -1;
}
