/// created by Andrey Vasiliev

#pragma once
#include <OgreException.h>
#include <OgreLogManager.h>
#include <SDL2/SDL_messagebox.h>

inline int ErrorWindow(const std::string &title, const std::string &text) noexcept {
  Ogre::LogManager::getSingleton().logError("ErrorWindow: [" + title + "] " + text);
  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.c_str(), text.c_str(), nullptr);
#ifdef __EMSCRIPTEN__
  emscripten_pause_main_loop();
#endif
  return -1;
}

inline void LogError(const std::string &title, const std::string &text) noexcept {
  static auto &logManager = Ogre::LogManager::getSingleton();
  logManager.logError("[" + title + "] " + text);
}

inline void LogWarning(const std::string &title, const std::string &text) noexcept {
  static auto &logManager = Ogre::LogManager::getSingleton();
  logManager.logWarning("[" + title + "] " + text);
}

inline void LogNormal(const std::string &title, const std::string &text) noexcept {
  static auto &logManager = Ogre::LogManager::getSingleton();
  logManager.logMessage("[" + title + "] " + text, Ogre::LML_NORMAL);
}

inline void LogTrivial(const std::string &title, const std::string &text) noexcept {
  static auto &logManager = Ogre::LogManager::getSingleton();
  logManager.logMessage("[" + title + "] " + text, Ogre::LML_TRIVIAL);
}

#define ASSERTION()
