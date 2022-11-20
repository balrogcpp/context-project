/// created by Andrey Vasiliev

#pragma once
#include <OgreException.h>
#include <OgreLogManager.h>
#include <SDL2/SDL_messagebox.h>

inline int ErrorWindow(const std::string &title, const std::string &text) noexcept {
  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.c_str(), text.c_str(), nullptr);
  // this creates Singleton error on crash
  //static auto &logManager = Ogre::LogManager::getSingleton();
  //logManager.logError("ErrorWindow: [" + title + "] " + text);
#ifdef __EMSCRIPTEN__
  emscripten_pause_main_loop();
#endif
  return -1;
}

inline void LogError(const std::string &title, const std::string &text) noexcept {
  static auto &logManager = Ogre::LogManager::getSingleton();
  logManager.logError("[" + title + "] " + text);
}

inline void LogError(int number, const std::string &description, const std::string &source, const char *type, const char *file, long line) noexcept {
  static auto &logManager = Ogre::LogManager::getSingleton();

  std::stringstream ss;

  ss << type << ": " << description << " in " << source;

  if (line > 0) {
    ss << " at " << file << " (line " << line << ")";
  }

  logManager.logError(ss.str());
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

#define LOG_ERROR(b) LogError(Ogre::Exception::ERR_RT_ASSERTION_FAILED, b, __FUNCTION__, "RuntimeAssertionException", __FILE__, __LINE__)
#define THROW_EXCEPTION(b) \
  throw Ogre::Exception(Ogre::Exception::ERR_RT_ASSERTION_FAILED, b, __FUNCTION__, "RuntimeAssertionException", __FILE__, __LINE__)


#ifndef NDEBUG
#define ASSERTION(a, b)   \
  if (!(a)) LOG_ERROR(b); \
  OgreAssert(a, b)
#else
#define ASSERTION(a, b)   \
  if (!(a)) LOG_ERROR(b); \
  if (!(a)) THROW_EXCEPTION(b)
#endif
