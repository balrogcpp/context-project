/// created by Andrey Vasiliev

#pragma once
#include "OggSound/OgreOggSound.h"
#include "OggSound/OgreOggSoundRoot.h"
#include "System.h"

namespace Glue {
class AudioManager final : public System<AudioManager> {
 public:
  AudioManager();
  virtual ~AudioManager();

  void Pause();
  void Resume();

  void AddSound(const char *name, const char *audioFile, Ogre::SceneNode *parent = nullptr, bool playInLoop = false);
  void PlaySound(const char *name, bool playImmediately = true);
  void StopSound(const char *name);
  void SetMasterVolume(float volume);
  void SetSoundMaxVolume(const char *name, float volume);
  void SetSoundVolume(const char *name, float volume);
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
}  // namespace Glue
