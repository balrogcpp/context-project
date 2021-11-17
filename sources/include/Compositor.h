// This source file is part of "glue project"
// Created by Andrew Vasiliev

#pragma once
#include <OgreMaterial.h>
#include <OgreMatrix4.h>

#include <map>
#include <string>

#include "Singleton.h"
#include "System.h"

namespace Ogre {
class Camera;
class Viewport;
class SceneManager;
}  // namespace Ogre

namespace glue {
class GBufferSchemeHandler;

class Compositor : public System, public Singleton<Compositor> {
 public:
  Compositor();
  virtual ~Compositor();

  void Cleanup() override;
  void Pause() override;
  void Resume() override;
  void Update(float time) override;
  void EnableEffect(const std::string &name, bool enable = true);
  void SetUp();

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

}  // namespace glue
