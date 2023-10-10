/// created by Andrey Vasiliev

#pragma once
#include "SinbadCharacterController.h"
#include "System.h"
#include <Ogre.h>

namespace gge {
class SceneManager final : public System<SceneManager>, public Ogre::RenderObjectListener {
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
  /// Ogre::RenderObjectListener impl
  void notifyRenderSingleObject(Ogre::Renderable *rend, const Ogre::Pass *pass, const Ogre::AutoParamDataSource *source,
                                const Ogre::LightList *pLightList, bool suppressRenderStateChanges) override;

  std::unique_ptr<SinbadCharacterController> sinbad;
  std::vector<Ogre::Entity *> entityList;

  Ogre::Root *ogreRoot = nullptr;
  Ogre::SceneManager *ogreSceneManager = nullptr;
  Ogre::Camera *ogreCamera = nullptr;

  bool isEven;
  Ogre::Matrix4 viewProj;
  Ogre::Matrix4 viewProjPrev;
  Ogre::uint32 pssmCount;
  Ogre::Vector4 pssmPoints;
};
}  // namespace gge
