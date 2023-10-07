/// created by Andrey Vasiliev

#pragma once
#include "System.h"
#include <Ogre.h>

namespace Forests {
class PagedGeometry;
}

namespace gge {
class ForestsManager final : public System<ForestsManager> {
 public:
  ForestsManager();
  virtual ~ForestsManager();

  void RegPagedGeometry(Forests::PagedGeometry *pagedGeometry);
  void UnregPagedGeometry(Forests::PagedGeometry *pagedGeometry);

 protected:
  /// System impl
  void OnSetUp() override;
  void OnUpdate(Ogre::Real time) override;
  void OnClean() override;

  std::vector<Forests::PagedGeometry *> pagedGeometryList;

  Ogre::Root *ogreRoot = nullptr;
  Ogre::SceneManager *ogreSceneManager = nullptr;
  Ogre::Camera *ogreCamera = nullptr;
};
}  // namespace gge
