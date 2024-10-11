/// created by Andrey Vasiliev

#pragma once
#include "Component.h"

namespace gge {


class TemplateManager final : public Component<TemplateManager> {
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
