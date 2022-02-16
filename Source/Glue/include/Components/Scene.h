// This source file is part of "glue project". Created by Andrey Vasiliev

#pragma once
#include "Components/Component.h"
#include <OgrePrerequisites.h>
#include <array>

namespace Ogre {
class TerrainGroup;
}  // namespace Ogre

namespace Forests {
class PagedGeometry;
class PageLoader;
class GeometryPage;
}  // namespace Forests

namespace Glue {
class CameraMan;
class Config;
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
  Ogre::Vector3 GetSunPosition();

  float GetHeight(float x, float z);
  void AddEntity(Ogre::Entity *EntityPtr);
  void AddMaterial(Ogre::MaterialPtr material);
  void AddMaterial(const std::string &MaterialName);
  void AddCamera(Ogre::Camera *OgreCameraPtr);
  void AddSinbad(Ogre::Camera *OgreCameraPtr);
  void AddForests(Forests::PagedGeometry *PGPtr, const std::string &MaterialName = "");
  void AddTerrain(Ogre::TerrainGroup* TGP);
  void AddSkyBox();

 protected:
  std::unique_ptr<CameraMan> CameraManPtr;
  std::unique_ptr<Ogre::TerrainGroup> OgreTerrainList;
  std::vector<std::unique_ptr<Forests::PagedGeometry>> PagedGeometryList;
  std::unique_ptr<SinbadCharacterController> Sinbad;
  Ogre::GpuProgramParametersSharedPtr SkyBoxFpParams;
  bool SkyNeedsUpdate = false;
  static constexpr std::array<const char *, 10> HosikParamList{"A", "B", "C", "D", "E", "F", "G", "H", "I", "Z"};
  std::array<Ogre::Vector3, 10> HosekParams;
  std::vector<Ogre::GpuProgramParametersSharedPtr> gpu_fp_params_;
  std::vector<Ogre::GpuProgramParametersSharedPtr> gpu_vp_params_;

  Ogre::SceneManager *OgreScene = nullptr;
  Ogre::Root *OgreRoot = nullptr;
  Ogre::SceneNode *RootNode = nullptr;
  std::string GroupName = Ogre::RGN_DEFAULT;
};

}  // namespace Glue
