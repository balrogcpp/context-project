// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "CameraMan.h"
#include "CubeMapCamera.h"
#include "Input.h"
#include "Landscape.h"
#include "ReflectionCamera.h"
#include "Scene.h"
#include "Singleton.h"
#include "System.h"
#include "VegetationSystem.h"
#include "view_ptr.h"
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

namespace glue {
class CameraMan;
class Config;
class RenderSystem;
class PhysicsSystem;
class AudioSystem;
class Overlay;
class SinbadCharacterController;
}  // namespace glue

namespace glue {

std::string GetAttrib(const pugi::xml_node &xml_node, const std::string &attrib, const std::string &defaultValue = "");
float GetAttribReal(const pugi::xml_node &xml_node, const std::string &attrib, float defaultValue = 0);
int GetAttribInt(const pugi::xml_node &xml_node, const std::string &attrib, int defaultValue = 0);
bool GetAttribBool(const pugi::xml_node &xml_node, const std::string &attrib, bool defaultValue = false);
Ogre::Vector3 ParseVector3(const pugi::xml_node &xml_node);
Ogre::Vector3 ParsePosition(const pugi::xml_node &xml_node);
Ogre::Vector3 ParseScale(const pugi::xml_node &xml_node);
Ogre::Quaternion ParseRotation(const pugi::xml_node &xml_node);
Ogre::ColourValue ParseColour(pugi::xml_node &xml_node);

class DotSceneLoaderB final : public System, public Singleton<DotSceneLoaderB> {
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
  void ProcessScene_(pugi::xml_node &xml_root);
  void ProcessNodes_(pugi::xml_node &xml_node);
  void ProcessExternals_(pugi::xml_node &xml_node);
  void ProcessEnvironment_(pugi::xml_node &xml_node);
  void ProcessUserData_(pugi::xml_node &xml_node, Ogre::UserObjectBindings &user_object_bindings);
  void ProcessLight_(pugi::xml_node &xml_node, Ogre::SceneNode *parent = nullptr);
  void ProcessCamera_(pugi::xml_node &xml_node, Ogre::SceneNode *parent = nullptr);
  void ProcessNode_(pugi::xml_node &xml_node, Ogre::SceneNode *parent = nullptr);
  void ProcessLookTarget_(pugi::xml_node &xml_node, Ogre::SceneNode *parent);
  void ProcessTrackTarget_(pugi::xml_node &xml_node, Ogre::SceneNode *parent);
  void ProcessEntity_(pugi::xml_node &xml_node, Ogre::SceneNode *parent);
  void ProcessParticleSystem_(pugi::xml_node &xml_node, Ogre::SceneNode *parent);
  void ProcessBillboardSet_(pugi::xml_node &xml_node, Ogre::SceneNode *parent);
  void ProcessPlane_(pugi::xml_node &xml_node, Ogre::SceneNode *parent);
  void ProcessForest_(pugi::xml_node &xml_node);
  void ProcessLandscape_(pugi::xml_node &xml_node);
  void ProcessFog_(pugi::xml_node &xml_node);
  void ProcessSkyBox_(pugi::xml_node &xml_node);
  void ProcessSkyDome_(pugi::xml_node &xml_node);
  void ProcessSkyPlane_(pugi::xml_node &xml_node);
  void ProcessLightRange_(pugi::xml_node &xml_node, Ogre::Light *light);
  void ProcessLightAttenuation_(pugi::xml_node &xml_node, Ogre::Light *light);
  void WriteNode_(pugi::xml_node &parentXML, const Ogre::SceneNode *node);

  std::unique_ptr<ReflectionCamera> rcamera_;
  std::unique_ptr<CubeMapCamera> ccamera_;
  std::unique_ptr<CameraMan> camera_;
  std::unique_ptr<Scene> scene_;

  view_ptr<Ogre::SceneManager> ogre_scene_;
  view_ptr<Ogre::Root> root_;
  view_ptr<Ogre::SceneNode> root_node_;
  view_ptr<Ogre::SceneNode> attach_node_;
  std::string group_name_ = Ogre::RGN_DEFAULT;

  std::unique_ptr<SinbadCharacterController> sinbad_;

  static inline std::unique_ptr<Landscape> terrain_;
  static inline std::unique_ptr<VegetationSystem> forest_;
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
