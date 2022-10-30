/// created by Andrey Vasiliev

#pragma once
#include "System.h"

namespace Glue {
class ForestsManager final : public System<ForestsManager> {
 public:
  ForestsManager();
  virtual ~ForestsManager();

 protected:
  /// System impl
  void OnSetUp() override;
  void OnUpdate(float time) override;
  void OnClean() override;

  Ogre::Root *ogreRoot = nullptr;
  Ogre::SceneManager *ogreSceneManager = nullptr;
  Ogre::Camera *ogreCamera = nullptr;
};
}
