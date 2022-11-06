/// created by Andrey Vasiliev

#pragma once
#include "SinbadCharacterController.h"
#include "System.h"
#include <Ogre.h>

namespace Glue {
class SceneManager final : public System<SceneManager>, public Ogre::RenderObjectListener {
 public:
  SceneManager();
  virtual ~SceneManager();

  void LoadFromFile(const std::string &filename);
  void RegCamera(Ogre::Camera *camera);
  void RegLight(Ogre::Light *light);
  void RegEntity(Ogre::Entity *entity);
  void RegEntity(const std::string &name);
  void RegMaterial(const Ogre::Material *material);
  void UnregMaterial(const Ogre::Material *material);
  void RegMaterial(const std::string &name);
  void UnregMaterial(const std::string &name);
  void ScanNode(Ogre::SceneNode *node);

 protected:
  /// System impl
  void OnSetUp() override;
  void OnClean() override;
  void OnUpdate(float time) override;

  /// Ogre::RenderObjectListener impl
  void notifyRenderSingleObject(Ogre::Renderable *rend, const Ogre::Pass *pass, const Ogre::AutoParamDataSource *source,
                                const Ogre::LightList *pLightList, bool suppressRenderStateChanges) override;

  std::unique_ptr<SinbadCharacterController> sinbad;
  std::vector<Ogre::Entity *> entityList;
  std::vector<Ogre::GpuProgramParametersSharedPtr> fpParams;
  std::vector<Ogre::GpuProgramParametersSharedPtr> vpParams;

  Ogre::Root *ogreRoot = nullptr;
  Ogre::SceneManager *ogreSceneManager = nullptr;
  Ogre::Camera *ogreCamera = nullptr;
};
}  // namespace Glue
