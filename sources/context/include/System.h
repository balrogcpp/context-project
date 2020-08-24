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

#include "Singleton.h"
#include "IO.h"

#include <OgreFrameListener.h>
#include <OgreRenderTargetListener.h>

namespace Context {

class System
 : public Ogre::RenderTargetListener, public Ogre::FrameListener, public io::InputListener, public Singleton {
 public:
  System();
  virtual ~System();

  virtual void Init() {}
  virtual void Clear() {}

  void preRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) override {}
  void postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) override {}
  bool frameRenderingQueued(const Ogre::FrameEvent &evt) override { return true; }

 public:
  Ogre::SceneManager *scene_ = nullptr;
  Ogre::Camera *camera_ = nullptr;
  Ogre::Viewport *viewport_ = nullptr;
};
}
