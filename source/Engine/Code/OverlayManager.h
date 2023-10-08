/// created by Andrey Vasiliev

#pragma once
#include "System.h"

namespace gge {
class OverlayManager final : public System<OverlayManager> {
 public:
  OverlayManager();
  virtual ~OverlayManager();

  /// System impl
  void OnSetUp() override;
  void OnUpdate(float time) override;
  void OnClean() override;

 protected:
  Ogre::Root *ogreRoot = nullptr;
  Ogre::SceneManager *ogreSceneManager = nullptr;
  Ogre::Camera *ogreCamera = nullptr;
};
}  // namespace gge
