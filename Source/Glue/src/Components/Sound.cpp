// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Components/Sound.h"
#include "Exception.h"
#include "OgreOggSound/OgreOggSound.h"
#include "OgreOggSound/OgreOggSoundRoot.h"

using namespace std;
using namespace Ogre;

namespace Glue {

Sound::Sound(unsigned int MaxSourceCount, unsigned int QueueListSize) {
#ifndef DEBUG
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
  putenv((char *)"ALSOFT_LOGLEVEL=LOG_NONE");
#elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  _putenv((char *)"ALSOFT_LOGLEVEL=LOG_NONE");
#endif
#endif

  AudioRootPtr = make_unique<OgreOggSound::Root>();
  AudioRootPtr->initialise();

  OgreOggSound::OgreOggSoundManager::getSingleton().init("", MaxSourceCount, QueueListSize);
  SoundManagerPtr = &OgreOggSound::OgreOggSoundManager::getSingleton();
  SoundManagerPtr->setResourceGroupName(RGN_AUTODETECT);
}

Sound::~Sound() { AudioRootPtr->shutdown(); }

void Sound::Cleanup() {}

void Sound::Update(float PassedTime) {}

void Sound::Pause() { SoundManagerPtr->pauseAllSounds(); }

void Sound::Resume() { SoundManagerPtr->resumeAllPausedSounds(); }

void Sound::CreateSound(const string &SoundName, const string &AudioFile, bool PlayInLoop) {
  auto *sound = SoundManagerPtr->createSound(SoundName, AudioFile, true, PlayInLoop, true, nullptr);
  auto *root_node = Root::getSingleton().getSceneManager("Default")->getRootSceneNode();
  root_node->createChildSceneNode()->attachObject(sound);
}

void Sound::SetListener(SceneNode *ParentPtr) {
  auto *listener = SoundManagerPtr->getListener();

  if (!listener) {
    SoundManagerPtr->createListener();
    listener = SoundManagerPtr->getListener();
  }

  if (ParentPtr) ParentPtr->attachObject(listener);
}

void Sound::PlaySound(const string &SoundName, bool PlayImmediately) {
  auto *sound = SoundManagerPtr->getSound(SoundName);
  if (sound) {
    if (PlayImmediately) sound->stop();
    sound->play();
  } else {
    throw Exception(string("Sound \"") + SoundName + "\" not found. Aborting\n");
  }
}

void Sound::SetMasterVolume(float Volume) { SoundManagerPtr->setMasterVolume(Volume); }

void Sound::SetMaxVolume(const string &SoundName, float MaxVolume) {
  if (SoundManagerPtr->getSound(SoundName)) {
    SoundManagerPtr->getSound(SoundName)->setMaxVolume(MaxVolume);
  }
}

void Sound::SetVolume(const string &SoundName, float Volume) {
  auto *sound = SoundManagerPtr->getSound(SoundName);
  if (sound) {
    sound->setVolume(Volume);
  }
}

}  // namespace Glue
