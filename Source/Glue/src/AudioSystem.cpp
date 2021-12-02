// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "AudioSystem.h"
#include "Exception.h"
#include "oggsound/OgreOggSound.h"

using namespace std;

namespace glue {

AudioSystem::AudioSystem(unsigned int max_sources, unsigned int queue_list_size) {
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
  putenv((char *)"ALSOFT_LOGLEVEL=LOG_NONE");
#elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  _putenv((char *)"ALSOFT_LOGLEVEL=LOG_NONE");
#endif

  // SetUp new factory
  auto *mOgreOggSoundFactory = new OgreOggSound::OgreOggSoundFactory();

  // Register
  Ogre::Root::getSingleton().addMovableObjectFactory(mOgreOggSoundFactory, true);
  OgreOggSound::OgreOggSoundManager::getSingleton().init("", max_sources, queue_list_size);
  sound_manager = &OgreOggSound::OgreOggSoundManager::getSingleton();
  sound_manager->setResourceGroupName(Ogre::RGN_AUTODETECT);
}

AudioSystem::~AudioSystem() {}

void AudioSystem::Cleanup() {}

void AudioSystem::Update(float time) {}

void AudioSystem::Pause() { sound_manager->pauseAllSounds(); }
void AudioSystem::Resume() { sound_manager->resumeAllPausedSounds(); }

void AudioSystem::CreateSound(const string &name, const string &file, bool loop) {
  auto *sound = sound_manager->createSound(name, file, true, loop, true, nullptr);
  auto *root_node = Ogre::Root::getSingleton().getSceneManager("Default")->getRootSceneNode();
  root_node->createChildSceneNode()->attachObject(sound);
}

void AudioSystem::SetListener(Ogre::SceneNode *parent) {
  auto *listener = sound_manager->getListener();
  if (!listener) {
    sound_manager->createListener();
    listener = sound_manager->getListener();
  }
  parent->attachObject(listener);
}

void AudioSystem::PlaySound(const string &name, bool immediate) {
  auto *sound = sound_manager->getSound(name);
  if (sound) {
    if (immediate) sound->stop();
    sound->play();
  } else {
    throw Exception(string("Sound \"") + name + "\" not found. Aborting.\n");
  }
}

void AudioSystem::SetMasterVolume(float volume) { sound_manager->setMasterVolume(volume); }

void AudioSystem::SetMaxVolume(const string &name, float volume) {
  if (sound_manager->getSound(name)) {
    sound_manager->getSound(name)->setMaxVolume(volume);
  }
}

void AudioSystem::SetVolume(const string &name, float gain) {
  if (sound_manager->getSound(name)) {
    sound_manager->getSound(name)->setVolume(gain);
  }
}

}  // namespace glue
