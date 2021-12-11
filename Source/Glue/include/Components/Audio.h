// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Components/Component.h"
#include "LazySingleton.h"
#include <string>

namespace OgreOggSound {
class OgreOggSoundManager;
class Root;
}

namespace Glue {
class Audio final : public Component, public Singleton<Audio> {
 public:
  Audio(unsigned int MaxSourceCount = 16, unsigned int QueueListSize = 4);
  virtual ~Audio();

  void Cleanup() override;
  void Pause() override;
  void Resume() override;
  void Update(float PassedTime) override;

  void CreateSound(const std::string &SoundName, const std::string &AudioFile, bool PlayInLoop = false);
  void PlaySound(const std::string &SoundName, bool PlayImmediatly = true);
  void SetMasterVolume(float MasterVolume);
  void SetMaxVolume(const std::string &SoundName, float MaxVolume);
  void SetVolume(const std::string &SoundName, float Volume);
  void SetListener(Ogre::SceneNode *ParentPtr);

 protected:
  std::unique_ptr<OgreOggSound::Root> AudioRootPtr;
  OgreOggSound::OgreOggSoundManager *SoundManagerPtr = nullptr;
};

}  // namespace Glue
