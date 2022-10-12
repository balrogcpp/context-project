/// created by Andrey Vasiliev

#pragma once

#include "OggSound/OgreOggSound.h"
#include "OggSound/OgreOggSoundRoot.h"
#include "System.h"
#include <Ogre.h>
#include <memory>
#include <string>

namespace Glue {

class Audio final : public System<Audio> {
 public:
  explicit Audio(int maxSourceCount = 16, int queueListSize = 4);
  virtual ~Audio();

  void OnSetUp() override;
  void OnClean() override;
  void OnPause() override;
  void OnResume() override;
  void Pause();
  void Resume();
  void OnUpdate(float passedTime) override;

  void AddSound(const char *name, const char *audioFile, Ogre::SceneNode *parent = nullptr, bool playInLoop = false);
  void PlaySound(const char *name, bool playImmediately = true);
  void StopSound(const char *name);
  void SetMasterVolume(float volume);
  void SetSoundMaxVolume(const char *name, float volume);
  void SetSoundVolume(const char *name, float volume);
  void AddListener(Ogre::SceneNode *parent);
  void RemoveListener(Ogre::SceneNode *parent);

 protected:
  std::unique_ptr<OgreOggSound::Root> audioRoot;
  OgreOggSound::OgreOggSoundManager *soundManager = nullptr;
};

}  // namespace Glue
