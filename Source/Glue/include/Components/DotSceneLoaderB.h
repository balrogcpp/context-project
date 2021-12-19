// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Components/Component.h"
#include "CubeMapCamera.h"
#include "LazySingleton.h"
#include "Objects/CameraMan.h"
#include "Input/InputSequencer.h"
#include "ReflectionCamera.h"
#include <OgreCodec.h>
#include <OgrePlugin.h>
#include <OgreVector.h>
#include <memory>
#include <string>

namespace pugi {
class xml_node;
}

namespace Ogre {
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

class DotSceneLoaderB final : public Component, public Singleton<DotSceneLoaderB> {
 public:
  DotSceneLoaderB();
  virtual ~DotSceneLoaderB();

  void Cleanup() override;
  void Update(float PassedTime) override;

  void Load(Ogre::DataStreamPtr &stream, const std::string &group_name, Ogre::SceneNode *root_node);
  void ExportScene(Ogre::SceneNode *rootNode, const std::string &outFileName);
  float GetHeight(float x, float z);
  CameraMan &GetCamera() const;

 protected:
  void ProcessScene(pugi::xml_node &XmlRoot);
  void ProcessNodes(pugi::xml_node &XmlNode);
  void ProcessExternals(pugi::xml_node &XmlNode);
  void ProcessEnvironment(pugi::xml_node &XmlNode);
  void ProcessUserData(pugi::xml_node &XmlNode, Ogre::UserObjectBindings &user_object_bindings);
  void ProcessLight(pugi::xml_node &XmlNode, Ogre::SceneNode *ParentNode = nullptr);
  void ProcessCamera(pugi::xml_node &XmlNode, Ogre::SceneNode *ParentNode = nullptr);
  void ProcessNode(pugi::xml_node &XmlNode, Ogre::SceneNode *ParentNode = nullptr);
  void ProcessLookTarget(pugi::xml_node &XmlNode, Ogre::SceneNode *ParentNode);
  void ProcessTrackTarget(pugi::xml_node &XmlNode, Ogre::SceneNode *ParentNode);
  void ProcessEntity(pugi::xml_node &XmlNode, Ogre::SceneNode *ParentNode);
  void ProcessParticleSystem(pugi::xml_node &XmlNode, Ogre::SceneNode *ParentNode);
  void ProcessBillboardSet(pugi::xml_node &XmlNode, Ogre::SceneNode *ParentNode);
  void ProcessPlane(pugi::xml_node &XmlNode, Ogre::SceneNode *ParentNode);
  void ProcessForest(pugi::xml_node &XmlNode);
  void ProcessTerrain(pugi::xml_node &XmlNode);
  void ProcessFog(pugi::xml_node &XmlNode);
  void ProcessSkyBox(pugi::xml_node &XmlNode);
  void ProcessSkyDome(pugi::xml_node &XmlNode);
  void ProcessSkyPlane(pugi::xml_node &XmlNode);
  void ProcessLightRange(pugi::xml_node &XmlNode, Ogre::Light *light);
  void ProcessLightAttenuation(pugi::xml_node &XmlNode, Ogre::Light *light);
  void WriteNode(pugi::xml_node &parentXML, const Ogre::SceneNode *node);

  std::unique_ptr<CameraMan> CameraManPtr;
  std::unique_ptr<Ogre::TerrainGroup> OgreTerrainPtr;
  std::vector<std::unique_ptr<Forests::PagedGeometry>> PGeometryList;

  Ogre::SceneManager *OgreScene = nullptr;
  Ogre::Root *OgreRoot = nullptr;
  Ogre::SceneNode *AttachNode = nullptr;
  std::string GroupName = Ogre::RGN_DEFAULT;
  std::unique_ptr<SinbadCharacterController> Sinbad;
};

class DotScenePluginB : public Ogre::Plugin {
  const Ogre::String &getName() const;

  void install() override;
  void initialise() override;
  void shutdown() override;
  void uninstall() override;

 protected:
  Ogre::Codec *mCodec = nullptr;
};

}  // namespace Glue
