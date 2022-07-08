// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include "CameraMan.h"
#include "Components/Component.h"
#include "CubeMapCamera.h"
#include "Input/InputSequencer.h"
#include "ReflectionCamera.h"
#include "Singleton.h"
#include <OgreCodec.h>
#include <OgrePlugin.h>
#include <OgreVector.h>
#include <memory>
#include <string>

namespace pugi {
class xml_node;
}

namespace Ogre {
class Camera;
class SceneManager;
class SceneNode;
class TerrainGroup;
class TerrainGlobalOptions;
class VertexDeclaration;
}  // namespace Ogre

namespace Glue {
class CameraMan;
class Config;
class Physics;
class Sound;
class Overlay;
class SinbadCharacterController;
}  // namespace Glue

namespace Glue {

class DotSceneLoaderB final {
 public:
  DotSceneLoaderB();
  virtual ~DotSceneLoaderB();

  void Load(Ogre::DataStreamPtr &stream, const std::string &group_name, Ogre::SceneNode *root_node);
  void ExportScene(Ogre::SceneNode *rootNode, const std::string &outFileName);

 protected:
  void WriteNode(pugi::xml_node &parentXML, const Ogre::SceneNode *node);
  void ProcessScene(pugi::xml_node &XmlRoot);
  void ProcessNodes(pugi::xml_node &XmlNode);
  void ProcessExternals(pugi::xml_node &XmlNode);
  void ProcessEnvironment(pugi::xml_node &XmlNode);
  void ProcessTerrainGroup(pugi::xml_node &XmlNode);
  // void ProcessBlendmaps(pugi::xml_node& XmlNode);
  void ProcessUserData(pugi::xml_node &XmlNode, Ogre::UserObjectBindings &user_object_bindings);
  void ProcessLight(pugi::xml_node &XmlNode, Ogre::SceneNode *ParentNode = nullptr);
  void ProcessCamera(pugi::xml_node &XmlNode, Ogre::SceneNode *ParentNode = nullptr);
  void ProcessForests(pugi::xml_node &XmlNode);
  void ProcessStaticGeometry(pugi::xml_node &XmlNode);
  void ProcessNode(pugi::xml_node &XmlNode, Ogre::SceneNode *ParentNode = nullptr);
  void ProcessLookTarget(pugi::xml_node &XmlNode, Ogre::SceneNode *ParentNode);
  void ProcessTrackTarget(pugi::xml_node &XmlNode, Ogre::SceneNode *ParentNode);
  void ProcessEntity(pugi::xml_node &XmlNode, Ogre::SceneNode *ParentNode);
  void ProcessParticleSystem(pugi::xml_node &XmlNode, Ogre::SceneNode *ParentNode);
  void ProcessBillboardSet(pugi::xml_node &XmlNode, Ogre::SceneNode *ParentNode);
  void ProcessPlane(pugi::xml_node &XmlNode, Ogre::SceneNode *ParentNode);
  void ProcessNodeAnimations(pugi::xml_node &XmlNode, Ogre::SceneNode *ParentNode);
  void ProcessNodeAnimation(pugi::xml_node &XmlNode, Ogre::SceneNode *ParentNode);
  void ProcessKeyframe(pugi::xml_node &XmlNode, Ogre::NodeAnimationTrack *ParentNode);
  void ProcessFog(pugi::xml_node &XmlNode);
  void ProcessSkyBox(pugi::xml_node &XmlNode);
  void ProcessSkyDome(pugi::xml_node &XmlNode);
  void ProcessSkyPlane(pugi::xml_node &XmlNode);
  void ProcessLightRange(pugi::xml_node &XmlNode, Ogre::Light *light);
  void ProcessLightAttenuation(pugi::xml_node &XmlNode, Ogre::Light *light);

  Ogre::Camera *OgreCameraPtr = nullptr;
  Ogre::SceneManager *OgreScene = nullptr;
  Ogre::Root *OgreRoot = nullptr;
  Ogre::SceneNode *RootNode = nullptr;
  Ogre::SceneNode *AttachNode = nullptr;
  std::string GroupName = Ogre::RGN_AUTODETECT;
};

class DotScenePluginB : public Ogre::Plugin {
  const Ogre::String &getName() const;

  void install() override;
  void initialise() override;
  void shutdown() override;
  void uninstall() override;

  Ogre::Codec *mCodec = nullptr;
};

}  // namespace Glue
