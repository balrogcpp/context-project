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

#include "pcheader.h"
#include "Sound.h"
#include "Exception.h"

namespace xio {
//----------------------------------------------------------------------------------------------------------------------
Sound::Sound() {
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
  putenv("ALSOFT_LOGLEVEL=LOG_NONE");
#elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  _putenv("ALSOFT_LOGLEVEL=LOG_NONE");
#endif

  // Create new factory
  auto *mOgreOggSoundFactory = OGRE_NEW_T(OgreOggSound::OgreOggSoundFactory, Ogre::MEMCATEGORY_GENERAL)();

  // Register
  Ogre::Root::getSingleton().addMovableObjectFactory(mOgreOggSoundFactory, true);
  const std::string DEVICE_NAME = "";
  const unsigned int MAX_SOURCES = 100;
  const unsigned int QUEUE_LIST_SIZE = 100;
  OgreOggSound::OgreOggSoundManager::getSingleton().init(DEVICE_NAME, MAX_SOURCES, QUEUE_LIST_SIZE);
  OgreOggSound::OgreOggSoundManager::getSingleton().setDistanceModel(AL_LINEAR_DISTANCE);
  manager_ = OgreOggSound::OgreOggSoundManager::getSingletonPtr();
  manager_->setResourceGroupName(Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
}
//----------------------------------------------------------------------------------------------------------------------
Sound::~Sound() {
  Clear();
}
//----------------------------------------------------------------------------------------------------------------------
void Sound::Clean() {
  OgreOggSound::OgreOggSoundManager::getSingleton().stopAllSounds();
  OgreOggSound::OgreOggSoundManager::getSingleton().destroyAllSounds();
}
//----------------------------------------------------------------------------------------------------------------------
void Sound::CreateSound(const std::string &name, const std::string &file, bool loop) {
  manager_->createSound(name, file, false, loop, true, nullptr);
}
//----------------------------------------------------------------------------------------------------------------------
void Sound::PlaySound(const std::string &name, bool immediate) {
  auto *sound = manager_->getSound(name);
  if (sound) {
    if (immediate)
      sound->stop();
    sound->play();
  } else {
    throw Exception(std::string("Sound \"") + name + "\" not found. Aborting.\n");
  }
}
//----------------------------------------------------------------------------------------------------------------------
void Sound::SetMasterVolume(float volume) {
  manager_->setMasterVolume(volume);
}
//----------------------------------------------------------------------------------------------------------------------
void Sound::SetMaxVolume(const std::string &name, float volume) {
  if (manager_->getSound(name)) {
    manager_->getSound(name)->setMaxVolume(volume);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void Sound::SetVolume(const std::string &name, float gain) {
  if (manager_->getSound(name)) {
    manager_->getSound(name)->setVolume(gain);
  }
}
}