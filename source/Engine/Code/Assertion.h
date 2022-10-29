/// created by Andrey Vasiliev

#pragma once
#include <OgreException.h>
#include <OgreLogManager.h>
#include <SDL2/SDL_messagebox.h>
#include <cassert>
#include <stdexcept>

inline int ErrorWindow(const std::string &caption, const std::string &text) {
  Ogre::LogManager::getSingleton().logError("[" + caption + "] " + text);
  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, caption.c_str(), text.c_str(), nullptr);
#ifdef __EMSCRIPTEN__
  emscripten_pause_main_loop();
#endif
  return -1;
}
