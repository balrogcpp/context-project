// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Components/Audio.h"
#include "Exception.h"
#include "OgreOggSound/OgreOggSound.h"
#include "OgreOggSound/OgreOggSoundRoot.h"

using namespace std;

namespace Glue {

Audio::Audio(unsigned int MaxSourceCount, unsigned int QueueListSize) {
#ifndef DEBUG
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
  putenv((char *)"ALSOFT_LOGLEVEL=LOG_NONE");
#elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  _putenv((char *)"ALSOFT_LOGLEVEL=LOG_NONE");
#endif
#endif

  // SetUp new factory
  AudioRootPtr = make_unique<OgreOggSound::Root>();
  AudioRootPtr->initialise();
  //  auto *mOgreOggSoundFactory = new OgreOggSound::OgreOggSoundFactory();
  //  Ogre::Root::getSingleton().addMovableObjectFactory(mOgreOggSoundFactory, true);

  OgreOggSound::OgreOggSoundManager::getSingleton().init("", MaxSourceCount, QueueListSize);
  SoundManagerPtr = &OgreOggSound::OgreOggSoundManager::getSingleton();
  SoundManagerPtr->setResourceGroupName(Ogre::RGN_AUTODETECT);
}

Audio::~Audio() { AudioRootPtr->shutdown(); }

void Audio::Cleanup() {}

void Audio::Update(float PassedTime) {}

void Audio::Pause() { SoundManagerPtr->pauseAllSounds(); }
void Audio::Resume() { SoundManagerPtr->resumeAllPausedSounds(); }

void Audio::CreateSound(const string &SoundName, const string &AudioFile, bool PlayInLoop) {
  auto *sound = SoundManagerPtr->createSound(SoundName, AudioFile, true, PlayInLoop, true, nullptr);
  auto *root_node = Ogre::Root::getSingleton().getSceneManager("Default")->getRootSceneNode();
  root_node->createChildSceneNode()->attachObject(sound);
}

void Audio::SetListener(Ogre::SceneNode *ParentPtr) {
  auto *listener = SoundManagerPtr->getListener();
  if (!listener) {
    SoundManagerPtr->createListener();
    listener = SoundManagerPtr->getListener();
  }
  ParentPtr->attachObject(listener);
}

void Audio::PlaySound(const string &SoundName, bool PlayImmediatly) {
  auto *sound = SoundManagerPtr->getSound(SoundName);
  if (sound) {
    if (PlayImmediatly) sound->stop();
    sound->play();
  } else {
    throw Exception(string("Sound \"") + SoundName + "\" not found. Aborting.\n");
  }
}

void Audio::SetMasterVolume(float Volume) { SoundManagerPtr->setMasterVolume(Volume); }

void Audio::SetMaxVolume(const string &SoundName, float MaxVolume) {
  if (SoundManagerPtr->getSound(SoundName)) {
    SoundManagerPtr->getSound(SoundName)->setMaxVolume(MaxVolume);
  }
}

void Audio::SetVolume(const string &SoundName, float Volume) {
  if (SoundManagerPtr->getSound(SoundName)) {
    SoundManagerPtr->getSound(SoundName)->setVolume(Volume);
  }
}

}  // namespace Glue
