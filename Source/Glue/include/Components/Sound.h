// This source file is part of "glue project". Created by Andrey Vasiliev

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

  void OnClean() override;
  void OnPause() override;
  void OnResume() override;
  void OnUpdate(float PassedTime) override;

  void CreateSound(const std::string &SoundName, const std::string &AudioFile, bool PlayInLoop = false);
  void PlaySound(const std::string &SoundName, bool PlayImmediately = true);
  void SetMasterVolume(float MasterVolume);
  void SetMaxVolume(const std::string &SoundName, float MaxVolume);
  void SetVolume(const std::string &SoundName, float Volume);
  void SetListener(Ogre::SceneNode *ParentPtr);

 protected:
  std::unique_ptr<OgreOggSound::Root> AudioRootPtr;
  OgreOggSound::OgreOggSoundManager *SoundManagerPtr = nullptr;
};

}  // namespace Glue
