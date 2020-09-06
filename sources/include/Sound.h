/*
MIT License

Copyright (c) 2020 Andrey Vasiliev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "Component.h"
#include "OgreOggSound.h"

namespace xio {

class Sound final : public Component {
 public:
  Sound() {
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
    putenv("ALSOFT_LOGLEVEL=LOG_NONE");
#elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    _putenv("ALSOFT_LOGLEVEL=LOG_NONE");
#endif

    OgreOggSound::OgreOggSoundFactory *mOgreOggSoundFactory;
    // Create new factory
    mOgreOggSoundFactory = OGRE_NEW_T(OgreOggSound::OgreOggSoundFactory, Ogre::MEMCATEGORY_GENERAL)();

    // Register
    Ogre::Root::getSingleton().addMovableObjectFactory(mOgreOggSoundFactory, true);

    OgreOggSound::OgreOggSoundManager::getSingleton().init();
  }

  virtual ~Sound() {
    OgreOggSound::OgreOggSoundManager::getSingleton().stopAllSounds();
    OgreOggSound::OgreOggSoundManager::getSingleton().destroyAllSounds();
  }

  void Create() final {}
  void Clear() final {}
  void Update(float time) final {}
};
}