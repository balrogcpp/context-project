// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "AudioSystem.h"
#include "Exception.h"
#include "OgreOggSound/OgreOggSound.h"

using namespace std;

namespace Glue {

AudioSystem::AudioSystem(unsigned int MaxSourceCount, unsigned int QueueListSize) {
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
  putenv((char *)"ALSOFT_LOGLEVEL=LOG_NONE");
#elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  _putenv((char *)"ALSOFT_LOGLEVEL=LOG_NONE");
#endif

  // SetUp new factory
  auto *mOgreOggSoundFactory = new OgreOggSound::OgreOggSoundFactory();

  // Register
  Ogre::Root::getSingleton().addMovableObjectFactory(mOgreOggSoundFactory, true);
  OgreOggSound::OgreOggSoundManager::getSingleton().init("", MaxSourceCount, QueueListSize);
  SoundManagerPtr = &OgreOggSound::OgreOggSoundManager::getSingleton();
  SoundManagerPtr->setResourceGroupName(Ogre::RGN_AUTODETECT);
}

AudioSystem::~AudioSystem() {}

void AudioSystem::Cleanup() {}

void AudioSystem::Update(float PassedTime) {}

void AudioSystem::Pause() { SoundManagerPtr->pauseAllSounds(); }
void AudioSystem::Resume() { SoundManagerPtr->resumeAllPausedSounds(); }

void AudioSystem::CreateSound(const string &SoundName, const string &AudioFile, bool PlayInLoop) {
  auto *sound = SoundManagerPtr->createSound(SoundName, AudioFile, true, PlayInLoop, true, nullptr);
  auto *root_node = Ogre::Root::getSingleton().getSceneManager("Default")->getRootSceneNode();
  root_node->createChildSceneNode()->attachObject(sound);
}

void AudioSystem::SetListener(Ogre::SceneNode *ParentPtr) {
  auto *listener = SoundManagerPtr->getListener();
  if (!listener) {
    SoundManagerPtr->createListener();
    listener = SoundManagerPtr->getListener();
  }
  ParentPtr->attachObject(listener);
}

void AudioSystem::PlaySound(const string &SoundName, bool PlayImmediatly) {
  auto *sound = SoundManagerPtr->getSound(SoundName);
  if (sound) {
    if (PlayImmediatly) sound->stop();
    sound->play();
  } else {
    throw Exception(string("Sound \"") + SoundName + "\" not found. Aborting.\n");
  }
}

void AudioSystem::SetMasterVolume(float Volume) { SoundManagerPtr->setMasterVolume(Volume); }

void AudioSystem::SetMaxVolume(const string &SoundName, float MaxVolume) {
  if (SoundManagerPtr->getSound(SoundName)) {
    SoundManagerPtr->getSound(SoundName)->setMaxVolume(MaxVolume);
  }
}

void AudioSystem::SetVolume(const string &SoundName, float Volume) {
  if (SoundManagerPtr->getSound(SoundName)) {
    SoundManagerPtr->getSound(SoundName)->setVolume(Volume);
  }
}

}  // namespace Glue
