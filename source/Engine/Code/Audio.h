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
  explicit Audio(int MaxSourceCount = 16, int QueueListSize = 4);
  virtual ~Audio();

  void OnSetUp() override;
  void OnClean() override;
  void OnPause() override;
  void OnResume() override;
  void Pause();
  void Resume();
  void OnUpdate(float PassedTime) override;

  void AddSound(const char *SoundName, const char *AudioFile, Ogre::SceneNode *Node = nullptr, bool PlayInLoop = false);
  void PlaySound(const char *SoundName, bool PlayImmediately = true);
  void StopSound(const char *SoundName);
  void SetMasterVolume(float MasterVolume);
  void SetSoundMaxVolume(const char *SoundName, float MaxVolume);
  void SetSoundVolume(const char *SoundName, float Volume);
  void AddListener(Ogre::SceneNode *ParentPtr);
  void RemoveListener(Ogre::SceneNode *ParentPtr);

 protected:
  std::unique_ptr<OgreOggSound::Root> AudioRootPtr;
  OgreOggSound::OgreOggSoundManager *SoundManagerPtr = nullptr;
};

}  // namespace Glue
