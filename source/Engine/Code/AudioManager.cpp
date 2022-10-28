/// created by Andrey Vasiliev

#include "pch.h"
#include "AudioManager.h"

using namespace std;

namespace Glue {

AudioManager::AudioManager() {}

AudioManager::~AudioManager() { audioRoot->shutdown(); }

void AudioManager::OnClean() {}

void AudioManager::OnSetUp() {
#ifndef DEBUG
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
  putenv((char *)"ALSOFT_LOGLEVEL=LOG_NONE");
#elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  _putenv((char *)"ALSOFT_LOGLEVEL=LOG_NONE");
#endif
#endif
  audioRoot = make_unique<OgreOggSound::Root>();
  audioRoot->initialise();
  oggSoundManager = &OgreOggSound::OgreOggSoundManager::getSingleton();
  oggSoundManager->init();
  oggSoundManager->setResourceGroupName(Ogre::RGN_AUTODETECT);

  auto *CameraNode = Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default")->getParentSceneNode();
  if (CameraNode) AddListener(CameraNode);
}

void AudioManager::OnUpdate(float passedTime) {}

void AudioManager::Pause() { oggSoundManager->pauseAllSounds(); }

void AudioManager::Resume() { oggSoundManager->resumeAllPausedSounds(); }

void AudioManager::AddSound(const char *name, const char *audioFile, Ogre::SceneNode *parent, bool playInLoop) {
  //#if OGRE_THREAD_SUPPORT > 0
  //  this_thread::sleep_for(chrono::milliseconds(16));
  //#endif
  auto *sound = oggSoundManager->createSound(name, audioFile, true, playInLoop, true, nullptr);
  auto *root_node = Ogre::Root::getSingleton().getSceneManager("Default")->getRootSceneNode();
  if (parent)
    parent->attachObject(sound);
  else
    root_node->createChildSceneNode()->attachObject(sound);
}

void AudioManager::AddListener(Ogre::SceneNode *parent) {
  auto *listener = oggSoundManager->getListener();

  if (!listener) {
    OgreAssert(oggSoundManager->createListener(), "Failed to create sound listener");
    listener = oggSoundManager->getListener();
  }

  if (parent) parent->attachObject(listener);
}

void AudioManager::RemoveListener(Ogre::SceneNode *parent) {}

void AudioManager::PlaySound(const char *name, bool playImmediately) {
  auto *sound = oggSoundManager->getSound(name);
  if (sound) {
    if (playImmediately) sound->stop();
    sound->play();
  } else {
    throw std::runtime_error(string("Sound \"") + name + "\" not found. Aborting\n");
  }
}

void AudioManager::StopSound(const char *name) {
  auto *sound = oggSoundManager->getSound(name);
  if (sound) {
    sound->stop();
  } else {
    throw std::runtime_error(string("Sound \"") + name + "\" not found. Aborting\n");
  }
}

void AudioManager::SetMasterVolume(float volume) { oggSoundManager->setMasterVolume(volume); }

void AudioManager::SetSoundMaxVolume(const char *name, float volume) {
  auto *Sound = oggSoundManager->getSound(name);
  if (Sound) Sound->setMaxVolume(volume);
}

void AudioManager::SetSoundVolume(const char *name, float volume) {
  auto *sound = oggSoundManager->getSound(name);
  if (sound) sound->setVolume(volume);
}

}  // namespace Glue
