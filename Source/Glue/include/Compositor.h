// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Component.h"
#include "LazySingleton.h"
#include <memory>
#include <string>

namespace Ogre {
class Camera;
class Viewport;
class SceneManager;
}  // namespace Ogre

namespace Glue {
class GBufferSchemeHandler;
}

namespace Glue {

class Compositor : public Component, public DynamicSingleton<Compositor> {
 public:
  Compositor();
  virtual ~Compositor();

  void Cleanup() override;
  void Pause() override;
  void Resume() override;
  void Update(float time) override;
  void EnableEffect(const std::string& name, bool enable = true);
  void SetUp();

 protected:
  void InitGbuffer();
  void AddCompositorEnabled(const std::string& name);
  void AddCompositorDisabled(const std::string& name);
  void EnableCompositor(const std::string& name);
  void InitMRT();
  void InitOutput();

  std::map<std::string, bool> EffectsList;
  std::unique_ptr<GBufferSchemeHandler> GBufferHandler;
  Ogre::CompositorManager* OgreCompositorManager = nullptr;
  Ogre::SceneManager* OgreSceneManager = nullptr;
  Ogre::Camera* OgreCamera = nullptr;
  Ogre::Viewport* OgreViewport = nullptr;
};

}  // namespace Glue
