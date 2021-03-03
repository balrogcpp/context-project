//MIT License
//
//Copyright (c) 2020 Andrey Vasiliev
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#pragma once
#include "Component.h"
#include "Singleton.h"
#include <string>
#include "view_ptr.h"

namespace OgreOggSound{
  class OgreOggSoundManager;
}

namespace xio {
class Sound final : public Component, public Singleton<Sound> {
 public:
  explicit Sound(unsigned int max_sources = 100, unsigned int queue_list_size = 100);
  virtual ~Sound();

  void Create() final {}
  void Reset() final {}
  void Clean() final;
  void Pause() final;
  void Resume() final;
  void Update(float time) final {}

  void CreateSound(const std::string &name, const std::string &file, bool loop = false);
  void PlaySound(const std::string &name, bool immediate = true);
  void SetMasterVolume(float volume);
  void SetMaxVolume(const std::string &name, float volume);
  void SetVolume(const std::string &name, float gain);
  void SetListener(Ogre::SceneNode *parent);
 private:
  view_ptr<OgreOggSound::OgreOggSoundManager> manager_;
};
}