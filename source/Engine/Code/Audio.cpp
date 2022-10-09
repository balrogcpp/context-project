/// created by Andrey Vasiliev

#include "pch.h"
#include "Audio.h"

using namespace std;
using namespace Ogre;

namespace Glue {

Audio::Audio(int MaxSourceCount, int QueueListSize) {
#ifndef DEBUG
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
  putenv((char *)"ALSOFT_LOGLEVEL=LOG_NONE");
#elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  _putenv((char *)"ALSOFT_LOGLEVEL=LOG_NONE");
#endif
#endif

  AudioRootPtr = make_unique<OgreOggSound::Root>();
  AudioRootPtr->initialise();
  SoundManagerPtr = &OgreOggSound::OgreOggSoundManager::getSingleton();
  SoundManagerPtr->init("", MaxSourceCount, QueueListSize);
  SoundManagerPtr->setResourceGroupName(RGN_AUTODETECT);
}

Audio::~Audio() { AudioRootPtr->shutdown(); }

void Audio::OnClean() {}

void Audio::OnSetUp() {
  auto *CameraNode = Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default")->getParentSceneNode();
  if (CameraNode) AddListener(CameraNode);
}

void Audio::OnUpdate(float PassedTime) {}

void Audio::OnPause() { SoundManagerPtr->pauseAllSounds(); }

void Audio::OnResume() { SoundManagerPtr->resumeAllPausedSounds(); }

void Audio::Pause() { SoundManagerPtr->pauseAllSounds(); }

void Audio::Resume() { SoundManagerPtr->resumeAllPausedSounds(); }

void Audio::AddSound(const char *SoundName, const char *AudioFile, Ogre::SceneNode *Node, bool PlayInLoop) {
//#if OGRE_THREAD_SUPPORT > 0
//  this_thread::sleep_for(chrono::milliseconds(16));
//#endif
  auto *sound = SoundManagerPtr->createSound(SoundName, AudioFile, true, PlayInLoop, true, nullptr);
  auto *root_node = Root::getSingleton().getSceneManager("Default")->getRootSceneNode();
  if (Node)
    Node->attachObject(sound);
  else
    root_node->createChildSceneNode()->attachObject(sound);
}

void Audio::AddListener(SceneNode *ParentPtr) {
  auto *listener = SoundManagerPtr->getListener();

  if (!listener) {
    OgreAssert(SoundManagerPtr->createListener(), "Failed to create sound listener");
    listener = SoundManagerPtr->getListener();
  }

  if (ParentPtr) ParentPtr->attachObject(listener);
}

void Audio::RemoveListener(SceneNode *ParentPtr) {}

void Audio::PlaySound(const char *SoundName, bool PlayImmediately) {
  auto *sound = SoundManagerPtr->getSound(SoundName);
  if (sound) {
    if (PlayImmediately) sound->stop();
    sound->play();
  } else {
    throw std::runtime_error(string("Sound \"") + SoundName + "\" not found. Aborting\n");
  }
}

void Audio::StopSound(const char *SoundName) {
  auto *sound = SoundManagerPtr->getSound(SoundName);
  if (sound) {
    sound->stop();
  } else {
    throw std::runtime_error(string("Sound \"") + SoundName + "\" not found. Aborting\n");
  }
}

void Audio::SetMasterVolume(float Volume) { SoundManagerPtr->setMasterVolume(Volume); }

void Audio::SetSoundMaxVolume(const char *SoundName, float MaxVolume) {
  auto *Sound = SoundManagerPtr->getSound(SoundName);
  if (Sound) Sound->setMaxVolume(MaxVolume);
}

void Audio::SetSoundVolume(const char *SoundName, float Volume) {
  auto *sound = SoundManagerPtr->getSound(SoundName);
  if (sound) sound->setVolume(Volume);
}

}  // namespace Glue
