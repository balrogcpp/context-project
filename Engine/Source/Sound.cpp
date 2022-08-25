// This source file is part of Glue Engine. Created by Andrey Vasiliev

#include "pch.h"
#include "Sound.h"

using namespace std;
using namespace Ogre;

namespace Glue {

Sound::Sound(const int MaxSourceCount, const int QueueListSize) {
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

void Sound::OnClean() {}

void Sound::OnSetUp() {
  auto *CameraNode = Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default")->getParentSceneNode();
  if (CameraNode) AddListener(CameraNode);
}

void Sound::OnUpdate(float PassedTime) {}

void Sound::OnPause() { SoundManagerPtr->pauseAllSounds(); }

void Sound::OnResume() { SoundManagerPtr->resumeAllPausedSounds(); }

void Sound::Pause() { SoundManagerPtr->pauseAllSounds(); }

void Sound::Resume() { SoundManagerPtr->resumeAllPausedSounds(); }

void Sound::AddSound(const string &SoundName, const string &AudioFile, Ogre::SceneNode *Node, bool PlayInLoop) {
#if OGRE_THREAD_SUPPORT > 0
  this_thread::sleep_for(chrono::milliseconds(16));
#endif
  auto *sound = SoundManagerPtr->createSound(SoundName, AudioFile, true, PlayInLoop, true, nullptr);
  auto *root_node = Root::getSingleton().getSceneManager("Default")->getRootSceneNode();
  if (Node)
    Node->attachObject(sound);
  else
    root_node->createChildSceneNode()->attachObject(sound);
}

void Sound::AddListener(SceneNode *ParentPtr) {
  auto *listener = SoundManagerPtr->getListener();

  if (!listener) {
    OgreAssert(SoundManagerPtr->createListener(), "Failed to create sound listener");
    listener = SoundManagerPtr->getListener();
  }

  if (ParentPtr) ParentPtr->attachObject(listener);
}

void Sound::RemoveListener(SceneNode *ParentPtr) {}

void Sound::PlaySound(const string &SoundName, bool PlayImmediately) {
  auto *sound = SoundManagerPtr->getSound(SoundName);
  if (sound) {
    if (PlayImmediately) sound->stop();
    sound->play();
  } else {
    throw std::runtime_error(string("Sound \"") + SoundName + "\" not found. Aborting\n");
  }
}

void Sound::StopSound(const string &SoundName) {
  auto *sound = SoundManagerPtr->getSound(SoundName);
  if (sound) {
    sound->stop();
  } else {
    throw std::runtime_error(string("Sound \"") + SoundName + "\" not found. Aborting\n");
  }
}

void Sound::SetMasterVolume(float Volume) { SoundManagerPtr->setMasterVolume(Volume); }

void Sound::SetSoundMaxVolume(const string &SoundName, float MaxVolume) {
  auto *Sound = SoundManagerPtr->getSound(SoundName);
  if (Sound) Sound->setMaxVolume(MaxVolume);
}

void Sound::SetSoundVolume(const string &SoundName, float Volume) {
  auto *sound = SoundManagerPtr->getSound(SoundName);
  if (sound) sound->setVolume(Volume);
}

}  // namespace Glue
