// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Component.h"
#include "Singleton.h"
#include <string>

namespace OgreOggSound {
class OgreOggSoundManager;
}

namespace Glue {
class AudioSystem final : public Component, public Singleton<AudioSystem> {
 public:
  AudioSystem(unsigned int MaxSourceCount = 16, unsigned int QueueListSize = 4);
  virtual ~AudioSystem();

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
  OgreOggSound::OgreOggSoundManager *SoundManagerPtr = nullptr;
};

}  // namespace Glue
