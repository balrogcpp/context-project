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

#include "NoCopy.hpp"
#include "ContextManager.hpp"
#include "InputListener.hpp"

#include <OgreFrameListener.h>
#include <OgreRenderTargetListener.h>

namespace Context {
  class CameraMan;
}

namespace Context {

class ManagerCommon
    : public Ogre::RenderTargetListener, public Ogre::FrameListener, public InputListener, public NoCopy {
 public:
  virtual void SetupGlobal();
  virtual void ResetGlobal();
  virtual void Setup() {}
  virtual void Reset() {}

  void preRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) override {}
  void postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) override {}
  bool frameRenderingQueued(const Ogre::FrameEvent &evt) override { return true; }
  void SetOgreScene(Ogre::SceneManager *scene_ptr);
  void SetCameraMan(std::shared_ptr<CameraMan> camera_ptr);
  void SetOgreCamera(Ogre::Camera *ogreCamera);
  void SetOgreViewport(Ogre::Viewport *ogreViewport);
  void SetOgreRoot(Ogre::Root *ogreRoot);

 public:
  Ogre::Root *ogre_root_ = nullptr;
  Ogre::SceneManager *ogre_scene_manager_ = nullptr;
  Ogre::SceneNode *ogre_root_node_ = nullptr;
  void SetOgreRootNode(Ogre::SceneNode *ogre_root_node);
  std::shared_ptr<CameraMan> camera_man_;
  Ogre::Camera *ogre_camera_ = nullptr;
  Ogre::Viewport *ogre_viewport_ = nullptr;
  bool registered_ = false;
};

}
