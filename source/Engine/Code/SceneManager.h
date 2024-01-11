/// created by Andrey Vasiliev

#pragma once
#include "SinbadCharacterController.h"
#include "System.h"
#include <Ogre.h>

namespace gge {

class SceneManager final : public System<SceneManager> {
 public:
  SceneManager();
  virtual ~SceneManager();

  void LoadFromFile(const std::string &filename);
  void ScanCamera(Ogre::Camera *camera);
  void ScanLight(Ogre::Light *light);
  void ScanEntity(Ogre::Entity *entity);
  void ScanEntity(const std::string &name);
  void ScanNode(Ogre::SceneNode *node);

  /// System impl
  void OnSetUp() override;
  void OnClean() override;
  void OnUpdate(float time) override;

 protected:
  std::unique_ptr<SinbadCharacterController> sinbad;

  Ogre::Root *ogreRoot = nullptr;
  Ogre::SceneManager *sceneManager = nullptr;
  Ogre::Camera *camera = nullptr;
};

}  // namespace gge
