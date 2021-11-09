// This source file is part of "glue project"
// Created by Andrew Vasiliev

#pragma once
#include <string>

#include "Singleton.h"
#include "System.h"
#include "view_ptr.h"

namespace OgreOggSound {
class OgreOggSoundManager;
}

namespace glue {
class AudioSystem final : public System, public Singleton<AudioSystem> {
 public:
  AudioSystem(unsigned int max_sources, unsigned int queue_list_size);
  virtual ~AudioSystem();

  void Cleanup() override;
  void Pause() override;
  void Resume() override;
  void Update(float time) override;

  void CreateSound(const std::string &name, const std::string &file, bool loop = false);
  void PlaySound(const std::string &name, bool immediate = true);
  void SetMasterVolume(float volume);
  void SetMaxVolume(const std::string &name, float volume);
  void SetVolume(const std::string &name, float gain);
  void SetListener(Ogre::SceneNode *parent);

 private:
  view_ptr<OgreOggSound::OgreOggSoundManager> manager_;
};

}  // namespace glue
