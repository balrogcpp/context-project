/// created by Andrey Vasiliev

#pragma once
#include "Component.h"
#include <Ogre.h>

namespace gge {

class SceneComponent final : public Component<SceneComponent> {
 public:
  SceneComponent();
  virtual ~SceneComponent();

  void LoadFromFile(const std::string &filename);
  void ProcessCamera(Ogre::Camera *camera);
  void ProcessLight(Ogre::Light *light);
  void ProcessEntity(Ogre::Entity *entity);
  void ProcessEntity(const std::string &name);
  void ProcessNode(Ogre::SceneNode *node);
  Ogre::SceneManager *GetOgreScene();

  /// System impl
  void OnSetUp() override;
  void OnClean() override;
  void OnUpdate(float time) override;

 protected:
  Ogre::Root *ogreRoot = nullptr;
  Ogre::SceneManager *sceneManager = nullptr;
  Ogre::Camera *camera = nullptr;
};

}  // namespace gge
