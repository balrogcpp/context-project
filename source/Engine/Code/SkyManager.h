/// created by Andrey Vasiliev

#pragma once
#include "System.h"
#include <Ogre.h>

namespace gge {
class SkyManager final : public System<SkyManager> {
 public:
  SkyManager();
  virtual ~SkyManager();

  Ogre::Vector3 GetSunPosition();
  void SetUpSky();

 protected:
  /// System impl
  void OnSetUp() override;
  void OnUpdate(float time) override;
  void OnClean() override;

  bool needsUpdate;
  Ogre::GpuProgramParametersSharedPtr fpParams;
  const std::array<const char *, 10> hosekParamList;
  std::array<Ogre::Vector3, 10> hosekParams;

  Ogre::Root *ogreRoot = nullptr;
  Ogre::SceneManager *ogreSceneManager = nullptr;
  Ogre::Camera *ogreCamera = nullptr;
};
}  // namespace gge
