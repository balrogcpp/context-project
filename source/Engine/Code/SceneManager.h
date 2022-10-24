/// created by Andrey Vasiliev

#pragma once

#include "SinbadCharacterController.h"
#include "System.h"
#include <Ogre.h>

namespace Glue {

class SceneManager final : public System<SceneManager> {
 public:
  SceneManager();
  virtual ~SceneManager();

  void OnSetUp() override;
  void OnClean() override;
  void OnPause() override;
  void OnResume() override;
  void OnUpdate(float time) override;

  void LoadFromFile(const std::string filename);
  Ogre::Vector3 GetSunPosition();
  void SetUpSky();
  void RegCamera(Ogre::Camera *camera);
  void RegSinbad(Ogre::Camera *camera);
  void RegEntity(Ogre::Entity *entity);
  void RegMaterial(const Ogre::MaterialPtr &material);
  void RegMaterial(const std::string &name);

 protected:
  Ogre::Root *ogreRoot = nullptr;
  Ogre::SceneManager *sceneManager = nullptr;
  Ogre::Camera *camera = nullptr;
  std::unique_ptr<SinbadCharacterController> sinbad;
  bool skyNeedsUpdate = false;
  Ogre::GpuProgramParametersSharedPtr skyBoxFpParams;
  const std::array<const char *, 10> hosekParamList{"A", "B", "C", "D", "E", "F", "G", "H", "I", "Z"};
  std::array<Ogre::Vector3, 10> hosekParams;
  std::vector<Ogre::GpuProgramParametersSharedPtr> gpuFpParams;
  std::vector<Ogre::GpuProgramParametersSharedPtr> gpuVpParams;
};

}  // namespace Glue
