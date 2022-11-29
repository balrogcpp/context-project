// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include "System.h"
#include <Ogre.h>

namespace Glue {
class CompositorManager : public System<CompositorManager>, public Ogre::Viewport::Listener {
 public:
  CompositorManager();
  virtual ~CompositorManager();

  void AddCompositor(const std::string& name, bool enable, int position = -1);
  void SetCompositorEnabled(const std::string& name, bool enable);

  void SetFixedViewportSize(int x, int y);

 protected:
  /// System impl
  void OnSetUp() override;
  void OnClean() override;
  void OnUpdate(float time) override;

  void InitMRT(bool enable = true);
  void InitNoMRT(bool enable = true);
  void InitMipChain(bool enable = false);

  /// Ogre::Viewport::Listener impl
  void viewportCameraChanged(Ogre::Viewport* viewport) override;
  void viewportDimensionsChanged(Ogre::Viewport* viewport) override;

  const std::string MRT_COMPOSITOR;
  const std::string BLOOM_COMPOSITOR;
  const int mipChain;
  bool fixedViewportSize;
  int forceSizeX, forceSizeY;

  Ogre::CompositorManager* compositorManager = nullptr;
  Ogre::CompositorChain* compositorChain = nullptr;
  Ogre::SceneManager* ogreSceneManager = nullptr;
  Ogre::Camera* ogreCamera = nullptr;
  Ogre::Viewport* ogreViewport = nullptr;
};
}  // namespace Glue
