/// created by Andrey Vasiliev

#pragma once
#include "System.h"

namespace gge {


class TemplateManager final : public System<TemplateManager> {
 public:
  TemplateManager();
  virtual ~TemplateManager();

  /// System impl
  void OnSetUp() override;
  void OnUpdate(float time) override;
  void OnClean() override;

 protected:
  Ogre::Root *ogreRoot = nullptr;
  Ogre::SceneManager *sceneManager = nullptr;
  Ogre::Camera *camera = nullptr;
};


}  // namespace gge
