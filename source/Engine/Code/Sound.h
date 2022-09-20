/// created by Andrey Vasiliev

#pragma once
#include "OggSound/OgreOggSound.h"
#include "OggSound/OgreOggSoundRoot.h"
#include "System.h"
#include <Ogre.h>
#include <memory>
#include <string>


namespace Glue {

class Sound final : public System<Sound> {
 public:
  Sound(const int MaxSourceCount = 16, const int QueueListSize = 4);
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
