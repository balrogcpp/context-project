//MIT License
//
//Copyright (c) 2021 Andrew Vasiliev
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
#include <OgreMatrix4.h>
#include <OgreMaterial.h>
#include <map>
#include <string>

namespace Ogre {
class Camera;
class Viewport;
class SceneManager;
}

namespace xio {
class GBufferSchemeHandler;

class Compositor : public Component, public Singleton<Compositor> {
 public:
  Compositor();
  virtual ~Compositor();

  void Cleanup() override;
  void Pause() override;
  void Resume() override;
  void Update(float time) override;
  void EnableEffect(const std::string &name, bool enable = true);
  void Init();

 private:
  void InitGbuffer_();
  void AddCompositorEnabled_(const std::string &name);
  void AddCompositorDisabled_(const std::string &name);
  void EnableCompositor_(const std::string &name);
  void InitMRT_();
  void InitOutput_();

  std::map<std::string, bool> effects_;
  std::unique_ptr<GBufferSchemeHandler> gbuff_handler_;
  view_ptr<Ogre::CompositorManager> compositor_manager_;
  view_ptr<Ogre::SceneManager> scene_;
  view_ptr<Ogre::Camera> camera_;
  view_ptr<Ogre::Viewport> viewport_;
};

} //namespace
