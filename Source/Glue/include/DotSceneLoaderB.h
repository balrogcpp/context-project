// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "CameraMan.h"
#include "CubeMapCamera.h"
#include "Input.h"
#include "Landscape.h"
#include "ReflectionCamera.h"
#include "Singleton.h"
#include "Component.h"
#include "VegetationSystem.h"
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

namespace Glue {
class CameraMan;
class Config;
class RenderSystem;
class PhysicsSystem;
class AudioSystem;
class Overlay;
class SinbadCharacterController;
}  // namespace glue

namespace Glue {

std::string GetAttrib(const pugi::xml_node &xml_node, const std::string &attrib, const std::string &defaultValue = "");
float GetAttribReal(const pugi::xml_node &xml_node, const std::string &attrib, float defaultValue = 0);
int GetAttribInt(const pugi::xml_node &xml_node, const std::string &attrib, int defaultValue = 0);
bool GetAttribBool(const pugi::xml_node &xml_node, const std::string &attrib, bool defaultValue = false);
Ogre::Vector3 ParseVector3(const pugi::xml_node &xml_node);
Ogre::Vector3 ParsePosition(const pugi::xml_node &xml_node);
Ogre::Vector3 ParseScale(const pugi::xml_node &xml_node);
Ogre::Quaternion ParseRotation(const pugi::xml_node &xml_node);
Ogre::ColourValue ParseColour(pugi::xml_node &xml_node);

class DotSceneLoaderB final : public Component, public Singleton<DotSceneLoaderB> {
 public:
  DotSceneLoaderB();
  virtual ~DotSceneLoaderB();

  void Cleanup() override;
  void Update(float time) override;

  void load(Ogre::DataStreamPtr &stream, const std::string &group_name, Ogre::SceneNode *root_node);
  void exportScene(Ogre::SceneNode *rootNode, const std::string &outFileName);
  float GetHeigh(float x, float z);

  static Landscape &GetTerrain();
  CameraMan &GetCamera() const;
  static VegetationSystem &GetForest();

 protected:
  void ProcessScene(pugi::xml_node &xml_root);
  void ProcessNodes(pugi::xml_node &xml_node);
  void ProcessExternals(pugi::xml_node &xml_node);
  void ProcessEnvironment(pugi::xml_node &xml_node);
  void ProcessUserData(pugi::xml_node &xml_node, Ogre::UserObjectBindings &user_object_bindings);
  void ProcessLight(pugi::xml_node &xml_node, Ogre::SceneNode *parent = nullptr);
  void ProcessCamera(pugi::xml_node &xml_node, Ogre::SceneNode *parent = nullptr);
  void ProcessNode(pugi::xml_node &xml_node, Ogre::SceneNode *parent = nullptr);
  void ProcessLookTarget(pugi::xml_node &xml_node, Ogre::SceneNode *parent);
  void ProcessTrackTarget(pugi::xml_node &xml_node, Ogre::SceneNode *parent);
  void ProcessEntity(pugi::xml_node &xml_node, Ogre::SceneNode *parent);
  void ProcessParticleSystem(pugi::xml_node &xml_node, Ogre::SceneNode *parent);
  void ProcessBillboardSet(pugi::xml_node &xml_node, Ogre::SceneNode *parent);
  void ProcessPlane(pugi::xml_node &xml_node, Ogre::SceneNode *parent);
  void ProcessForest(pugi::xml_node &xml_node);
  void ProcessLandscape(pugi::xml_node &xml_node);
  void ProcessFog(pugi::xml_node &xml_node);
  void ProcessSkyBox(pugi::xml_node &xml_node);
  void ProcessSkyDome(pugi::xml_node &xml_node);
  void ProcessSkyPlane(pugi::xml_node &xml_node);
  void ProcessLightRange(pugi::xml_node &xml_node, Ogre::Light *light);
  void ProcessLightAttenuation(pugi::xml_node &xml_node, Ogre::Light *light);
  void WriteNode(pugi::xml_node &parentXML, const Ogre::SceneNode *node);

  std::unique_ptr<ReflectionCamera> rcamera;
  std::unique_ptr<CubeMapCamera> ccamera;
  std::unique_ptr<CameraMan> camera_man;

  Ogre::SceneManager* ogre_scene;
  Ogre::Root* root;
  Ogre::SceneNode* root_node;
  Ogre::SceneNode* attach_node;
  std::string group_name = Ogre::RGN_DEFAULT;

  std::unique_ptr<SinbadCharacterController> sinbad;

  static inline std::unique_ptr<Landscape> terrain;
  static inline std::unique_ptr<VegetationSystem> forest;
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
}  // namespace glue
