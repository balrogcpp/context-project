// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include "Components/Component.h"
#include <string>

namespace OgreOggSound {
class OgreOggSoundManager;
class Root;
}

namespace Glue {

class Sound final : public Component<Sound> {
 public:
  Sound(unsigned int MaxSourceCount = 16, unsigned int QueueListSize = 4);
  virtual ~Sound();

  void OnSetUp() override;
  void OnClean() override;
  void OnPause() override;
  void OnResume() override;
  void Pause();
  void Resume();
  void OnUpdate(float PassedTime) override;

  void AddSound(const std::string &SoundName, const std::string &AudioFile, Ogre::SceneNode *Node = nullptr, bool PlayInLoop = false);
  void PlaySound(const std::string &SoundName, bool PlayImmediately = true);
  void StopSound(const std::string &SoundName);
  void SetMasterVolume(float MasterVolume);
  void SetSoundMaxVolume(const std::string &SoundName, float MaxVolume);
  void SetSoundVolume(const std::string &SoundName, float Volume);
  void AddListener(Ogre::SceneNode *ParentPtr);
  void RemoveListener(Ogre::SceneNode *ParentPtr);

 protected:
  std::unique_ptr<OgreOggSound::Root> AudioRootPtr;
  OgreOggSound::OgreOggSoundManager *SoundManagerPtr = nullptr;
};

}  // namespace Glue
