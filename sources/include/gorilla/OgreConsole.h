// This source file is part of context-project
// Created by Andrew Vasiliev

#pragma once

#include "Gorilla.h"
#include "Input.h"
#include "Singleton.h"

namespace Gorilla {
typedef void (*OgreConsoleFunctionPtr)(Ogre::StringVector &);

class OgreConsole : public glue::Singleton<OgreConsole>,
                    public Ogre::FrameListener,
                    public Ogre::LogListener,
                    public glue::InputObserver {
 public:
  OgreConsole();

  ~OgreConsole();

  void init(Gorilla::Screen *screen_to_use);
  void shutdown();

  void setVisible(bool mIsVisible);
  bool isVisible() { return mIsVisible; }

  void print(const Ogre::String &text);

  bool frameStarted(const Ogre::FrameEvent &evt) override;
  bool frameEnded(const Ogre::FrameEvent &evt) override;

  void OnKeyDown(SDL_Keycode arg) override;

  void addCommand(const Ogre::String &command, OgreConsoleFunctionPtr);
  void removeCommand(const Ogre::String &command);

  // log
#if OGRE_VERSION_MINOR < 8 && OGRE_VERSION_MAJOR < 2
  void messageLogged(const Ogre::String &message, Ogre::LogMessageLevel lml, bool maskDebug,
                     const Ogre::String &logName);
#else
  // "bool& skip" added in Ogre 1.8
  void messageLogged(const Ogre::String &message, Ogre::LogMessageLevel lml, bool maskDebug,
                     const Ogre::String &logName, bool &skip) override;
#endif
 private:
  void updateConsole();
  void updatePrompt();

  bool mIsVisible;
  bool mIsInitialised;
  Gorilla::Screen *mScreen;
  Gorilla::Layer *mLayer;
  Gorilla::Caption *mPromptText;
  Gorilla::MarkupText *mConsoleText;
  Gorilla::Rectangle *mDecoration;
  Gorilla::GlyphData *mGlyphData;

  bool mUpdateConsole;
  bool mUpdatePrompt;

  unsigned int mStartline;
  std::list<Ogre::String> lines;
  Ogre::String prompt;
  std::map<Ogre::String, OgreConsoleFunctionPtr> commands;
};

}  // namespace Gorilla
