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

#include "pcheader.h"
#include "Audio.h"
#include "oggsound/OgreOggSound.h"
#include "Exception.h"

#include <string>

using namespace std;

namespace xio {
//----------------------------------------------------------------------------------------------------------------------
Audio::Audio(unsigned int max_sources, unsigned int queue_list_size) {
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
  putenv((char*)"ALSOFT_LOGLEVEL=LOG_NONE");
#elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  _putenv((char*)"ALSOFT_LOGLEVEL=LOG_NONE");
#endif

  // Init new factory
  auto *mOgreOggSoundFactory = OGRE_NEW_T(OgreOggSound::OgreOggSoundFactory, Ogre::MEMCATEGORY_GENERAL)();

  // Register
  Ogre::Root::getSingleton().addMovableObjectFactory(mOgreOggSoundFactory, true);
  OgreOggSound::OgreOggSoundManager::getSingleton().init("", max_sources, queue_list_size);
  manager_ = &OgreOggSound::OgreOggSoundManager::getSingleton();
  manager_->setResourceGroupName(Ogre::RGN_AUTODETECT);
}
//----------------------------------------------------------------------------------------------------------------------
Audio::~Audio() {

}

//----------------------------------------------------------------------------------------------------------------------
void Audio::Cleanup() {

}

//----------------------------------------------------------------------------------------------------------------------
void Audio::Update(float time) {

}

//----------------------------------------------------------------------------------------------------------------------
void Audio::Pause() {
  manager_->pauseAllSounds();
}
//----------------------------------------------------------------------------------------------------------------------
void Audio::Resume() {
  manager_->resumeAllPausedSounds();
}

//----------------------------------------------------------------------------------------------------------------------
void Audio::CreateSound(const string &name, const string &file, bool loop) {
  auto *sound = manager_->createSound(name, file, true, loop, true, nullptr);
  Ogre::Root::getSingleton().getSceneManager("Default")->getRootSceneNode()->createChildSceneNode()->attachObject(sound);
}

//----------------------------------------------------------------------------------------------------------------------
void Audio::SetListener(Ogre::SceneNode *parent) {
  auto *listener = manager_->getListener();
  if (!listener) {
    manager_->createListener();
    listener = manager_->getListener();
  }
  parent->attachObject(listener);
}

//----------------------------------------------------------------------------------------------------------------------
void Audio::PlaySound(const string &name, bool immediate) {
  auto *sound = manager_->getSound(name);
  if (sound) {
    if (immediate)
      sound->stop();
    sound->play();
  } else {
    throw Exception(string("Sound \"") + name + "\" not found. Aborting.\n");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Audio::SetMasterVolume(float volume) {
  manager_->setMasterVolume(volume);
}

//----------------------------------------------------------------------------------------------------------------------
void Audio::SetMaxVolume(const string &name, float volume) {
  if (manager_->getSound(name)) {
    manager_->getSound(name)->setMaxVolume(volume);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Audio::SetVolume(const string &name, float gain) {
  if (manager_->getSound(name)) {
    manager_->getSound(name)->setVolume(gain);
  }
}

} //namespace
