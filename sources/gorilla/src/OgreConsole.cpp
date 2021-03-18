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

#include "OgreConsole.h"

#define CONSOLE_FONT_INDEX 14

#define CONSOLE_LINE_LENGTH 85
#define CONSOLE_LINE_COUNT 15
static const unsigned char
	legalchars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890+!\"'#%&/()=?[]\\*-_.:,; ";

namespace Gorilla {

OgreConsole::OgreConsole()
	: mIsVisible(true), mIsInitialised(false), mScreen(0), mUpdateConsole(false), mUpdatePrompt(false), mStartline(0) {
}

OgreConsole::~OgreConsole() {
  if (mIsInitialised)
	shutdown();
}

void OgreConsole::init(Gorilla::Screen *screen) {

  if (mIsInitialised)
	shutdown();

  Ogre::Root::getSingletonPtr()->addFrameListener(this);
  Ogre::LogManager::getSingleton().getDefaultLog()->addListener(this);

  // Init gorilla things here.
  mScreen = screen;
  mLayer = mScreen->createLayer(15);
  mGlyphData = mLayer->_getGlyphData(CONSOLE_FONT_INDEX); // Font.CONSOLE_FONT_INDEX

  mConsoleText = mLayer->createMarkupText(CONSOLE_FONT_INDEX, 10, 10, Ogre::StringUtil::BLANK);
  mConsoleText->width(mScreen->getWidth() - 10);
  mPromptText = mLayer->createCaption(CONSOLE_FONT_INDEX, 10, 10, "> _");
  mDecoration = mLayer->createRectangle(8, 8, mScreen->getWidth() - 16, mGlyphData->mLineHeight);
  mDecoration->background_gradient(Gorilla::Gradient_NorthSouth,
								   Gorilla::rgb(128, 128, 128, 128),
								   Gorilla::rgb(64, 64, 64, 128));
  mDecoration->border(2, Gorilla::rgb(128, 128, 128, 128));

  mIsInitialised = true;

  print("%5Ogre%R%6Console%0 Activated. Press F1 to show/hide.%R");
}

void OgreConsole::shutdown() {
  if (!mIsInitialised)
	return;

  mIsInitialised = false;

  Ogre::Root::getSingletonPtr()->removeFrameListener(this);
  Ogre::LogManager::getSingleton().getDefaultLog()->removeListener(this);

  mScreen->destroy(mLayer);

}

void OgreConsole::OnKeyDown(SDL_Keycode arg) {
  if (arg==SDLK_BACKQUOTE)
	setVisible(!isVisible());

  if (!mIsVisible)
	return;

  if (arg==SDLK_RETURN) {

	print("%3> " + prompt + "%R");

	//split the parameter list
	Ogre::StringVector params = Ogre::StringUtil::split(prompt, " ");

	if (params.size()) {
	  std::map<Ogre::String, OgreConsoleFunctionPtr>::iterator i;
	  for (i = commands.begin(); i!=commands.end(); i++) {
		if ((*i).first==params[0]) {
		  if ((*i).second)
			(*i).second(params);
		  break;
		}
	  }
	  prompt.clear();
	  mUpdateConsole = true;
	  mUpdatePrompt = true;
	}
  } else if (arg==SDLK_AC_BACK) {
	if (!prompt.empty())
	  prompt.pop_back();
	mUpdatePrompt = true;
  } else if (arg==SDLK_PAGEUP) {
	if (mStartline > 0)
	  mStartline--;
	mUpdateConsole = true;
  } else if (arg==SDLK_PAGEDOWN) {
	if (mStartline < lines.size())
	  mUpdateConsole = true;
  } else if (arg==SDLK_BACKSPACE) {
	if (!prompt.empty())
	  prompt.pop_back();
	mUpdatePrompt = true;
  } else {
	for (unsigned int c = 0; c < sizeof(legalchars); c++) {
	  if (legalchars[c]==arg) {
		prompt += arg;
		break;
	  }
	}
	mUpdatePrompt = true;
  }
}

bool OgreConsole::frameStarted(const Ogre::FrameEvent &evt) {
  if (mUpdateConsole)
	updateConsole();

  if (mUpdatePrompt)
	updatePrompt();

  return true;
}

void OgreConsole::updateConsole() {

  mUpdateConsole = false;

  std::stringstream text;
  std::list<Ogre::String>::iterator i, start, end;

  //make sure is in range
  if (mStartline > lines.size())
	mStartline = lines.size();

  int lcount = 0;
  start = lines.begin();
  for (unsigned int c = 0; c < mStartline; c++)
	start++;
  end = start;
  for (unsigned int c = 0; c < CONSOLE_LINE_COUNT; c++) {
	if (end==lines.end())
	  break;
	end++;
  }

  for (i = start; i!=end; i++) {
	lcount++;
	text << (*i) << "\n";
  }
  mConsoleText->text(text.str());

  // Move prompt downwards.
  mPromptText->top(10 + (lcount*mGlyphData->mLineHeight));

  // Change background height so it covers the text and prompt
  mDecoration->height(((lcount + 1)*mGlyphData->mLineHeight) + 4);

  mConsoleText->width(mScreen->getWidth() - 20);
  mDecoration->width(mScreen->getWidth() - 16);
  mPromptText->width(mScreen->getWidth() - 20);

}

void OgreConsole::updatePrompt() {
  mUpdatePrompt = false;
  std::stringstream text;
  text << "> " << prompt << "_";
  mPromptText->text(text.str());
}

void OgreConsole::print(const Ogre::String &text) {
  //subdivide it into lines
  const char *str = text.c_str();
  int len = text.length();
  Ogre::String line;
  for (int c = 0; c < len; c++) {
	if (str[c]=='\n' || line.length() >= CONSOLE_LINE_LENGTH) {
	  lines.push_back(line);
	  line = "";
	}
	if (str[c]!='\n')
	  line += str[c];
  }
  if (line.length())
	lines.push_back(line);
  if (lines.size() > CONSOLE_LINE_COUNT)
	mStartline = lines.size() - CONSOLE_LINE_COUNT;
  else
	mStartline = 0;
  mUpdateConsole = true;
}

bool OgreConsole::frameEnded(const Ogre::FrameEvent &evt) {
  return true;
}

void OgreConsole::setVisible(bool isVisible) {
  mIsVisible = isVisible;
  mLayer->setVisible(mIsVisible);
}

void OgreConsole::addCommand(const Ogre::String &command, OgreConsoleFunctionPtr func) {
  commands[command] = func;
}

void OgreConsole::removeCommand(const Ogre::String &command) {
  commands.erase(commands.find(command));
}

#if OGRE_VERSION_MINOR < 8 && OGRE_VERSION_MAJOR < 2
void OgreConsole::messageLogged( const Ogre::String& message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String &logName )
#else
// "bool& skip" added in Ogre 1.8
void OgreConsole::messageLogged(const Ogre::String &message,
								Ogre::LogMessageLevel lml,
								bool maskDebug,
								const Ogre::String &logName,
								bool &skip)
#endif
{
  print(message);
}

} //namespace Gorilla