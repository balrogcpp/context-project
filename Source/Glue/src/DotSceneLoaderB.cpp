// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "DotSceneLoaderB.h"
#include "ComponentLocator.h"
#include "MeshUtils.h"
#include "PbrShaderUtils.h"
#include "SinbadCharacterController.h"
#include "BtOgre/BtOgre.h"
#ifdef OGRE_BUILD_COMPONENT_MESHLODGENERATOR
#include <MeshLodGenerator/OgreLodConfig.h>
#include <MeshLodGenerator/OgreMeshLodGenerator.h>
#include <OgreDistanceLodStrategy.h>
#include <OgrePixelCountLodStrategy.h>
#endif
#ifdef OGRE_BUILD_COMPONENT_PAGING
#include <Paging/OgrePage.h>
#include <Paging/OgrePaging.h>
#endif
#include <pugixml.hpp>

using namespace std;

namespace Glue {

template <typename T>
static string ToString(T t) {
  return t == 0 ? string() : to_string(t);
}

template <typename T>
static const char *ToCString(T t) {
  return t == 0 ? "" : to_string(t).c_str();
}

static float ToFloat(const string &s, float defaultValue = 0.0f) { return s.empty() ? defaultValue : atof(s.c_str()); }

static float ToInt(const string &s, int defaultValue = 0) { return s.empty() ? defaultValue : atoi(s.c_str()); }

string GetAttrib(const pugi::xml_node &xml_node, const string &attrib, const string &defaultValue) {
  if (auto anode = xml_node.attribute(attrib.c_str())) {
    return anode.value();
  } else {
    return defaultValue;
  }
}

float GetAttribReal(const pugi::xml_node &xml_node, const string &attrib, float defaultValue) {
  if (auto anode = xml_node.attribute(attrib.c_str())) {
    return ToFloat(anode.value());
  } else {
    return defaultValue;
  }
}

int GetAttribInt(const pugi::xml_node &xml_node, const string &attrib, int defaultValue) {
  if (auto anode = xml_node.attribute(attrib.c_str())) {
    return ToInt(anode.value());
  } else {
    return defaultValue;
  }
}

bool GetAttribBool(const pugi::xml_node &xml_node, const string &attrib, bool defaultValue) {
  if (auto anode = xml_node.attribute(attrib.c_str())) {
    return anode.as_bool();
  } else {
    return defaultValue;
  }
}

Ogre::Vector3 ParseVector3(const pugi::xml_node &xml_node) {
  float x = ToFloat(xml_node.attribute("x").value());
  float y = ToFloat(xml_node.attribute("y").value());
  float z = ToFloat(xml_node.attribute("z").value());

  return Ogre::Vector3(x, y, z);
}

Ogre::Vector3 ParsePosition(const pugi::xml_node &xml_node) {
  float x = ToFloat(xml_node.attribute("x").value());
  float y = ToFloat(xml_node.attribute("y").value());
  float z = ToFloat(xml_node.attribute("z").value());

  return Ogre::Vector3(x, y, z);
}

Ogre::Vector3 ParseScale(const pugi::xml_node &xml_node) {
  float x = ToFloat(xml_node.attribute("x").value());
  float y = ToFloat(xml_node.attribute("y").value());
  float z = ToFloat(xml_node.attribute("z").value());

  return Ogre::Vector3(x, y, z);
}

Ogre::Quaternion ParseRotation(const pugi::xml_node &xml_node) {
  Ogre::Quaternion orientation;

  if (xml_node.attribute("qw")) {
    orientation.w = ToFloat(xml_node.attribute("qw").value());
    orientation.x = ToFloat(xml_node.attribute("qx").value());
    orientation.y = -ToFloat(xml_node.attribute("qz").value());
    orientation.z = ToFloat(xml_node.attribute("qy").value());
  } else if (xml_node.attribute("axisX")) {
    Ogre::Vector3 axis;
    axis.x = ToFloat(xml_node.attribute("axisX").value());
    axis.y = ToFloat(xml_node.attribute("axisY").value());
    axis.z = ToFloat(xml_node.attribute("axisZ").value());
    float angle = ToFloat(xml_node.attribute("angle").value());

    orientation.FromAngleAxis(Ogre::Angle(angle), axis);
  } else if (xml_node.attribute("angleX")) {
    Ogre::Matrix3 rot;
    rot.FromEulerAnglesXYZ(Ogre::Angle(ToFloat(xml_node.attribute("angleX").value())),
                           Ogre::Angle(ToFloat(xml_node.attribute("angleY").value())),
                           Ogre::Angle(ToFloat(xml_node.attribute("angleZ").value())));
    orientation.FromRotationMatrix(rot);
  } else if (xml_node.attribute("x")) {
    orientation.x = ToFloat(xml_node.attribute("x").value());
    orientation.y = ToFloat(xml_node.attribute("y").value());
    orientation.z = ToFloat(xml_node.attribute("z").value());
    orientation.w = ToFloat(xml_node.attribute("w").value());
  } else if (xml_node.attribute("w")) {
    orientation.w = ToFloat(xml_node.attribute("w").value());
    orientation.x = ToFloat(xml_node.attribute("x").value());
    orientation.y = ToFloat(xml_node.attribute("y").value());
    orientation.z = ToFloat(xml_node.attribute("z").value());
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

Ogre::ColourValue ParseColour(pugi::xml_node &xml_node) {
  return Ogre::ColourValue(ToFloat(xml_node.attribute("r").value()), ToFloat(xml_node.attribute("g").value()),
                           ToFloat(xml_node.attribute("b").value()), ToFloat(xml_node.attribute("a").value(), 1));
}

static void write(pugi::xml_node &node, const Ogre::Vector3 &v) {
  node.append_attribute("x") = ToCString(v.x);
  node.append_attribute("y") = ToCString(v.y);
  node.append_attribute("z") = ToCString(v.z);
}

static void write(pugi::xml_node &node, const Ogre::ColourValue &c) {
  node.append_attribute("r") = ToCString(c.r);
  node.append_attribute("g") = ToCString(c.g);
  node.append_attribute("b") = ToCString(c.b);
  node.append_attribute("a") = ToCString(c.a);
}

DotSceneLoaderB::DotSceneLoaderB() { camera_man = make_unique<CameraMan>(); }

DotSceneLoaderB::~DotSceneLoaderB() { Cleanup(); }

Landscape &DotSceneLoaderB::GetTerrain() { return *terrain; }

CameraMan &DotSceneLoaderB::GetCamera() const { return *camera_man; }

VegetationSystem &DotSceneLoaderB::GetForest() { return *forest; }

void DotSceneLoaderB::Update(float time) {
  if (paused) return;
  if (camera_man) camera_man->Update(time);
  if (terrain) terrain->Update(time);
  if (forest) forest->Update(time);
  if (sinbad && terrain) sinbad->Update(time);
}

void DotSceneLoaderB::load(Ogre::DataStreamPtr &stream, const string &group_name, Ogre::SceneNode *root_node) {
  this->group_name = group_name;
  ogre_scene = root_node->getCreator();

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

  // figure out where to attach any nodes we init
  root = Ogre::Root::getSingletonPtr();
  attach_node = root_node;

  if (!terrain) terrain = make_unique<Landscape>();
  if (!forest) forest = make_unique<VegetationSystem>();
  forest->SetHeighFunc([](float x, float z) { return terrain->GetHeigh(x, z); });

  // Process the scene
  ProcessScene(XMLRoot);
}

void DotSceneLoaderB::WriteNode(pugi::xml_node &parentXML, const Ogre::SceneNode *node) {
  auto nodeXML = parentXML.append_child("node");
  if (!node->getName().empty()) nodeXML.append_attribute("name") = node->getName().c_str();

  auto pos = nodeXML.append_child("position");
  write(pos, node->getPosition());

  auto scale = nodeXML.append_child("scale");
  write(scale, node->getScale());

  auto rot = nodeXML.append_child("rotation");
  rot.append_attribute("qw") = ToCString(node->getOrientation().w);
  rot.append_attribute("qx") = ToCString(node->getOrientation().x);
  rot.append_attribute("qy") = ToCString(node->getOrientation().y);
  rot.append_attribute("qz") = ToCString(node->getOrientation().z);

  for (auto mo : node->getAttachedObjects()) {
    if (auto c = dynamic_cast<Ogre::Camera *>(mo)) {
      auto camera = nodeXML.append_child("camera");
      camera.append_attribute("name") = c->getName().c_str();
      auto clipping = camera.append_child("clipping");
      clipping.append_attribute("near") = ToCString(c->getNearClipDistance());
      clipping.append_attribute("far") = ToCString(c->getFarClipDistance());
      continue;
    }

    if (auto l = dynamic_cast<Ogre::Light *>(mo)) {
      auto light = nodeXML.append_child("light");
      light.append_attribute("name") = l->getName().c_str();
      light.append_attribute("castShadows") = ToCString(l->getCastShadows());

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
        range.append_attribute("inner") = ToCString(l->getSpotlightInnerAngle().valueRadians());
        range.append_attribute("outer") = ToCString(l->getSpotlightOuterAngle().valueRadians());
        range.append_attribute("falloff") = ToCString(l->getSpotlightFalloff());
        auto atten = light.append_child("lightAttenuation");
        atten.append_attribute("range") = ToCString(l->getAttenuationRange());
        atten.append_attribute("constant") = ToCString(l->getAttenuationConstant());
        atten.append_attribute("linear") = ToCString(l->getAttenuationLinear());
        atten.append_attribute("quadratic") = ToCString(l->getAttenuationQuadric());
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
  for (auto c : node->getChildren()) WriteNode(nodeXML, static_cast<Ogre::SceneNode *>(c));
}

void DotSceneLoaderB::exportScene(Ogre::SceneNode *rootNode, const string &outFileName) {
  pugi::xml_document XMLDoc;  // character type defaults to char
  auto comment = XMLDoc.append_child(pugi::node_comment);
  comment.set_value(Ogre::StringUtil::format(" exporter: Plugin_DotScene %d.%d.%d ", OGRE_VERSION_MAJOR,
                                             OGRE_VERSION_MINOR, OGRE_VERSION_PATCH)
                        .c_str());
  auto scene = XMLDoc.append_child("scene");
  scene.append_attribute("formatVersion") = "1.1";
  scene.append_attribute("sceneManager") = rootNode->getCreator()->getTypeName().c_str();

  auto nodes = scene.append_child("nodes");

  for (auto c : rootNode->getChildren()) WriteNode(nodes, static_cast<Ogre::SceneNode *>(c));

  XMLDoc.save_file(outFileName.c_str());
}

void DotSceneLoaderB::Cleanup() {
  terrain.reset();
  forest.reset();
  sinbad.reset();
  //  Pbr::Cleanup();
  if (ogre_scene) ogre_scene->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
  if (ogre_scene) ogre_scene->clearScene();
  if (camera_man) camera_man->SetStyle(CameraMan::Style::MANUAL);
  Ogre::ResourceGroupManager::getSingleton().unloadResourceGroup(group_name);
}

float DotSceneLoaderB::GetHeigh(float x, float z) { return terrain ? terrain->GetHeigh(x, z) : 0; }

void DotSceneLoaderB::ProcessScene(pugi::xml_node &xml_root) {
  // Process the scene parameters
  string version = GetAttrib(xml_root, "formatVersion", "unknown");

  string message = "[DotSceneLoader] Parsing dotScene file with version " + version;

  if (xml_root.attribute("sceneManager")) {
    message += ", scene manager " + string(xml_root.attribute("sceneManager").value());
  }

  if (xml_root.attribute("minOgreVersion")) {
    message += ", min. Ogre version " + string(xml_root.attribute("minOgreVersion").value());
  }

  if (xml_root.attribute("author")) message += ", author " + string(xml_root.attribute("author").value());

  Ogre::LogManager::getSingleton().logMessage(message);

  if (auto element = xml_root.child("environment")) {
    ProcessEnvironment(element);
  }

  if (auto element = xml_root.child("terrainGroup")) {
    ProcessLandscape(element);
  }

  if (auto element = xml_root.child("nodes")) {
    ProcessNodes(element);
  }

  if (auto element = xml_root.child("externals")) {
    ProcessExternals(element);
  }

  if (auto element = xml_root.child("forest")) {
    ProcessForest(element);
  }

  if (auto element = xml_root.child("userData")) {
    ProcessUserData(element, attach_node->getUserObjectBindings());
  }
}

void DotSceneLoaderB::ProcessNodes(pugi::xml_node &xml_node) {
  // Process node (*)
  for (auto element : xml_node.children("node")) {
    ProcessNode(element);
  }

  // Process position
  if (auto element = xml_node.child("position")) {
    attach_node->setPosition(ParsePosition(element));
    attach_node->setInitialState();
  }

  // Process rotation
  if (auto element = xml_node.child("rotation")) {
    attach_node->setOrientation(ParseRotation(element));
    attach_node->setInitialState();
  }

  // Process scale
  if (auto element = xml_node.child("scale")) {
    attach_node->setScale(ParseScale(element));
    attach_node->setInitialState();
  }
}

void DotSceneLoaderB::ProcessExternals(pugi::xml_node &xml_node) {
  //! @todo Implement this
}

void DotSceneLoaderB::ProcessEnvironment(pugi::xml_node &xml_node) {
  // Process camera
  auto *viewport = Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default")->getViewport();

  // Process fog
  if (auto element = xml_node.child("fog")) {
    ProcessFog(element);
  }

  // Process skyBox
  if (auto element = xml_node.child("skyBox")) {
    ProcessSkyBox(element);
  }

  // Process skyDome
  if (auto element = xml_node.child("skyDome")) {
    ProcessSkyDome(element);
  }

  // Process skyPlane
  if (auto element = xml_node.child("skyPlane")) {
    ProcessSkyPlane(element);
  }

  // Process colourAmbient
  if (auto element = xml_node.child("colourAmbient")) {
    ogre_scene->setAmbientLight(ParseColour(element));
  }

  if (auto element = xml_node.child("shadowColor")) {
    ogre_scene->setShadowColour(ParseColour(element));
  }

  // Process colourBackground
  if (auto element = xml_node.child("colourBackground")) {
    viewport->setBackgroundColour(ParseColour(element));
  }
}

void DotSceneLoaderB::ProcessLightRange(pugi::xml_node &xml_node, Ogre::Light *light) {
  // Process attributes
  float inner = GetAttribReal(xml_node, "inner");
  float outer = GetAttribReal(xml_node, "outer");
  float falloff = GetAttribReal(xml_node, "falloff", 1.0);

  // Setup the light range
  light->setSpotlightRange(Ogre::Radian(inner), Ogre::Radian(outer), falloff);
}

void DotSceneLoaderB::ProcessLightAttenuation(pugi::xml_node &xml_node, Ogre::Light *light) {
  // Process attributes
  float range = GetAttribReal(xml_node, "range");
  float constant = GetAttribReal(xml_node, "constant");
  float linear = GetAttribReal(xml_node, "linear");
  float quadratic = GetAttribReal(xml_node, "quadratic");

  // Setup the light attenuation
  light->setAttenuation(range, constant, linear, quadratic);
}

void DotSceneLoaderB::ProcessLight(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  // Process attributes
  string name = GetAttrib(xml_node, "name");
  const size_t MAX_TEX_COUNT = 9;

  // SetUp the light
  Ogre::Light *light = ogre_scene->createLight(name);
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
    auto texture_config = ogre_scene->getShadowTextureConfigList()[0];

    if (ogre_scene->getShadowTextureConfigList().size() < MAX_TEX_COUNT) {
      if (light->getType() == Ogre::Light::LT_POINT) {
        light->setCastShadows(false);
      } else if (light->getType() == Ogre::Light::LT_SPOTLIGHT && light->getCastShadows()) {
        int camera_type = 4;

        config->Get("camera_type", camera_type);
        static Ogre::ShadowCameraSetupPtr default_scs;

        switch (camera_type) {
          case 1: {
            default_scs = Ogre::DefaultShadowCameraSetup::create();
            break;
          }
          case 2: {
            default_scs = Ogre::FocusedShadowCameraSetup::create();
            break;
          }
          case 3: {
            // default_scs = Ogre::PlaneOptimalShadowCameraSetup::create();
            break;
          }
          case 4:
          default: {
            default_scs = Ogre::LiSPSMShadowCameraSetup::create();
            break;
          }
        }

        light->setCustomShadowCameraSetup(default_scs);
        size_t tex_count = ogre_scene->getShadowTextureConfigList().size() + 1;
        ogre_scene->setShadowTextureCount(tex_count);

        size_t index = tex_count - 1;
        texture_config.height *= pow(2.0, -floor(index / 3.0));
        texture_config.width *= pow(2.0, -floor(index / 3.0));
        ogre_scene->setShadowTextureConfig(index, texture_config);
      } else if (light->getType() == Ogre::Light::LT_DIRECTIONAL && light->getCastShadows()) {
        size_t per_light = ogre_scene->getShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL);
        size_t tex_count = ogre_scene->getShadowTextureConfigList().size() + per_light - 1;
        ogre_scene->setShadowTextureCount(tex_count);

        for (size_t i = 1; i <= per_light; i++) {
          size_t index = tex_count - i;
          //		  texture_config.height *= pow(2, -floor(index/3));
          //		  texture_config.width *= pow(2, -floor(index/3));
          ogre_scene->setShadowTextureConfig(index, texture_config);
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
      ProcessLightAttenuation(element, light);
    }
  }

  if (light->getType() != Ogre::Light::LT_SPOTLIGHT) {
    // Process lightRange
    if (auto element = xml_node.child("lightRange")) {
      ProcessLightRange(element, light);
    }
  }

  // Process userDataReference
  if (auto element = xml_node.child("userData")) {
    ProcessUserData(element, light->getUserObjectBindings());
  }
}

void DotSceneLoaderB::ProcessCamera(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  // Process attributes
  string name = GetAttrib(xml_node, "name");
  float fov = GetAttribReal(xml_node, "fov", 90);
  float aspectRatio = GetAttribReal(xml_node, "aspectRatio", 0);
  string projectionType = GetAttrib(xml_node, "projectionType", "perspective");

  // SetUp the camera
  auto *camera = Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default");

  if (!camera_man) camera_man = make_unique<CameraMan>();

  camera_man->AttachCamera(parent, camera);

  if (camera_man->GetStyle() == CameraMan::Style::FPS) {
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
    camera_man->SetRigidBody(entBody);
    camera_man->AttachNode(parent);
  } else {
    sinbad = make_unique<SinbadCharacterController>(ogre_scene->getCamera("Default"));
    camera_man->AttachNode(parent, sinbad->GetBodyNode());
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
    ProcessUserData(element, static_cast<Ogre::MovableObject *>(camera)->getUserObjectBindings());
  }
}

void DotSceneLoaderB::ProcessNode(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  // Construct the node's name
  string name = GetAttrib(xml_node, "name");

  // SetUp the scene node
  Ogre::SceneNode *node;

  if (name.empty()) {
    // Let Ogre choose the name
    if (parent) {
      node = parent->createChildSceneNode();
    } else {
      node = attach_node->createChildSceneNode();
    }
  } else {
    // Provide the name
    if (parent) {
      node = parent->createChildSceneNode(name);
    } else {
      node = attach_node->createChildSceneNode(name);
    }
  }

  // Process other attributes

  // Process position
  if (auto element = xml_node.child("position")) {
    Ogre::Vector3 position = ParsePosition(element);
    if (!parent) {
      position.y += GetHeigh(position.x, position.z);
    }
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
  if (auto element = xml_node.child("lookTarget")) {
    ProcessLookTarget(element, node);
  }

  // Process trackTarget
  if (auto element = xml_node.child("trackTarget")) {
    ProcessTrackTarget(element, node);
  }

  // Process node (*)
  for (auto element : xml_node.children("node")) {
    ProcessNode(element, node);
  }

  // Process entity (*)
  for (auto element : xml_node.children("entity")) {
    ProcessEntity(element, node);
  }

  // Process light (*)
  for (auto element : xml_node.children("light")) {
    ProcessLight(element, node);
  }

  // Process particleSystem (*)
  for (auto element : xml_node.children("particleSystem")) {
    ProcessParticleSystem(element, node);
  }

  // Process billboardSet (*)
  for (auto element : xml_node.children("billboardSet")) {
    ProcessBillboardSet(element, node);
  }

  // Process plane (*)
  for (auto element : xml_node.children("plane")) {
    ProcessPlane(element, node);
  }

  // Process camera (*)
  for (auto element : xml_node.children("camera")) {
    ProcessCamera(element, node);
  }

  // Process userDataReference
  if (auto element = xml_node.child("userData")) {
    ProcessUserData(element, node->getUserObjectBindings());
  }
}

void DotSceneLoaderB::ProcessLookTarget(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
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
      Ogre::SceneNode *pLookNode = ogre_scene->getSceneNode(nodeName);
      position = pLookNode->_getDerivedPosition();
    }

    parent->lookAt(position, relativeTo, localDirection);
  } catch (Ogre::Exception &e) {
    Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error processing a look target!");
  }
}

void DotSceneLoaderB::ProcessTrackTarget(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  // Process attributes
  string nodeName = GetAttrib(xml_node, "nodeName");

  // Process localDirection
  Ogre::Vector3 localDirection = Ogre::Vector3::NEGATIVE_UNIT_Z;

  if (auto element = xml_node.child("localDirection")) {
    localDirection = ParseVector3(element);
  }

  // Process offset
  Ogre::Vector3 offset = Ogre::Vector3::ZERO;

  if (auto element = xml_node.child("offset")) {
    offset = ParseVector3(element);
  }

  // Setup the track target
  try {
    Ogre::SceneNode *pTrackNode = ogre_scene->getSceneNode(nodeName);
    parent->setAutoTracking(true, pTrackNode, localDirection, offset);
  } catch (Ogre::Exception &e) {
    Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error processing a track target!");
  }
}

void DotSceneLoaderB::ProcessEntity(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  // Process attributes
  string name = GetAttrib(xml_node, "name");
  string id = GetAttrib(xml_node, "id");
  string meshFile = GetAttrib(xml_node, "meshFile");
  string material_name = GetAttrib(xml_node, "material");
  bool cast_shadows = GetAttribBool(xml_node, "castShadows", true);
  bool active_ibl = GetAttribBool(xml_node, "activeIBL", false);
  bool planar_reflection = GetAttribBool(xml_node, "planarReflection", false);

  // SetUp the entity
  Ogre::Entity *entity = ogre_scene->createEntity(name, meshFile);

  try {
    parent->attachObject(entity);
    UpdateEntityMaterial(entity, cast_shadows, material_name, planar_reflection, active_ibl);

    // Process userDataReference
    if (auto element = xml_node.child("userData")) {
      ProcessUserData(element, entity->getUserObjectBindings());
      GetPhysics().ProcessData(entity->getUserObjectBindings(), entity, parent);
    } else {
      GetPhysics().ProcessData(entity, parent);
    }
  } catch (Ogre::Exception &e) {
    Ogre::LogManager::getSingleton().logMessage(e.getFullDescription());
    Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error loading an entity!");
  }
}

void DotSceneLoaderB::ProcessParticleSystem(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  // Process attributes
  string name = GetAttrib(xml_node, "name");
  string templateName = GetAttrib(xml_node, "template");
  bool attachedCamera = GetAttribBool(xml_node, "attachedCamera", false);

  if (templateName.empty()) {
    templateName = GetAttrib(xml_node, "file");  // compatibility with old scenes
  }

  // SetUp the particle system
  try {
    Ogre::ParticleSystem *pParticles = ogre_scene->createParticleSystem(name, templateName);
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

void DotSceneLoaderB::ProcessBillboardSet(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  //! @todo Implement this
}

void DotSceneLoaderB::ProcessPlane(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  string name = GetAttrib(xml_node, "name");
  float distance = GetAttribReal(xml_node, "distance", 0.0f);
  float width = GetAttribReal(xml_node, "width", 1.0f);
  float height = GetAttribReal(xml_node, "height", width);
  int xSegments = ToInt(GetAttrib(xml_node, "xSegments"), width / 5.0f);
  int ySegments = ToInt(GetAttrib(xml_node, "ySegments"), height / 5.0f);
  int numTexCoordSets = ToInt(GetAttrib(xml_node, "numTexCoordSets"), 1);
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

  Ogre::MeshPtr res = mesh_mgr.createPlane(mesh_name, group_name, plane, width, height, xSegments, ySegments,
                                           hasNormals, numTexCoordSets, uTile, vTile, up);
  res->buildTangentVectors();
  Ogre::Entity *entity = ogre_scene->createEntity(name, mesh_name);
  entity->setCastShadows(true);

  if (material.empty()) return;

  if (!material.empty()) {
    entity->setMaterialName(material);
    Ogre::MaterialPtr material_ptr = Ogre::MaterialManager::getSingleton().getByName(material);
    Pbr::UpdatePbrParams(material);
    if (material_ptr->getReceiveShadows()) Pbr::UpdatePbrShadowReceiver(material);
  }

  if (reflection) {
    rcamera.reset();
    rcamera = make_unique<ReflectionCamera>(plane, 512);

    rcamera->SetPlane(plane);

    Ogre::MaterialPtr material_ptr = Ogre::MaterialManager::getSingleton().getByName(material);

    auto material_unit = material_ptr->getTechnique(0)->getPass(0)->getTextureUnitState("ReflectionMap");

    if (material_unit) {
      material_unit->setTexture(rcamera->reflection_texture);
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

void DotSceneLoaderB::ProcessForest(pugi::xml_node &xml_node) { forest->ProcessForest(); }

void DotSceneLoaderB::ProcessLandscape(pugi::xml_node &xml_node) {
  if (terrain) terrain->ProcessTerrainGroup(xml_node);
}

void DotSceneLoaderB::ProcessFog(pugi::xml_node &xml_node) {
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
    mode = (Ogre::FogMode)ToInt(sMode);
  }

  if (auto element = xml_node.child("colour")) {
    colour = ParseColour(element);
  }

  // Setup the fog
  ogre_scene->setFog(mode, colour, expDensity, linearStart, linearEnd);
}

void DotSceneLoaderB::ProcessSkyBox(pugi::xml_node &xml_node) {
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
  ogre_scene->setSkyBox(true, material, distance, drawFirst, rotation, group_name);
}

void DotSceneLoaderB::ProcessSkyDome(pugi::xml_node &xml_node) {
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
  ogre_scene->setSkyDome(true, material, curvature, tiling, distance, drawFirst, rotation, 16, 16, -1, group_name);
}

void DotSceneLoaderB::ProcessSkyPlane(pugi::xml_node &xml_node) {
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
  ogre_scene->setSkyPlane(true, plane, material, scale, tiling, drawFirst, bow, 1, 1, group_name);
}

void DotSceneLoaderB::ProcessUserData(pugi::xml_node &xml_node, Ogre::UserObjectBindings &user_object_bindings) {
  // Process node (*)
  for (auto element : xml_node.children("property")) {
    string name = GetAttrib(element, "name");
    string type = GetAttrib(element, "type");
    string data = GetAttrib(element, "data");

    Ogre::Any value;

    if (type == "bool") {
      value = (ToInt(data) != 0);
    } else if (type == "float") {
      value = ToFloat(data);
    } else if (type == "int") {
      value = ToInt(data);
    } else {
      value = data;
    }

    user_object_bindings.setUserAny(name, value);
  }
}

struct DotSceneCodec : public Ogre::Codec {
  string magicNumberToFileExt(const char *magicNumberPtr, size_t maxbytes) const { return ""; }
  string getType() const override { return "scene"; }
  void decode(const Ogre::DataStreamPtr &stream, const Ogre::Any &output) const override {
    using namespace Ogre;

    DataStreamPtr _stream(stream);
    //    DotSceneLoaderB loader;
    //    loader.load(_stream, ResourceGroupManager::getSingleton().getWorldResourceGroupName(),
    //                any_cast<SceneNode *>(output));
    GetLoader().load(_stream, ResourceGroupManager::getSingleton().getWorldResourceGroupName(),
                     any_cast<SceneNode *>(output));
  }

  void encodeToFile(const Ogre::Any &input, const Ogre::String &outFileName) const override {
    DotSceneLoaderB loader;
    loader.exportScene(Ogre::any_cast<Ogre::SceneNode *>(input), outFileName);
  }
};

const Ogre::String &DotScenePluginB::getName() const {
  static Ogre::String name = "DotScene Loader";
  return name;
}

void DotScenePluginB::install() {}

void DotScenePluginB::initialise() {
  mCodec = new DotSceneCodec();
  Ogre::Codec::registerCodec(mCodec);
}

void DotScenePluginB::shutdown() {
  Ogre::Codec::unregisterCodec(mCodec);
  delete mCodec;
}

void DotScenePluginB::uninstall() {}

}  // namespace glue
