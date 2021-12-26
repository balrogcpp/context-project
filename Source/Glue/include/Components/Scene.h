// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Components/Component.h"

namespace Ogre {
class Camera;
class SceneManager;
class SceneNode;
class TerrainGroup;
class TerrainGlobalOptions;
class VertexDeclaration;
}  // namespace Ogre

namespace Forests {
class PagedGeometry;
class PageLoader;
class GeometryPage;
}  // namespace Forests

namespace Glue {
class CameraMan;
class Conf;
class Physics;
class Sound;
class Overlay;
class SinbadCharacterController;
}  // namespace Glue

namespace Glue {

class Scene final : public Component<Scene> {
 public:
  Scene();
  virtual ~Scene();

  void OnClean() override;
  void OnUpdate(float PassedTime) override;

  CameraMan &GetCamera() const;
  float GetHeight(float x, float z);
  void AddCamera(Ogre::Camera *OgreCameraPtr);
  void AddSinbad(Ogre::Camera *OgreCameraPtr);
  void AddForests(Forests::PagedGeometry *PGPtr);
  void AddTerrain(Ogre::TerrainGroup* TGP);

 protected:
  std::unique_ptr<CameraMan> CameraManPtr;
  std::unique_ptr<Ogre::TerrainGroup> OgreTerrainList;
  std::vector<std::unique_ptr<Forests::PagedGeometry>> PagedGeometryList;
  std::unique_ptr<SinbadCharacterController> Sinbad;

  Ogre::SceneManager *OgreScene = nullptr;
  Ogre::Root *OgreRoot = nullptr;
  Ogre::SceneNode *RootNode = nullptr;
  std::string GroupName = Ogre::RGN_DEFAULT;
};

}  // namespace Glue
