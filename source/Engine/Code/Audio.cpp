/// created by Andrey Vasiliev

#include "pch.h"
#include "Audio.h"

using namespace std;
using namespace Ogre;

namespace Glue {

Audio::Audio(int maxSourceCount, int queueListSize) {
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
  oggSoundManager->init("", maxSourceCount, queueListSize);
  oggSoundManager->setResourceGroupName(RGN_AUTODETECT);
}

Audio::~Audio() { audioRoot->shutdown(); }

void Audio::OnClean() {}

void Audio::OnSetUp() {
  auto *CameraNode = Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default")->getParentSceneNode();
  if (CameraNode) AddListener(CameraNode);
}

void Audio::OnUpdate(float passedTime) {}

void Audio::OnPause() { oggSoundManager->pauseAllSounds(); }

void Audio::OnResume() { oggSoundManager->resumeAllPausedSounds(); }

void Audio::Pause() { oggSoundManager->pauseAllSounds(); }

void Audio::Resume() { oggSoundManager->resumeAllPausedSounds(); }

void Audio::AddSound(const char *name, const char *audioFile, Ogre::SceneNode *parent, bool playInLoop) {
//#if OGRE_THREAD_SUPPORT > 0
//  this_thread::sleep_for(chrono::milliseconds(16));
//#endif
  auto *sound = oggSoundManager->createSound(name, audioFile, true, playInLoop, true, nullptr);
  auto *root_node = Root::getSingleton().getSceneManager("Default")->getRootSceneNode();
  if (parent)
    parent->attachObject(sound);
  else
    root_node->createChildSceneNode()->attachObject(sound);
}

void Audio::AddListener(SceneNode *parent) {
  auto *listener = oggSoundManager->getListener();

  if (!listener) {
    OgreAssert(oggSoundManager->createListener(), "Failed to create sound listener");
    listener = oggSoundManager->getListener();
  }

  if (parent) parent->attachObject(listener);
}

void Audio::RemoveListener(SceneNode *parent) {}

void Audio::PlaySound(const char *name, bool playImmediately) {
  auto *sound = oggSoundManager->getSound(name);
  if (sound) {
    if (playImmediately) sound->stop();
    sound->play();
  } else {
    throw std::runtime_error(string("Sound \"") + name + "\" not found. Aborting\n");
  }
}

void Audio::StopSound(const char *name) {
  auto *sound = oggSoundManager->getSound(name);
  if (sound) {
    sound->stop();
  } else {
    throw std::runtime_error(string("Sound \"") + name + "\" not found. Aborting\n");
  }
}

void Audio::SetMasterVolume(float volume) { oggSoundManager->setMasterVolume(volume); }

void Audio::SetSoundMaxVolume(const char *name, float volume) {
  auto *Sound = oggSoundManager->getSound(name);
  if (Sound) Sound->setMaxVolume(volume);
}

void Audio::SetSoundVolume(const char *name, float volume) {
  auto *sound = oggSoundManager->getSound(name);
  if (sound) sound->setVolume(volume);
}

}  // namespace Glue
