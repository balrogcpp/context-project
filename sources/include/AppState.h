/*
MIT License

Copyright (c) 2020 Andrey Vasiliev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <OgreFrameListener.h>
#include <OgreRenderTargetListener.h>

#include "Input.h"
#include "NoCopy.h"
#include <OgreRoot.h>
#include <OgreSceneLoaderManager.h>

namespace xio {
class JsonConfigurator;
class YamlConfigurator;
class Render;
class Physics;
class Sound;
class Overlay;
class DotSceneLoaderB;

class AppState
    : public Ogre::RenderTargetListener, public Ogre::FrameListener, public io::InputObserver, public NoCopy {
 public:
//----------------------------------------------------------------------------------------------------------------------
  void SwitchNextState(std::unique_ptr<AppState> &&app_state) {
    next_ = std::move(app_state);
    waiting_ = true;
  }
//----------------------------------------------------------------------------------------------------------------------
  std::unique_ptr<AppState> &&GetNextState() {
    waiting_ = false;
    return move(next_);
  }
//----------------------------------------------------------------------------------------------------------------------
  void LocateComponents(YamlConfigurator *conf,
                        io::InputSequencer *io,
                        Render *renderer,
                        Physics *physics,
                        Sound *sounds,
                        Overlay *overlay,
                        DotSceneLoaderB *loader) {
    conf_ = conf;
    io_ = io;
    renderer_ = renderer;
    physics_ = physics;
    sounds_ = sounds;
    overlay_ = overlay;
    loader_ = loader;
  }
//----------------------------------------------------------------------------------------------------------------------
  void Load(const std::string &file_name) {
    auto *scene_ = Ogre::Root::getSingleton().getSceneManager("Default");
    Ogre::SceneLoaderManager::getSingleton().load(file_name,
                                                  Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME,
                                                  scene_->getRootSceneNode());
  }

  virtual void Create() = 0;
  virtual void Clear() = 0;

  bool Loop() {
    return waiting_;
  }

 protected:
  YamlConfigurator *conf_ = nullptr;
  Render *renderer_ = nullptr;
  Physics *physics_ = nullptr;
  Sound *sounds_ = nullptr;
  Overlay *overlay_ = nullptr;
  DotSceneLoaderB *loader_ = nullptr;
  io::InputSequencer *io_ = nullptr;
  std::unique_ptr<AppState> next_;
  bool waiting_ = false;
};
}