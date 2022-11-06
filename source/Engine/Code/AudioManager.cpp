/// created by Andrey Vasiliev

#include "pch.h"
#include "AudioManager.h"
#include <OggSound/OgreOggSound.h>
#include <OggSound/OgreOggSoundRoot.h>

using namespace std;

namespace Glue {
AudioManager::AudioManager() {}
AudioManager::~AudioManager() { audioRoot->shutdown(); }

void AudioManager::OnSetUp() {
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX || OGRE_PLATFORM == OGRE_PLATFORM_APPLE
  putenv((char *)"ALSOFT_LOGLEVEL=LOG_NONE");
#elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  _putenv((char *)"ALSOFT_LOGLEVEL=LOG_NONE");
#endif
  ogreRoot = Ogre::Root::getSingletonPtr();
  ASSERTION(ogreRoot, "[AudioManager] ogreRoot is not initialised");
  ogreSceneManager = ogreRoot->getSceneManager("Default");
  ASSERTION(ogreSceneManager, "[AudioManager] ogreSceneManager is not initialised");
  ASSERTION(ogreSceneManager->hasCamera("Default"), "[AudioManager] ogreCamera is not initialised");
  ogreCamera = ogreSceneManager->getCamera("Default");

  audioRoot = make_unique<OgreOggSound::Root>();
  audioRoot->initialise();
  oggSoundManager = OgreOggSound::OgreOggSoundManager::getSingletonPtr();
  ASSERTION(oggSoundManager, "[AudioManager] oggSoundManager not initialised");
  oggSoundManager->init();
  oggSoundManager->setResourceGroupName(Ogre::RGN_AUTODETECT);

  if (ogreCamera->getParentSceneNode()) {
    AddListener(ogreCamera->getParentSceneNode());
  }
}
void AudioManager::OnUpdate(float passedTime) {}
void AudioManager::OnClean() {}

void AudioManager::Pause() { oggSoundManager->pauseAllSounds(); }
void AudioManager::Resume() { oggSoundManager->resumeAllPausedSounds(); }

void AudioManager::AddSound(const std::string &name, const std::string &audioFile, Ogre::SceneNode *parent, bool playInLoop) {
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
    OgreAssert(oggSoundManager->createListener(), "[AudioManager] Failed to create listener");
    listener = oggSoundManager->getListener();
  }

  parent->attachObject(listener);
}

void AudioManager::RemoveListener(Ogre::SceneNode *parent) {}

void AudioManager::PlaySound(const std::string &name, bool playImmediately) {
  auto *sound = oggSoundManager->getSound(name);
  OgreAssert(sound, "[AudioManager] Sound not found");
  if (playImmediately) sound->stop();
  sound->play();
}

void AudioManager::StopSound(const std::string &name) {
  auto *sound = oggSoundManager->getSound(name);
  OgreAssert(sound, "[AudioManager] Sound not found");
  sound->stop();
}

void AudioManager::SetMasterVolume(float volume) { oggSoundManager->setMasterVolume(volume); }

void AudioManager::SetSoundMaxVolume(const std::string &name, float volume) {
  auto *sound = oggSoundManager->getSound(name);
  OgreAssert(sound, "[AudioManager] Sound not found");
  sound->setMaxVolume(volume);
}

void AudioManager::SetSoundVolume(const std::string &name, float volume) {
  auto *sound = oggSoundManager->getSound(name);
  OgreAssert(sound, "[AudioManager] Sound not found");
  sound->setVolume(volume);
}
}  // namespace Glue
