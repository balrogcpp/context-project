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

#include <vector>
#include <string>
#include <memory>

#include "Singleton.h"
#include "Window.h"

namespace Context {
class CameraMan;
class ShaderResolver;
}

namespace Ogre {
class RenderTarget;
class Texture;
class Root;
class SceneManager;
class Camera;
class SceneNode;
class Viewport;
class ShadowCameraSetup;
}

namespace Context {
class Graphics : public Singleton {
 public:
  static Graphics &GetSingleton() {
    static Graphics singleton;
    return singleton;
  }

  Graphics();
  virtual ~Graphics();

  void CreateScene();
  void Render();

 private:
  void InitOgre();
  void CreateCamera();

 private:
  Window window_;

  Ogre::Root *root_ = nullptr;
  Ogre::SceneManager *scene_ = nullptr;
  Ogre::SceneNode *camera_node_ = nullptr;
  Ogre::Viewport *viewport_ = nullptr;
  Ogre::Camera *camera_ = nullptr;
  CameraMan *camera_man_ = nullptr;
  std::shared_ptr<Ogre::ShadowCameraSetup> ogre_shadow_camera_setup_;

 public:
  CameraMan *GetCameraMan() const {
    return camera_man_;
  }
}; //class ContextManager
} //namespace Context
