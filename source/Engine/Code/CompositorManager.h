// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include "System.h"
#include <Ogre.h>

namespace Glue {

class CompositorManager : public System<CompositorManager> {
 public:
  CompositorManager();
  virtual ~CompositorManager();

  void OnSetUp() override;
  void OnClean() override;
  void OnPause() override;
  void OnResume() override;
  void OnUpdate(float time) override;

 protected:
  void InitMRT();
  void AddCompositorEnabled(const std::string& name);
  void AddCompositorDisabled(const std::string& name);
  void EnableCompositor(const std::string& name);

  Ogre::CompositorManager* compositorManager = nullptr;
  Ogre::CompositorChain* compositorChain = nullptr;
  Ogre::SceneManager* sceneManager = nullptr;
  Ogre::Camera* camera = nullptr;
  Ogre::Viewport* viewport = nullptr;
};

}  // namespace Glue
