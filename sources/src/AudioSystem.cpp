// This source file is part of context-project
// Created by Andrew Vasiliev

#include "AudioSystem.h"

#include <string>

#include "Exception.h"
#include "oggsound/OgreOggSound.h"
#include "pcheader.h"

using namespace std;

namespace glue {

//----------------------------------------------------------------------------------------------------------------------
AudioSystem::AudioSystem(unsigned int max_sources, unsigned int queue_list_size) {
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
  putenv((char *)"ALSOFT_LOGLEVEL=LOG_NONE");
#elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  _putenv((char *)"ALSOFT_LOGLEVEL=LOG_NONE");
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
AudioSystem::~AudioSystem() {}

//----------------------------------------------------------------------------------------------------------------------
void AudioSystem::Cleanup() {}

//----------------------------------------------------------------------------------------------------------------------
void AudioSystem::Update(float time) {}

//----------------------------------------------------------------------------------------------------------------------
void AudioSystem::Pause() { manager_->pauseAllSounds(); }
//----------------------------------------------------------------------------------------------------------------------
void AudioSystem::Resume() { manager_->resumeAllPausedSounds(); }

//----------------------------------------------------------------------------------------------------------------------
void AudioSystem::CreateSound(const string &name, const string &file, bool loop) {
  auto *sound = manager_->createSound(name, file, true, loop, true, nullptr);
  Ogre::Root::getSingleton().getSceneManager("Default")->getRootSceneNode()->createChildSceneNode()->attachObject(
      sound);
}

//----------------------------------------------------------------------------------------------------------------------
void AudioSystem::SetListener(Ogre::SceneNode *parent) {
  auto *listener = manager_->getListener();
  if (!listener) {
    manager_->createListener();
    listener = manager_->getListener();
  }
  parent->attachObject(listener);
}

//----------------------------------------------------------------------------------------------------------------------
void AudioSystem::PlaySound(const string &name, bool immediate) {
  auto *sound = manager_->getSound(name);
  if (sound) {
    if (immediate) sound->stop();
    sound->play();
  } else {
    throw Exception(string("Sound \"") + name + "\" not found. Aborting.\n");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AudioSystem::SetMasterVolume(float volume) { manager_->setMasterVolume(volume); }

//----------------------------------------------------------------------------------------------------------------------
void AudioSystem::SetMaxVolume(const string &name, float volume) {
  if (manager_->getSound(name)) {
    manager_->getSound(name)->setMaxVolume(volume);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AudioSystem::SetVolume(const string &name, float gain) {
  if (manager_->getSound(name)) {
    manager_->getSound(name)->setVolume(gain);
  }
}

}  // namespace glue
