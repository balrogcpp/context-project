// This source file is part of context-project
// Created by Andrew Vasiliev

#include "DotSceneLoaderB.h"

#include <OgreSceneLoaderManager.h>

#include <pugixml.hpp>

#include "ComponentLocator.h"
#include "MeshUtils.h"
#include "PbrShaderUtils.h"
#include "SinbadCharacterController.h"
#include "btogre/BtOgre.h"

// LOD Generator
#ifdef OGRE_BUILD_COMPONENT_MESHLODGENERATOR
#include <MeshLodGenerator/OgreLodConfig.h>
#include <MeshLodGenerator/OgreMeshLodGenerator.h>
#include <OgreDistanceLodStrategy.h>
#include <OgrePixelCountLodStrategy.h>
#endif
// Paging
#ifdef OGRE_BUILD_COMPONENT_PAGING
#include <Paging/OgrePage.h>
#include <Paging/OgrePaging.h>
#endif

using namespace std;
using SC = Ogre::StringConverter;

namespace xio {

//----------------------------------------------------------------------------------------------------------------------
string GetAttrib(const pugi::xml_node &xml_node, const string &attrib, const string &defaultValue) {
  if (auto anode = xml_node.attribute(attrib.c_str())) {
    return anode.value();
  } else {
    return defaultValue;
  }
}

//----------------------------------------------------------------------------------------------------------------------
float GetAttribReal(const pugi::xml_node &xml_node, const string &attrib, float defaultValue) {
  if (auto anode = xml_node.attribute(attrib.c_str())) {
    return SC::parseReal(anode.value());
  } else {
    return defaultValue;
  }
}

//----------------------------------------------------------------------------------------------------------------------
int GetAttribInt(const pugi::xml_node &xml_node, const string &attrib, int defaultValue) {
  if (auto anode = xml_node.attribute(attrib.c_str())) {
    return SC::parseInt(anode.value());
  } else {
    return defaultValue;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool GetAttribBool(const pugi::xml_node &xml_node, const string &attrib, bool defaultValue) {
  if (auto anode = xml_node.attribute(attrib.c_str())) {
    return anode.as_bool();
  } else {
    return defaultValue;
  }
}

//----------------------------------------------------------------------------------------------------------------------
Ogre::Vector3 ParseVector3(const pugi::xml_node &xml_node) {
  float x = SC::parseReal(xml_node.attribute("x").value());
  float y = SC::parseReal(xml_node.attribute("y").value());
  float z = SC::parseReal(xml_node.attribute("z").value());

  return Ogre::Vector3(x, y, z);
}

//----------------------------------------------------------------------------------------------------------------------
Ogre::Vector3 ParsePosition(const pugi::xml_node &xml_node) {
  float x = SC::parseReal(xml_node.attribute("x").value());
  float y = SC::parseReal(xml_node.attribute("y").value());
  float z = SC::parseReal(xml_node.attribute("z").value());

  return Ogre::Vector3(x, y, z);
}

//----------------------------------------------------------------------------------------------------------------------
Ogre::Vector3 ParseScale(const pugi::xml_node &xml_node) {
  float x = SC::parseReal(xml_node.attribute("x").value());
  float y = SC::parseReal(xml_node.attribute("y").value());
  float z = SC::parseReal(xml_node.attribute("z").value());

  return Ogre::Vector3(x, y, z);
}

//----------------------------------------------------------------------------------------------------------------------
Ogre::Quaternion ParseRotation(const pugi::xml_node &xml_node) {
  Ogre::Quaternion orientation;

  if (xml_node.attribute("qw")) {
    orientation.w = SC::parseReal(xml_node.attribute("qw").value());
    orientation.x = SC::parseReal(xml_node.attribute("qx").value());
    orientation.y = -SC::parseReal(xml_node.attribute("qz").value());
    orientation.z = SC::parseReal(xml_node.attribute("qy").value());
  } else if (xml_node.attribute("axisX")) {
    Ogre::Vector3 axis;
    axis.x = SC::parseReal(xml_node.attribute("axisX").value());
    axis.y = SC::parseReal(xml_node.attribute("axisY").value());
    axis.z = SC::parseReal(xml_node.attribute("axisZ").value());
    float angle = SC::parseReal(xml_node.attribute("angle").value());

    orientation.FromAngleAxis(Ogre::Angle(angle), axis);
  } else if (xml_node.attribute("angleX")) {
    Ogre::Matrix3 rot;
    rot.FromEulerAnglesXYZ(SC::parseAngle(xml_node.attribute("angleX").value()),
                           SC::parseAngle(xml_node.attribute("angleY").value()),
                           SC::parseAngle(xml_node.attribute("angleZ").value()));
    orientation.FromRotationMatrix(rot);
  } else if (xml_node.attribute("x")) {
    orientation.x = SC::parseReal(xml_node.attribute("x").value());
    orientation.y = SC::parseReal(xml_node.attribute("y").value());
    orientation.z = SC::parseReal(xml_node.attribute("z").value());
    orientation.w = SC::parseReal(xml_node.attribute("w").value());
  } else if (xml_node.attribute("w")) {
    orientation.w = SC::parseReal(xml_node.attribute("w").value());
    orientation.x = SC::parseReal(xml_node.attribute("x").value());
    orientation.y = SC::parseReal(xml_node.attribute("y").value());
    orientation.z = SC::parseReal(xml_node.attribute("z").value());
  }

  Ogre::Vector3 direction = orientation * Ogre::Vector3::NEGATIVE_UNIT_Z;
  float x = direction.x;
  float y = direction.y;
  float z = direction.z;
  direction = Ogre::Vector3(x, z, -y).normalisedCopy();

  auto *scene = Ogre::Root::getSingleton().getSceneManager("Default");
  auto *node = scene->getRootSceneNode()->createChildSceneNode("tmp");
  node->setDirection(direction);
  orientation = node->getOrientation();
  scene->destroySceneNode(node);

  return orientation;
}

//----------------------------------------------------------------------------------------------------------------------
Ogre::ColourValue ParseColour(pugi::xml_node &xml_node) {
  return Ogre::ColourValue(
      SC::parseReal(xml_node.attribute("r").value()), SC::parseReal(xml_node.attribute("g").value()),
      SC::parseReal(xml_node.attribute("b").value()), SC::parseReal(xml_node.attribute("a").value(), 1));
}

//---------------------------------------------------------------------------------------------------------------------
static void write(pugi::xml_node &node, const Ogre::Vector3 &v) {
  node.append_attribute("x") = Ogre::StringConverter::toString(v.x).c_str();
  node.append_attribute("y") = Ogre::StringConverter::toString(v.y).c_str();
  node.append_attribute("z") = Ogre::StringConverter::toString(v.z).c_str();
}

//---------------------------------------------------------------------------------------------------------------------
static void write(pugi::xml_node &node, const Ogre::ColourValue &c) {
  node.append_attribute("r") = Ogre::StringConverter::toString(c.r).c_str();
  node.append_attribute("g") = Ogre::StringConverter::toString(c.g).c_str();
  node.append_attribute("b") = Ogre::StringConverter::toString(c.b).c_str();
  node.append_attribute("a") = Ogre::StringConverter::toString(c.a).c_str();
}

//---------------------------------------------------------------------------------------------------------------------
DotSceneLoaderB::DotSceneLoaderB() {
  static auto &sl = Ogre::SceneLoaderManager::getSingleton();

  if (sl._getSceneLoader("DotScene")) {
    sl.unregisterSceneLoader("DotScene");
  }

  if (sl._getSceneLoader("DotSceneB")) {
    sl.unregisterSceneLoader("DotSceneB");
  }

  sl.registerSceneLoader("DotSceneB", {".scene", ".xml"}, this);

  camera_ = make_unique<CameraMan>();
}

//---------------------------------------------------------------------------------------------------------------------
DotSceneLoaderB::~DotSceneLoaderB() { Cleanup(); }

//---------------------------------------------------------------------------------------------------------------------
Landscape &DotSceneLoaderB::GetTerrain() { return *terrain_; }

//---------------------------------------------------------------------------------------------------------------------
CameraMan &DotSceneLoaderB::GetCamera() const { return *camera_; }

//---------------------------------------------------------------------------------------------------------------------
VegetationSystem &DotSceneLoaderB::GetForest() { return *forest_; }

//---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::Update(float time) {
  if (paused_) {
    return;
  }

  if (camera_) camera_->Update(time);
  if (terrain_) terrain_->Update(time);
  if (forest_) forest_->Update(time);
  if (scene_) scene_->Update(time);
  if (sinbad_ && terrain_) sinbad_->Update(time);
}

//---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::load(Ogre::DataStreamPtr &stream, const string &group_name, Ogre::SceneNode *root_node) {
  group_name_ = group_name;
  ogre_scene_ = root_node->getCreator();

  pugi::xml_document XMLDoc;  // character type defaults to char

  auto result = XMLDoc.load_buffer(stream->getAsString().c_str(), stream->size());

  if (!result) {
    Ogre::LogManager::getSingleton().stream(Ogre::LML_CRITICAL) << "[DotSceneLoader] " << result.description();
    return;
  }

  // Grab the scene node
  auto XMLRoot = XMLDoc.child("scene");

  // Validate the File
  if (!XMLRoot.attribute("formatVersion")) {
    Ogre::LogManager::getSingleton().logError(
        "[DotSceneLoader] Invalid .scene File. Missing <scene "
        "formatVersion='x.y' >");
    return;
  }

  // figure out where to attach any nodes we Init
  root_ = Ogre::Root::getSingletonPtr();
  attach_node_ = root_node;

  if (!terrain_) terrain_ = make_unique<Landscape>();
  if (!forest_) forest_ = make_unique<VegetationSystem>();
  forest_->SetHeighFunc([](float x, float z) { return terrain_->GetHeigh(x, z); });
  assert(!scene_);
  scene_ = make_unique<Scene>();

  // Process the scene
  ProcessScene_(XMLRoot);
}

//---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::WriteNode_(pugi::xml_node &parentXML, const Ogre::SceneNode *node) {
  auto nodeXML = parentXML.append_child("node");
  if (!node->getName().empty()) nodeXML.append_attribute("name") = node->getName().c_str();

  auto pos = nodeXML.append_child("position");
  write(pos, node->getPosition());

  auto scale = nodeXML.append_child("scale");
  write(scale, node->getScale());

  auto rot = nodeXML.append_child("rotation");
  rot.append_attribute("qw") = Ogre::StringConverter::toString(node->getOrientation().w).c_str();
  rot.append_attribute("qx") = Ogre::StringConverter::toString(node->getOrientation().x).c_str();
  rot.append_attribute("qy") = Ogre::StringConverter::toString(node->getOrientation().y).c_str();
  rot.append_attribute("qz") = Ogre::StringConverter::toString(node->getOrientation().z).c_str();

  for (auto mo : node->getAttachedObjects()) {
    if (auto c = dynamic_cast<Ogre::Camera *>(mo)) {
      auto camera = nodeXML.append_child("camera");
      camera.append_attribute("name") = c->getName().c_str();
      auto clipping = camera.append_child("clipping");
      clipping.append_attribute("near") = Ogre::StringConverter::toString(c->getNearClipDistance()).c_str();
      clipping.append_attribute("far") = Ogre::StringConverter::toString(c->getFarClipDistance()).c_str();
      continue;
    }

    if (auto l = dynamic_cast<Ogre::Light *>(mo)) {
      auto light = nodeXML.append_child("light");
      light.append_attribute("name") = l->getName().c_str();
      light.append_attribute("castShadows") = Ogre::StringConverter::toString(l->getCastShadows()).c_str();

      if (!l->isVisible()) light.append_attribute("visible") = "false";

      auto diffuse = light.append_child("colourDiffuse");
      write(diffuse, l->getDiffuseColour());
      auto specular = light.append_child("colourSpecular");
      write(specular, l->getSpecularColour());
      switch (l->getType()) {
        case Ogre::Light::LT_POINT:
          light.append_attribute("type") = "point";
          break;
        case Ogre::Light::LT_DIRECTIONAL:
          light.append_attribute("type") = "directional";
          break;
        case Ogre::Light::LT_SPOTLIGHT:
          light.append_attribute("type") = "spot";
          break;
      }

      if (l->getType() != Ogre::Light::LT_DIRECTIONAL) {
        auto range = light.append_child("lightRange");
        range.append_attribute("inner") = Ogre::StringConverter::toString(l->getSpotlightInnerAngle()).c_str();
        range.append_attribute("outer") = Ogre::StringConverter::toString(l->getSpotlightOuterAngle()).c_str();
        range.append_attribute("falloff") = Ogre::StringConverter::toString(l->getSpotlightFalloff()).c_str();
        auto atten = light.append_child("lightAttenuation");
        atten.append_attribute("range") = Ogre::StringConverter::toString(l->getAttenuationRange()).c_str();
        atten.append_attribute("constant") = Ogre::StringConverter::toString(l->getAttenuationConstant()).c_str();
        atten.append_attribute("linear") = Ogre::StringConverter::toString(l->getAttenuationLinear()).c_str();
        atten.append_attribute("quadratic") = Ogre::StringConverter::toString(l->getAttenuationQuadric()).c_str();
      }

      continue;
    }

    if (auto e = dynamic_cast<Ogre::Entity *>(mo)) {
      auto entity = nodeXML.append_child("entity");
      entity.append_attribute("name") = e->getName().c_str();
      entity.append_attribute("meshFile") = e->getMesh()->getName().c_str();

      if (!e->isVisible()) entity.append_attribute("visible") = "false";

      // Heuristic: assume first submesh is representative
      auto sub0mat = e->getSubEntity(0)->getMaterial();
      if (sub0mat != e->getMesh()->getSubMesh(0)->getMaterial())
        entity.append_attribute("material") = sub0mat->getName().c_str();
      continue;
    }

    Ogre::LogManager::getSingleton().logWarning("DotSceneLoader - unsupported MovableType " + mo->getMovableType());
  }

  // recurse
  for (auto c : node->getChildren()) WriteNode_(nodeXML, static_cast<Ogre::SceneNode *>(c));
}

//---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::exportScene(Ogre::SceneNode *rootNode, const string &outFileName) {
  pugi::xml_document XMLDoc;  // character type defaults to char
  auto comment = XMLDoc.append_child(pugi::node_comment);
  comment.set_value(Ogre::StringUtil::format(" exporter: Plugin_DotScene %d.%d.%d ", OGRE_VERSION_MAJOR, OGRE_VERSION_MINOR,
                                       OGRE_VERSION_PATCH)
                        .c_str());
  auto scene = XMLDoc.append_child("scene");
  scene.append_attribute("formatVersion") = "1.1";
  scene.append_attribute("sceneManager") = rootNode->getCreator()->getTypeName().c_str();

  auto nodes = scene.append_child("nodes");

  for (auto c : rootNode->getChildren()) WriteNode_(nodes, static_cast<Ogre::SceneNode *>(c));

  // writeNode(nodes, rootNode);

  XMLDoc.save_file(outFileName.c_str());
}

//---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::Cleanup() {
  terrain_.reset();
  forest_.reset();
  sinbad_.reset();
  //  Pbr::Cleanup();
  if (ogre_scene_) ogre_scene_->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
  if (ogre_scene_) ogre_scene_->clearScene();
  if (camera_) camera_->SetStyle(CameraMan::Style::MANUAL);
  scene_.reset();
  Ogre::ResourceGroupManager::getSingleton().unloadResourceGroup(group_name_);
}

//---------------------------------------------------------------------------------------------------------------------
float DotSceneLoaderB::GetHeigh(float x, float z) { return terrain_ ? terrain_->GetHeigh(x, z) : 0; }

//---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessScene_(pugi::xml_node &xml_root) {
  // Process the scene parameters
  string version = GetAttrib(xml_root, "formatVersion", "unknown");

  string message = "[DotSceneLoader] Parsing dotScene file with version " + version;

  if (xml_root.attribute("sceneManager"))
    message += ", scene manager " + string(xml_root.attribute("sceneManager").value());

  if (xml_root.attribute("minOgreVersion"))
    message += ", min. Ogre version " + string(xml_root.attribute("minOgreVersion").value());

  if (xml_root.attribute("author")) message += ", author " + string(xml_root.attribute("author").value());

  Ogre::LogManager::getSingleton().logMessage(message);

  if (auto element = xml_root.child("environment")) ProcessEnvironment_(element);

  if (auto element = xml_root.child("terrainGroup")) ProcessLandscape_(element);

  if (auto element = xml_root.child("nodes")) ProcessNodes_(element);

  if (auto element = xml_root.child("externals")) ProcessExternals_(element);

  if (auto element = xml_root.child("forest")) ProcessForest_(element);

  if (auto element = xml_root.child("userData")) ProcessUserData_(element, attach_node_->getUserObjectBindings());
}

//---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessNodes_(pugi::xml_node &xml_node) {
  // Process node (*)
  for (auto element : xml_node.children("node")) {
    ProcessNode_(element);
  }

  // Process position
  if (auto element = xml_node.child("position")) {
    attach_node_->setPosition(ParsePosition(element));
    attach_node_->setInitialState();
  }

  // Process rotation
  if (auto element = xml_node.child("rotation")) {
    attach_node_->setOrientation(ParseRotation(element));
    attach_node_->setInitialState();
  }

  // Process scale
  if (auto element = xml_node.child("scale")) {
    attach_node_->setScale(ParseScale(element));
    attach_node_->setInitialState();
  }
}

//---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessExternals_(pugi::xml_node &xml_node) {
  //! @todo Implement this
}

//---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessEnvironment_(pugi::xml_node &xml_node) {
  // Process camera
  auto *viewport = Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default")->getViewport();

  // Process fog
  if (auto element = xml_node.child("fog")) ProcessFog_(element);

  // Process skyBox
  if (auto element = xml_node.child("skyBox")) ProcessSkyBox_(element);

  // Process skyDome
  if (auto element = xml_node.child("skyDome")) ProcessSkyDome_(element);

  // Process skyPlane
  if (auto element = xml_node.child("skyPlane")) ProcessSkyPlane_(element);

  // Process colourAmbient
  if (auto element = xml_node.child("colourAmbient")) ogre_scene_->setAmbientLight(ParseColour(element));

  if (auto element = xml_node.child("shadowColor")) ogre_scene_->setShadowColour(ParseColour(element));

  // Process colourBackground
  if (auto element = xml_node.child("colourBackground")) viewport->setBackgroundColour(ParseColour(element));
}

//---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessLightRange_(pugi::xml_node &xml_node, Ogre::Light *light) {
  // Process attributes
  float inner = GetAttribReal(xml_node, "inner");
  float outer = GetAttribReal(xml_node, "outer");
  float falloff = GetAttribReal(xml_node, "falloff", 1.0);

  // Setup the light range
  light->setSpotlightRange(Ogre::Radian(inner), Ogre::Radian(outer), falloff);
}

//---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessLightAttenuation_(pugi::xml_node &xml_node, Ogre::Light *light) {
  // Process attributes
  float range = GetAttribReal(xml_node, "range");
  float constant = GetAttribReal(xml_node, "constant");
  float linear = GetAttribReal(xml_node, "linear");
  float quadratic = GetAttribReal(xml_node, "quadratic");

  // Setup the light attenuation
  light->setAttenuation(range, constant, linear, quadratic);
}

//---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessLight_(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  // Process attributes
  string name = GetAttrib(xml_node, "name");
  const size_t MAX_TEX_COUNT = 9;

  // Init the light
  Ogre::Light *light = ogre_scene_->createLight(name);
  parent->attachObject(light);

  string sValue = GetAttrib(xml_node, "type");

  if (sValue == "point") {
    light->setType(Ogre::Light::LT_POINT);
  } else if (sValue == "directional") {
    light->setType(Ogre::Light::LT_DIRECTIONAL);
  } else if (sValue == "spot") {
    light->setType(Ogre::Light::LT_SPOTLIGHT);
  } else if (sValue == "radPoint") {
    light->setType(Ogre::Light::LT_POINT);
  }

  light->setVisible(GetAttribBool(xml_node, "visible", true));
  light->setCastShadows(GetAttribBool(xml_node, "castShadows", false));

  if (Ogre::Root::getSingleton().getSceneManager("Default")->getShadowTechnique() != Ogre::SHADOWTYPE_NONE) {
    auto texture_config = ogre_scene_->getShadowTextureConfigList()[0];

    if (ogre_scene_->getShadowTextureConfigList().size() < MAX_TEX_COUNT) {
      if (light->getType() == Ogre::Light::LT_POINT) {
        light->setCastShadows(false);
      } else if (light->getType() == Ogre::Light::LT_SPOTLIGHT && light->getCastShadows()) {
        int camera_type = 4;

        conf_->Get("camera_type", camera_type);
        static Ogre::ShadowCameraSetupPtr default_scs;

        switch (camera_type) {
          case 1:
            default_scs = Ogre::DefaultShadowCameraSetup::create();
            break;
          case 2:
            default_scs = Ogre::FocusedShadowCameraSetup::create();
            break;
          case 3:
            // default_scs = Ogre::PlaneOptimalShadowCameraSetup::create();
            break;
          case 4:
          default:
            default_scs = Ogre::LiSPSMShadowCameraSetup::create();
            break;
        }

        light->setCustomShadowCameraSetup(default_scs);
        size_t tex_count = ogre_scene_->getShadowTextureConfigList().size() + 1;
        ogre_scene_->setShadowTextureCount(tex_count);

        size_t index = tex_count - 1;
        texture_config.height *= pow(2, -floor(index / 3));
        texture_config.width *= pow(2, -floor(index / 3));
        ogre_scene_->setShadowTextureConfig(index, texture_config);
      } else if (light->getType() == Ogre::Light::LT_DIRECTIONAL && light->getCastShadows()) {
        size_t per_light = ogre_scene_->getShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL);
        size_t tex_count = ogre_scene_->getShadowTextureConfigList().size() + per_light - 1;
        ogre_scene_->setShadowTextureCount(tex_count);

        for (size_t i = 1; i <= per_light; i++) {
          size_t index = tex_count - i;
          //		  texture_config.height *= pow(2, -floor(index/3));
          //		  texture_config.width *= pow(2, -floor(index/3));
          ogre_scene_->setShadowTextureConfig(index, texture_config);
        }
      }
    }
  }

  light->setPowerScale(GetAttribReal(xml_node, "powerScale", 1.0));

  // Process colourDiffuse
  if (auto element = xml_node.child("colourDiffuse")) {
    light->setDiffuseColour(ParseColour(element));
  }

  // Process colourSpecular
  if (auto element = xml_node.child("colourSpecular")) {
    light->setSpecularColour(ParseColour(element));
  }

  if (light->getType() != Ogre::Light::LT_DIRECTIONAL) {
    // Process lightAttenuation
    if (auto element = xml_node.child("lightAttenuation")) {
      ProcessLightAttenuation_(element, light);
    }
  }

  if (light->getType() != Ogre::Light::LT_SPOTLIGHT) {
    // Process lightRange
    if (auto element = xml_node.child("lightRange")) {
      ProcessLightRange_(element, light);
    }
  }

  // Process userDataReference
  if (auto element = xml_node.child("userData")) {
    ProcessUserData_(element, light->getUserObjectBindings());
  }
}

//---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessCamera_(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  // Process attributes
  string name = GetAttrib(xml_node, "name");
  float fov = GetAttribReal(xml_node, "fov", 90);
  float aspectRatio = GetAttribReal(xml_node, "aspectRatio", 0);
  string projectionType = GetAttrib(xml_node, "projectionType", "perspective");

  // Init the camera
  auto *camera = Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default");

  if (!camera_) camera_ = make_unique<CameraMan>();

  camera_->AttachCamera(parent, camera);

  if (camera_->GetStyle() == CameraMan::Style::FPS) {
    auto *scene = Ogre::Root::getSingleton().getSceneManager("Default");
    auto *actor = scene->createEntity("Actor", "Icosphere.mesh");
    actor->setCastShadows(false);
    actor->setVisible(false);
    parent->attachObject(actor);
    unique_ptr<BtOgre::StaticMeshToShapeConverter> converter;
    btVector3 inertia;
    btRigidBody *entBody;
    converter = make_unique<BtOgre::StaticMeshToShapeConverter>(actor);
    auto *entShape = converter->createCapsule();
    GetAudio().SetListener(camera->getParentSceneNode());
    float mass = 100.0f;
    entShape->calculateLocalInertia(mass, inertia);
    auto *bodyState = new BtOgre::RigidBodyState(parent);
    entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    entBody->setAngularFactor(0);
    entBody->activate(true);
    entBody->forceActivationState(DISABLE_DEACTIVATION);
    entBody->setActivationState(DISABLE_DEACTIVATION);
    entBody->setFriction(1.0);
    entBody->setUserIndex(1);
    GetPhysics().AddRigidBody(entBody);
    camera_->SetRigidBody(entBody);

    camera_->AttachNode(parent);
  } else {
    sinbad_ = make_unique<SinbadCharacterController>(ogre_scene_->getCamera("Default"));
    camera_->AttachNode(parent, sinbad_->GetBodyNode());
  }

  // Set the field-of-view
  camera->setFOVy(Ogre::Radian(fov));

  // Set the aspect ratio
  if (aspectRatio != 0) camera->setAspectRatio(aspectRatio);

  // Set the projection type
  if (projectionType == "perspective") {
    camera->setProjectionType(Ogre::PT_PERSPECTIVE);
  } else if (projectionType == "orthographic") {
    camera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
  }

  // Process clipping
  if (auto element = xml_node.child("clipping")) {
    float nearDist = GetAttribReal(element, "near");
    camera->setNearClipDistance(nearDist);

    float farDist = GetAttribReal(element, "far");
    camera->setFarClipDistance(farDist);
  }

  // Process userDataReference
  if (auto element = xml_node.child("userData")) {
    ProcessUserData_(element, static_cast<Ogre::MovableObject *>(camera)->getUserObjectBindings());
  }
}

//---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessNode_(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  // Construct the node's name
  string name = GetAttrib(xml_node, "name");

  // Init the scene node
  Ogre::SceneNode *node;

  if (name.empty()) {
    // Let Ogre choose the name
    if (parent) {
      node = parent->createChildSceneNode();
    } else {
      node = attach_node_->createChildSceneNode();
    }
  } else {
    // Provide the name
    if (parent) {
      node = parent->createChildSceneNode(name);
    } else {
      node = attach_node_->createChildSceneNode(name);
    }
  }

  // Process other attributes

  // Process position
  if (auto element = xml_node.child("position")) {
    Ogre::Vector3 position = ParsePosition(element);
    if (!parent) position.y += GetHeigh(position.x, position.z);
    node->setPosition(position);
    node->setInitialState();
  }

  // Process rotation
  if (auto element = xml_node.child("rotation")) {
    node->setOrientation(ParseRotation(element));
    node->setInitialState();
  }

  // Process rotation
  if (auto element = xml_node.child("direction")) {
    node->setDirection(ParsePosition(element).normalisedCopy());
    node->setInitialState();
  }

  // Process scale
  if (auto element = xml_node.child("scale")) {
    node->setScale(ParseScale(element));
    node->setInitialState();
  }

  // Process lookTarget
  if (auto element = xml_node.child("lookTarget")) ProcessLookTarget_(element, node);

  // Process trackTarget
  if (auto element = xml_node.child("trackTarget")) ProcessTrackTarget_(element, node);

  // Process node (*)
  for (auto element : xml_node.children("node")) {
    ProcessNode_(element, node);
  }

  // Process entity (*)
  for (auto element : xml_node.children("entity")) {
    ProcessEntity_(element, node);
  }

  // Process light (*)
  for (auto element : xml_node.children("light")) {
    ProcessLight_(element, node);
  }

  // Process particleSystem (*)
  for (auto element : xml_node.children("particleSystem")) {
    ProcessParticleSystem_(element, node);
  }

  // Process billboardSet (*)
  for (auto element : xml_node.children("billboardSet")) {
    ProcessBillboardSet_(element, node);
  }

  // Process plane (*)
  for (auto element : xml_node.children("plane")) {
    ProcessPlane_(element, node);
  }

  // Process camera (*)
  for (auto element : xml_node.children("camera")) {
    ProcessCamera_(element, node);
  }

  // Process userDataReference
  if (auto element = xml_node.child("userData")) {
    ProcessUserData_(element, node->getUserObjectBindings());
  }
}

//---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessLookTarget_(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  //! @todo Is this correct? Cause I don't have a clue actually

  // Process attributes
  string nodeName = GetAttrib(xml_node, "nodeName");

  Ogre::Node::TransformSpace relativeTo = Ogre::Node::TS_PARENT;
  string sValue = GetAttrib(xml_node, "relativeTo");

  if (sValue == "local") {
    relativeTo = Ogre::Node::TS_LOCAL;
  } else if (sValue == "parent") {
    relativeTo = Ogre::Node::TS_PARENT;
  } else if (sValue == "world") {
    relativeTo = Ogre::Node::TS_WORLD;
  }

  // Process position
  Ogre::Vector3 position;

  if (auto element = xml_node.child("position")) {
    position = ParseVector3(element);
  }

  // Process localDirection
  Ogre::Vector3 localDirection = Ogre::Vector3::NEGATIVE_UNIT_Z;

  if (auto element = xml_node.child("localDirection")) {
    localDirection = ParseVector3(element);
  }

  // Setup the look target
  try {
    if (!nodeName.empty()) {
      Ogre::SceneNode *pLookNode = ogre_scene_->getSceneNode(nodeName);
      position = pLookNode->_getDerivedPosition();
    }

    parent->lookAt(position, relativeTo, localDirection);
  } catch (Ogre::Exception &e) {
    Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error processing a look target!");
  }
}

//---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessTrackTarget_(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  // Process attributes
  string nodeName = GetAttrib(xml_node, "nodeName");

  // Process localDirection
  Ogre::Vector3 localDirection = Ogre::Vector3::NEGATIVE_UNIT_Z;

  if (auto element = xml_node.child("localDirection")) localDirection = ParseVector3(element);

  // Process offset
  Ogre::Vector3 offset = Ogre::Vector3::ZERO;

  if (auto element = xml_node.child("offset")) offset = ParseVector3(element);

  // Setup the track target
  try {
    Ogre::SceneNode *pTrackNode = ogre_scene_->getSceneNode(nodeName);
    parent->setAutoTracking(true, pTrackNode, localDirection, offset);
  } catch (Ogre::Exception &e) {
    Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error processing a track target!");
  }
}

//---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessEntity_(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  // Process attributes
  string name = GetAttrib(xml_node, "name");
  string id = GetAttrib(xml_node, "id");
  string meshFile = GetAttrib(xml_node, "meshFile");
  string material_name = GetAttrib(xml_node, "material");
  bool cast_shadows = GetAttribBool(xml_node, "castShadows", true);
  bool active_ibl = GetAttribBool(xml_node, "activeIBL", false);
  bool planar_reflection = GetAttribBool(xml_node, "planarReflection", false);

  // Init the entity
  Ogre::Entity *entity = ogre_scene_->createEntity(name, meshFile);

  try {
    parent->attachObject(entity);
    UpdateEntityMaterial(entity, cast_shadows, material_name, planar_reflection, active_ibl);

    // Process userDataReference
    if (auto element = xml_node.child("userData")) {
      ProcessUserData_(element, entity->getUserObjectBindings());
      GetPhysics().ProcessData(entity->getUserObjectBindings(), entity, parent);
    } else {
      GetPhysics().ProcessData(entity, parent);
    }
  } catch (Ogre::Exception &e) {
    Ogre::LogManager::getSingleton().logMessage(e.getFullDescription());
    Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error loading an entity!");
  }
}

//---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessParticleSystem_(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  // Process attributes
  string name = GetAttrib(xml_node, "name");
  string templateName = GetAttrib(xml_node, "template");
  bool attachedCamera = GetAttribBool(xml_node, "attachedCamera", false);

  if (templateName.empty()) {
    templateName = GetAttrib(xml_node, "file");  // compatibility with old scenes
  }

  // Init the particle system
  try {
    Ogre::ParticleSystem *pParticles = ogre_scene_->createParticleSystem(name, templateName);
    Pbr::UpdatePbrParams(pParticles->getMaterialName());

    const Ogre::uint32 WATER_MASK = 0xF00;
    pParticles->setVisibilityFlags(WATER_MASK);

    if (attachedCamera) {
      auto *camera = Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default");
      camera->getParentSceneNode()->createChildSceneNode(Ogre::Vector3{0, 10, 0})->attachObject(pParticles);
    } else {
      parent->attachObject(pParticles);
    }
  } catch (Ogre::Exception &e) {
    Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error creating a particle system!");
  }
}

//---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessBillboardSet_(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  //! @todo Implement this
}

//---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessPlane_(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  string name = GetAttrib(xml_node, "name");
  float distance = GetAttribReal(xml_node, "distance", 0.0f);
  float width = GetAttribReal(xml_node, "width", 1.0f);
  float height = GetAttribReal(xml_node, "height", width);
  int xSegments = SC::parseInt(GetAttrib(xml_node, "xSegments"), width / 5.0f);
  int ySegments = SC::parseInt(GetAttrib(xml_node, "ySegments"), height / 5.0f);
  int numTexCoordSets = SC::parseInt(GetAttrib(xml_node, "numTexCoordSets"), 1);
  float uTile = GetAttribReal(xml_node, "uTile", width / 10.0f);
  float vTile = GetAttribReal(xml_node, "vTile", height / 10.0f);
  string material = GetAttrib(xml_node, "material", "BaseWhite");
  bool hasNormals = GetAttribBool(xml_node, "hasNormals", true);
  Ogre::Vector3 normal = ParseVector3(xml_node.child("normal"));
  Ogre::Vector3 up = ParseVector3(xml_node.child("upVector"));
  bool reflection = GetAttribBool(xml_node, "reflection", false);

  normal = {0, 1, 0};
  up = {0, 0, 1};
  Ogre::Plane plane(normal, distance);

  string mesh_name = name + "_mesh";

  auto &mesh_mgr = Ogre::MeshManager::getSingleton();

  Ogre::MeshPtr plane_mesh = mesh_mgr.getByName(mesh_name);

  if (plane_mesh) mesh_mgr.remove(plane_mesh);

  Ogre::MeshPtr res = mesh_mgr.createPlane(mesh_name, group_name_, plane, width, height, xSegments, ySegments,
                                           hasNormals, numTexCoordSets, uTile, vTile, up);
  res->buildTangentVectors();
  Ogre::Entity *entity = ogre_scene_->createEntity(name, mesh_name);
  entity->setCastShadows(true);

  if (material.empty()) return;

  if (!material.empty()) {
    entity->setMaterialName(material);
    Ogre::MaterialPtr material_ptr = Ogre::MaterialManager::getSingleton().getByName(material);
    Pbr::UpdatePbrParams(material);
    if (material_ptr->getReceiveShadows()) Pbr::UpdatePbrShadowReceiver(material);
  }

  if (reflection) {
    rcamera_.reset();
    rcamera_ = make_unique<ReflectionCamera>(plane, 512);

    rcamera_->SetPlane(plane);

    Ogre::MaterialPtr material_ptr = Ogre::MaterialManager::getSingleton().getByName(material);

    auto material_unit = material_ptr->getTechnique(0)->getPass(0)->getTextureUnitState("ReflectionMap");

    if (material_unit) {
      material_unit->setTexture(rcamera_->reflection_tex_);
      material_unit->setTextureAddressingMode(Ogre::TAM_CLAMP);
      material_unit->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_POINT);
    }
  }

  parent->attachObject(entity);

  unique_ptr<BtOgre::StaticMeshToShapeConverter> converter = make_unique<BtOgre::StaticMeshToShapeConverter>(entity);

  auto *entShape = converter->createTrimesh();
  auto *bodyState = new BtOgre::RigidBodyState(parent);
  auto *entBody = new btRigidBody(0, bodyState, entShape, btVector3(0, 0, 0));
  entBody->setFriction(1);
  GetPhysics().AddRigidBody(entBody);

  const Ogre::uint32 WATER_MASK = 0xF00;
  entity->setVisibilityFlags(WATER_MASK);
}

//---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessForest_(pugi::xml_node &xml_node) { forest_->ProcessForest(); }

//---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessLandscape_(pugi::xml_node &xml_node) {
  if (terrain_) terrain_->ProcessTerrainGroup(xml_node);
}

//---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessFog_(pugi::xml_node &xml_node) {
  // Process attributes
  float expDensity = GetAttribReal(xml_node, "density", 0.001);
  float linearStart = GetAttribReal(xml_node, "start", 0.0);
  float linearEnd = GetAttribReal(xml_node, "end", 1.0);
  Ogre::ColourValue colour = Ogre::ColourValue::White;

  Ogre::FogMode mode = Ogre::FOG_NONE;
  string sMode = GetAttrib(xml_node, "mode");

  if (sMode == "none") {
    mode = Ogre::FOG_NONE;
  } else if (sMode == "exp") {
    mode = Ogre::FOG_EXP;
  } else if (sMode == "exp2") {
    mode = Ogre::FOG_EXP2;
  } else if (sMode == "linear") {
    mode = Ogre::FOG_LINEAR;
  } else {
    mode = (Ogre::FogMode)SC::parseInt(sMode);
  }

  if (auto element = xml_node.child("colour")) {
    colour = ParseColour(element);
  }

  // Setup the fog
  ogre_scene_->setFog(mode, colour, expDensity, linearStart, linearEnd);
}

//---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessSkyBox_(pugi::xml_node &xml_node) {
  // Process attributes
  string material = GetAttrib(xml_node, "material", "SkyBox");
  string cubemap = GetAttrib(xml_node, "cubemap", "OutputCube.dds");
  float distance = GetAttribReal(xml_node, "distance", 500);
  bool drawFirst = GetAttribBool(xml_node, "drawFirst", true);
  bool active = GetAttribBool(xml_node, "active", true);

  if (!active) {
    return;
  }

  // Process rotation
  Ogre::Quaternion rotation = Ogre::Quaternion::IDENTITY;

  if (auto element = xml_node.child("rotation")) {
    rotation = ParseRotation(element);
  }

  Ogre::MaterialPtr material_ptr = Ogre::MaterialManager::getSingleton().getByName(material);

  if (material_ptr->getTechnique(0)->getPass(0)->getNumTextureUnitStates() > 0) {
    auto texture_unit = material_ptr->getTechnique(0)->getPass(0)->getTextureUnitState(0);

    if (texture_unit) {
      texture_unit->setTextureName(cubemap);
    }
  }

  // Setup the sky box
  ogre_scene_->setSkyBox(true, material, distance, drawFirst, rotation, group_name_);
}

//---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessSkyDome_(pugi::xml_node &xml_node) {
  // Process attributes
  string material = xml_node.attribute("material").value();
  float curvature = GetAttribReal(xml_node, "curvature", 10);
  float tiling = GetAttribReal(xml_node, "tiling", 8);
  float distance = GetAttribReal(xml_node, "distance", 500);
  bool drawFirst = GetAttribBool(xml_node, "drawFirst", true);
  bool active = GetAttribBool(xml_node, "active", false);

  if (!active) {
    return;
  }

  // Process rotation
  Ogre::Quaternion rotation = Ogre::Quaternion::IDENTITY;

  if (auto element = xml_node.child("rotation")) {
    rotation = ParseRotation(element);
  }

  // Setup the sky dome
  ogre_scene_->setSkyDome(true, material, curvature, tiling, distance, drawFirst, rotation, 16, 16, -1, group_name_);
}

//---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessSkyPlane_(pugi::xml_node &xml_node) {
  // Process attributes
  string material = GetAttrib(xml_node, "material");
  float planeX = GetAttribReal(xml_node, "planeX", 0);
  float planeY = GetAttribReal(xml_node, "planeY", -1);
  float planeZ = GetAttribReal(xml_node, "planeZ", 0);
  float planeD = GetAttribReal(xml_node, "planeD", 5000);
  float scale = GetAttribReal(xml_node, "scale", 1000);
  float bow = GetAttribReal(xml_node, "bow", 0);
  float tiling = GetAttribReal(xml_node, "tiling", 10);
  bool drawFirst = GetAttribBool(xml_node, "drawFirst", true);

  // Setup the sky plane
  Ogre::Plane plane;
  plane.normal = Ogre::Vector3(planeX, planeY, planeZ);
  plane.d = planeD;
  ogre_scene_->setSkyPlane(true, plane, material, scale, tiling, drawFirst, bow, 1, 1, group_name_);
}

//---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessUserData_(pugi::xml_node &xml_node, Ogre::UserObjectBindings &user_object_bindings) {
  // Process node (*)
  for (auto element : xml_node.children("property")) {
    string name = GetAttrib(element, "name");
    string type = GetAttrib(element, "type");
    string data = GetAttrib(element, "data");

    Ogre::Any value;

    if (type == "bool") {
      value = SC::parseBool(data);
    } else if (type == "float") {
      value = SC::parseReal(data);
    } else if (type == "int") {
      value = SC::parseInt(data);
    } else {
      value = data;
    }

    user_object_bindings.setUserAny(name, value);
  }
}


//---------------------------------------------------------------------------------------------------------------------
struct DotSceneCodec : public Ogre::Codec {
  string magicNumberToFileExt(const char *magicNumberPtr, size_t maxbytes) const { return ""; }
  string getType() const override { return "scene"; }
  void decode(const Ogre::DataStreamPtr &stream, const Ogre::Any &output) const override {
      using namespace Ogre;

    DataStreamPtr _stream(stream);
    DotSceneLoaderB loader;
    loader.load(_stream, ResourceGroupManager::getSingleton().getWorldResourceGroupName(),
                any_cast<SceneNode *>(output));
  }

  void encodeToFile(const Ogre::Any &input, const Ogre::String &outFileName) const override {
    DotSceneLoaderB loader;
    loader.exportScene(Ogre::any_cast<Ogre::SceneNode *>(input), outFileName);
  }
};

//---------------------------------------------------------------------------------------------------------------------
const Ogre::String &DotScenePluginB::getName() const {
  static Ogre::String name = "DotScene Loader";
  return name;
}

//---------------------------------------------------------------------------------------------------------------------
void DotScenePluginB::install() {}

//---------------------------------------------------------------------------------------------------------------------
void DotScenePluginB::initialise() {
  mCodec = new DotSceneCodec();
  Ogre::Codec::registerCodec(mCodec);
}

//---------------------------------------------------------------------------------------------------------------------
void DotScenePluginB::shutdown() {
  Ogre::Codec::unregisterCodec(mCodec);
  delete mCodec;
}

//---------------------------------------------------------------------------------------------------------------------
void DotScenePluginB::uninstall() {}

}  // namespace xio
