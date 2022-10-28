// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include "System.h"
#include <Ogre.h>

namespace Glue {

class CompositorManager : public System<CompositorManager>, public Ogre::Viewport::Listener {
 public:
  CompositorManager();
  virtual ~CompositorManager();

  void OnSetUp() override;
  void OnClean() override;
  void OnUpdate(float time) override;

  void AddCompositor(const std::string& name, bool enable, int position = -1);
  void SetCompositorEnabled(const std::string& name, bool enable);

 protected:
  void InitMRT();

  void viewportDimensionsChanged(Ogre::Viewport* viewport) override;

  Ogre::CompositorManager* compositorManager = nullptr;
  Ogre::CompositorChain* compositorChain = nullptr;
  Ogre::SceneManager* ogreSceneManager = nullptr;
  Ogre::Camera* ogreCamera = nullptr;
  Ogre::Viewport* ogreViewport = nullptr;
  const std::string MRT_COMPOSITOR;
  std::vector<std::string> compositorList;
};

}  // namespace Glue
