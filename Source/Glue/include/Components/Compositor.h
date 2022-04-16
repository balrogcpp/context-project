// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include "Components/Component.h"
#include <memory>
#include <string>

namespace Ogre {
class Camera;
class Viewport;
class SceneManager;
class CompositorChain;
}  // namespace Ogre

namespace Glue {

class Compositor : public Component<Compositor> {
 public:
  Compositor();
  virtual ~Compositor();

  void OnClean() override;
  void OnPause() override;
  void OnResume() override;
  void OnUpdate(float time) override;
  void EnableEffect(const std::string& name, bool enable = true);
  void SetUp();

 protected:
  void AddCompositorEnabled(const std::string& name);
  void AddCompositorDisabled(const std::string& name);
  void EnableCompositor(const std::string& name);
  void InitMRT();
  void InitOutput();
  void InitDummyOutput();

  bool AnyEffectEnabled = false;
  std::map<std::string, bool> EffectsList;
  Ogre::CompositorManager* OgreCompositorManager = nullptr;
  Ogre::CompositorChain *OgreCompositorChain = nullptr;
  Ogre::SceneManager* OgreSceneManager = nullptr;
  Ogre::Camera* OgreCamera = nullptr;
  Ogre::Viewport* OgreViewport = nullptr;
};

}  // namespace Glue
