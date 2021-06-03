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

#include "Singleton.h"
#include "Input.h"
#include "Gorilla.h"

namespace Gorilla {
typedef void (*OgreConsoleFunctionPtr)(Ogre::StringVector &);

class OgreConsole
	: public xio::Singleton<OgreConsole>,
	  public Ogre::FrameListener,
	  public Ogre::LogListener,
	  public xio::InputObserver {

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

  //log
#if OGRE_VERSION_MINOR < 8 && OGRE_VERSION_MAJOR < 2
  void messageLogged(const Ogre::String &message,
					 Ogre::LogMessageLevel lml,
					 bool maskDebug,
					 const Ogre::String &logName);
#else
  // "bool& skip" added in Ogre 1.8
  void messageLogged(const Ogre::String &message,
					 Ogre::LogMessageLevel lml,
					 bool maskDebug,
					 const Ogre::String &logName,
					 bool &skip) override;
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

}
