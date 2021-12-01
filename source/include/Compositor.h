// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Singleton.h"
#include "System.h"
#include <memory>
#include <string>

namespace Ogre {
class Camera;
class Viewport;
class SceneManager;
}  // namespace Ogre

namespace glue {
class GBufferSchemeHandler;
}

namespace glue {

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

 protected:
  void InitGbuffer();
  void AddCompositorEnabled(const std::string &name);
  void AddCompositorDisabled(const std::string &name);
  void EnableCompositor(const std::string &name);
  void InitMRT();
  void InitOutput();

  std::map<std::string, bool> effects;
  std::unique_ptr<GBufferSchemeHandler> gbuff_handler;
  ViewPtr<Ogre::CompositorManager> compositor_manager;
  ViewPtr<Ogre::SceneManager> scene;
  ViewPtr<Ogre::Camera> camera;
  ViewPtr<Ogre::Viewport> viewport;
};

}  // namespace glue
