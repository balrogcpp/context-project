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

#include "ManagerCommon.hpp"

#include <OgrePlane.h>

namespace Ogre {
class RenderTarget;
class Texture;
class SceneNode;
}

namespace Context {

class ReflectionCamera : public ManagerCommon {
 private:
  static ReflectionCamera CubeMapCameraSingleton;

 public:
  static ReflectionCamera *GetSingletonPtr();
  static ReflectionCamera &GetSingleton();

 public:
  void Setup() final;
  void Reset() final;
  void FreeCamera();
  void RegPlane(Ogre::Plane);
  void UnregPlane();

 public:
  void preRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) final;
  void postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) final;
  bool frameRenderingQueued(const Ogre::FrameEvent &evt) final;

 private:
  std::shared_ptr<Ogre::Texture> reflection_;
  std::shared_ptr<Ogre::Texture> refraction_;
  Ogre::Plane plane_;
  Ogre::Camera *reflection_camera_ = nullptr ;
  Ogre::SceneNode *ogre_reflection_camera_node_ = nullptr;

 public:
  [[nodiscard]] const std::shared_ptr<Ogre::Texture> &GetReflectionTex() const {
    return reflection_;
  }
  [[nodiscard]] const std::shared_ptr<Ogre::Texture> &GetRefractionTex() const {
    return refraction_;
  }
};

} //namespace Context