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

#include "IO.h"
#include "NoCopy.h"

namespace Context {
class ConfiguratorJson;
class Renderer;
class Physic;
class Sound;
class Overlay;
class DotSceneLoaderB;

class AppState
    : public Ogre::RenderTargetListener, public Ogre::FrameListener, public io::InputListener, public NoCopy {
 public:
  AppState();
  virtual ~AppState();

  void GetComponents(ConfiguratorJson *conf,
                     io::InputSequencer *io,
                     Renderer *renderer,
                     Physic *physics,
                     Sound *sounds,
                     Overlay *overlay,
                     DotSceneLoaderB *loader
  );

  virtual void Init() = 0;
  virtual void Clear() = 0;

  void Load(const std::string &file_name);

 protected:
  ConfiguratorJson *conf_ = nullptr;
  Renderer *renderer_ = nullptr;
  Physic *physics_ = nullptr;
  Sound *sounds_ = nullptr;
  Overlay *overlay_ = nullptr;
  DotSceneLoaderB *loader_ = nullptr;
  io::InputSequencer *io_ = nullptr;

 private:
  Ogre::SceneManager *scene_ = nullptr;
  Ogre::Camera *camera_ = nullptr;
  Ogre::Viewport *viewport_ = nullptr;
}; //class AppState
} //namespace Context