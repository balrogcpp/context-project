/// created by Andrey Vasiliev

#pragma once
#include "System.h"
#include <Ogre.h>

namespace OgreOggSound {
class Root;
class OgreOggSoundManager;
}  // namespace OgreOggSound

namespace gge {
class AudioManager final : public System<AudioManager> {
 public:
  AudioManager();
  virtual ~AudioManager();

  void Pause();
  void Resume();

  void AddSound(const std::string &name, const std::string &audioFile, Ogre::SceneNode *parent = nullptr, bool playInLoop = false);
  void PlaySound(const std::string &name, bool playImmediately = true);
  void StopSound(const std::string &name);
  void SetMasterVolume(float volume);
  void SetSoundMaxVolume(const std::string &name, float volume);
  void SetSoundVolume(const std::string &name, float volume);
  void AddListener(Ogre::SceneNode *parent);
  void RemoveListener(Ogre::SceneNode *parent);

 protected:
  void OnSetUp() override;
  void OnClean() override;
  void OnUpdate(float passedTime) override;

  std::unique_ptr<OgreOggSound::Root> audioRoot;
  OgreOggSound::OgreOggSoundManager *oggSoundManager = nullptr;

  Ogre::Root *ogreRoot = nullptr;
  Ogre::SceneManager *ogreSceneManager = nullptr;
  Ogre::Camera *ogreCamera = nullptr;
};
}  // namespace gge
