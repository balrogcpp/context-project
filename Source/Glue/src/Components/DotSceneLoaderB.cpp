// This source file is part of "glue project". Created by Andrey Vasiliev

#include "PCHeader.h"
#include "Components/DotSceneLoaderB.h"
#include "CubeMapCamera.h"
#include "Engine.h"
#include "Objects/SinbadCharacterController.h"
#include "PagedGeometry/PagedGeometryAll.h"
#include "ReflectionCamera.h"
#include "ShaderHelpers.h"
#ifdef OGRE_BUILD_COMPONENT_MESHLODGENERATOR
#include <MeshLodGenerator/OgreLodConfig.h>
#include <MeshLodGenerator/OgreMeshLodGenerator.h>
#include <OgreDistanceLodStrategy.h>
#include <OgrePixelCountLodStrategy.h>
#endif
#ifdef OGRE_BUILD_COMPONENT_TERRAIN
#include "Components/TerrainMaterialGeneratorB.h"
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainAutoUpdateLod.h>
#include <Terrain/OgreTerrainGroup.h>
#include <Terrain/OgreTerrainMaterialGeneratorA.h>
#include <Terrain/OgreTerrainQuadTreeNode.h>
#endif
#ifdef OGRE_BUILD_COMPONENT_PAGING
#include <Paging/OgrePage.h>
#include <Paging/OgrePaging.h>
#endif

#include <pugixml.hpp>

using namespace std;
using namespace Forests;
using namespace Ogre;

template <typename T>
std::string ToString(T t) {
  return t == 0 ? std::string() : std::to_string(t);
}

template <typename T>
const char *ToCString(T t) {
  return t == 0 ? "" : std::to_string(t).c_str();
}

inline float ToFloat(const std::string &s, float defaultValue = 0) { return s.empty() ? defaultValue : std::atof(s.c_str()); }
inline float ToInt(const std::string &s, int defaultValue = 0) { return s.empty() ? defaultValue : std::atoi(s.c_str()); }

static string GetAttrib(const pugi::xml_node &XmlNode, const string &attrib, const string &defaultValue = "") {
  if (auto anode = XmlNode.attribute(attrib.c_str())) {
    return anode.value();
  } else {
    return defaultValue;
  }
}

static float GetAttribReal(const pugi::xml_node &XmlNode, const string &attrib, float defaultValue = 0.0) {
  if (auto anode = XmlNode.attribute(attrib.c_str())) {
    return ToFloat(anode.value());
  } else {
    return defaultValue;
  }
}

static int GetAttribInt(const pugi::xml_node &XmlNode, const string &attrib, int defaultValue = 0) {
  if (auto anode = XmlNode.attribute(attrib.c_str())) {
    return ToInt(anode.value());
  } else {
    return defaultValue;
  }
}

static bool GetAttribBool(const pugi::xml_node &XmlNode, const string &attrib, bool defaultValue = false) {
  if (auto anode = XmlNode.attribute(attrib.c_str())) {
    return anode.as_bool();
  } else {
    return defaultValue;
  }
}

static Ogre::Vector3 ParseVector3(const pugi::xml_node &XmlNode) {
  float x = ToFloat(XmlNode.attribute("x").value());
  float y = ToFloat(XmlNode.attribute("y").value());
  float z = ToFloat(XmlNode.attribute("z").value());

  return Ogre::Vector3(x, y, z);
}

static Ogre::Vector3 ParsePosition(const pugi::xml_node &XmlNode) {
  float x = ToFloat(XmlNode.attribute("x").value());
  float y = ToFloat(XmlNode.attribute("y").value());
  float z = ToFloat(XmlNode.attribute("z").value());

  return Ogre::Vector3(x, y, z);
}

static Ogre::Vector3 ParseScale(const pugi::xml_node &XmlNode) {
  float x = ToFloat(XmlNode.attribute("x").value());
  float y = ToFloat(XmlNode.attribute("y").value());
  float z = ToFloat(XmlNode.attribute("z").value());

  return Ogre::Vector3(x, y, z);
}

static Ogre::Quaternion ParseRotation(const pugi::xml_node &XmlNode) {
  Ogre::Quaternion orientation;

  if (XmlNode.attribute("qw")) {
    orientation.w = ToFloat(XmlNode.attribute("qw").value());
    orientation.x = ToFloat(XmlNode.attribute("qx").value());
    orientation.y = -ToFloat(XmlNode.attribute("qz").value());
    orientation.z = ToFloat(XmlNode.attribute("qy").value());
  } else if (XmlNode.attribute("axisX")) {
    Ogre::Vector3 axis;
    axis.x = ToFloat(XmlNode.attribute("axisX").value());
    axis.y = ToFloat(XmlNode.attribute("axisY").value());
    axis.z = ToFloat(XmlNode.attribute("axisZ").value());
    float angle = ToFloat(XmlNode.attribute("angle").value());

    orientation.FromAngleAxis(Ogre::Angle(angle), axis);
  } else if (XmlNode.attribute("angleX")) {
    Ogre::Matrix3 rot;
    rot.FromEulerAnglesXYZ(StringConverter::parseAngle(XmlNode.attribute("angleX").value()),
                           StringConverter::parseAngle(XmlNode.attribute("angleY").value()),
                           StringConverter::parseAngle(XmlNode.attribute("angleZ").value()));
    orientation.FromRotationMatrix(rot);
  } else if (XmlNode.attribute("x")) {
    orientation.x = ToFloat(XmlNode.attribute("x").value());
    orientation.y = ToFloat(XmlNode.attribute("y").value());
    orientation.z = ToFloat(XmlNode.attribute("z").value());
    orientation.w = ToFloat(XmlNode.attribute("w").value());
  } else if (XmlNode.attribute("w")) {
    orientation.w = ToFloat(XmlNode.attribute("w").value());
    orientation.x = ToFloat(XmlNode.attribute("x").value());
    orientation.y = ToFloat(XmlNode.attribute("y").value());
    orientation.z = ToFloat(XmlNode.attribute("z").value());
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

static Quaternion ParseQuaternion(const pugi::xml_node &XmlNode) {
  //! @todo Fix this crap!

  Quaternion orientation;

  if (XmlNode.attribute("qw")) {
    orientation.w = StringConverter::parseReal(XmlNode.attribute("qw").value());
    orientation.x = StringConverter::parseReal(XmlNode.attribute("qx").value());
    orientation.y = StringConverter::parseReal(XmlNode.attribute("qy").value());
    orientation.z = StringConverter::parseReal(XmlNode.attribute("qz").value());
  } else if (XmlNode.attribute("axisX")) {
    Vector3 axis;
    axis.x = StringConverter::parseReal(XmlNode.attribute("axisX").value());
    axis.y = StringConverter::parseReal(XmlNode.attribute("axisY").value());
    axis.z = StringConverter::parseReal(XmlNode.attribute("axisZ").value());
    Real angle = StringConverter::parseReal(XmlNode.attribute("angle").value());

    orientation.FromAngleAxis(Radian(angle), axis);
  } else if (XmlNode.attribute("angleX")) {
    Matrix3 rot;
    rot.FromEulerAnglesXYZ(StringConverter::parseAngle(XmlNode.attribute("angleX").value()),
                           StringConverter::parseAngle(XmlNode.attribute("angleY").value()),
                           StringConverter::parseAngle(XmlNode.attribute("angleZ").value()));
    orientation.FromRotationMatrix(rot);
  } else if (XmlNode.attribute("x")) {
    orientation.x = StringConverter::parseReal(XmlNode.attribute("x").value());
    orientation.y = StringConverter::parseReal(XmlNode.attribute("y").value());
    orientation.z = StringConverter::parseReal(XmlNode.attribute("z").value());
    orientation.w = StringConverter::parseReal(XmlNode.attribute("w").value());
  } else if (XmlNode.attribute("w")) {
    orientation.w = StringConverter::parseReal(XmlNode.attribute("w").value());
    orientation.x = StringConverter::parseReal(XmlNode.attribute("x").value());
    orientation.y = StringConverter::parseReal(XmlNode.attribute("y").value());
    orientation.z = StringConverter::parseReal(XmlNode.attribute("z").value());
  }

  return orientation;
}

static Ogre::ColourValue ParseColour(pugi::xml_node &XmlNode) {
  return Ogre::ColourValue(ToFloat(XmlNode.attribute("r").value()) / 255.0, ToFloat(XmlNode.attribute("g").value()) / 255.0,
                           ToFloat(XmlNode.attribute("b").value()) / 255.0, ToFloat(XmlNode.attribute("a").value(), 255.0) / 255.0);
}

namespace Glue {

DotSceneLoaderB::DotSceneLoaderB() {}

DotSceneLoaderB::~DotSceneLoaderB() {}

void DotSceneLoaderB::Load(DataStreamPtr &stream, const string &group_name, SceneNode *root_node) {
  this->GroupName = group_name;
  OgreScene = root_node->getCreator();

  pugi::xml_document XMLDoc;  // character type defaults to char

  auto result = XMLDoc.load_buffer(stream->getAsString().c_str(), stream->size());

  if (!result) {
    LogManager::getSingleton().stream(LML_CRITICAL) << "[DotSceneLoader] " << result.description();
    return;
  }

  // Grab the scene node
  auto XMLRoot = XMLDoc.child("scene");

  // Validate the File
  if (!XMLRoot.attribute("formatVersion")) {
    LogManager::getSingleton().logError(
        "[DotSceneLoader] Invalid .scene File. Missing <scene "
        "formatVersion='x.y' >");
    return;
  }

  // figure out where to attach any nodes we init
  OgreRoot = Root::getSingletonPtr();
  AttachNode = root_node;
  RootNode = root_node;
  // Process the scene
  ProcessScene(XMLRoot);
}

static void Write(pugi::xml_node &node, const Vector3 &v) {
  node.append_attribute("x") = ToCString(v.x);
  node.append_attribute("y") = ToCString(v.y);
  node.append_attribute("z") = ToCString(v.z);
}

static void Write(pugi::xml_node &node, const ColourValue &c) {
  node.append_attribute("r") = ToCString(c.r);
  node.append_attribute("g") = ToCString(c.g);
  node.append_attribute("b") = ToCString(c.b);
  node.append_attribute("a") = ToCString(c.a);
}

void DotSceneLoaderB::WriteNode(pugi::xml_node &parentXML, const SceneNode *node) {
  auto nodeXML = parentXML.append_child("node");
  if (!node->getName().empty()) nodeXML.append_attribute("name") = node->getName().c_str();

  auto pos = nodeXML.append_child("position");
  Write(pos, node->getPosition());

  auto scale = nodeXML.append_child("scale");
  Write(scale, node->getScale());

  auto rot = nodeXML.append_child("rotation");
  rot.append_attribute("qw") = ToCString(node->getOrientation().w);
  rot.append_attribute("qx") = ToCString(node->getOrientation().x);
  rot.append_attribute("qy") = ToCString(node->getOrientation().y);
  rot.append_attribute("qz") = ToCString(node->getOrientation().z);

  for (auto mo : node->getAttachedObjects()) {
    if (auto c = dynamic_cast<Camera *>(mo)) {
      auto camera = nodeXML.append_child("camera");
      camera.append_attribute("name") = c->getName().c_str();
      auto clipping = camera.append_child("clipping");
      clipping.append_attribute("near") = ToCString(c->getNearClipDistance());
      clipping.append_attribute("far") = ToCString(c->getFarClipDistance());
      continue;
    }

    if (auto l = dynamic_cast<Light *>(mo)) {
      auto light = nodeXML.append_child("light");
      light.append_attribute("name") = l->getName().c_str();
      light.append_attribute("castShadows") = ToCString(l->getCastShadows());

      if (!l->isVisible()) light.append_attribute("visible") = "false";

      auto diffuse = light.append_child("colourDiffuse");
      Write(diffuse, l->getDiffuseColour());
      auto specular = light.append_child("colourSpecular");
      Write(specular, l->getSpecularColour());
      switch (l->getType()) {
        case Light::LT_POINT:
          light.append_attribute("type") = "point";
          break;
        case Light::LT_DIRECTIONAL:
          light.append_attribute("type") = "directional";
          break;
        case Light::LT_SPOTLIGHT:
          light.append_attribute("type") = "spot";
          break;
      }

      if (l->getType() != Light::LT_DIRECTIONAL) {
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

    if (auto e = dynamic_cast<Entity *>(mo)) {
      auto entity = nodeXML.append_child("entity");
      entity.append_attribute("name") = e->getName().c_str();
      entity.append_attribute("meshFile") = e->getMesh()->getName().c_str();

      if (!e->isVisible()) entity.append_attribute("visible") = "false";

      // Heuristic: assume first submesh is representative
      auto sub0mat = e->getSubEntity(0)->getMaterial();
      if (sub0mat != e->getMesh()->getSubMesh(0)->getMaterial()) entity.append_attribute("material") = sub0mat->getName().c_str();
      continue;
    }

    LogManager::getSingleton().logWarning("DotSceneLoader - unsupported MovableType " + mo->getMovableType());
  }

  // recurse
  for (auto c : node->getChildren()) WriteNode(nodeXML, static_cast<SceneNode *>(c));
}

void DotSceneLoaderB::ExportScene(SceneNode *rootNode, const string &outFileName) {
  pugi::xml_document XMLDoc;  // character type defaults to char
  auto comment = XMLDoc.append_child(pugi::node_comment);
  comment.set_value(StringUtil::format(" exporter: Plugin_DotScene %d.%d.%d ", OGRE_VERSION_MAJOR, OGRE_VERSION_MINOR, OGRE_VERSION_PATCH).c_str());
  auto scene = XMLDoc.append_child("scene");
  scene.append_attribute("formatVersion") = "1.1";
  scene.append_attribute("sceneManager") = rootNode->getCreator()->getTypeName().c_str();

  auto nodes = scene.append_child("nodes");

  for (auto c : rootNode->getChildren()) WriteNode(nodes, static_cast<SceneNode *>(c));

  XMLDoc.save_file(outFileName.c_str());
}

void DotSceneLoaderB::ProcessScene(pugi::xml_node &XmlRoot) {
  // Process the scene parameters
  string version = GetAttrib(XmlRoot, "formatVersion", "unknown");

  string message = "[DotSceneLoader] Parsing dotScene file with version " + version;

  if (XmlRoot.attribute("sceneManager")) {
    message += ", scene manager " + string(XmlRoot.attribute("sceneManager").value());
  }

  if (XmlRoot.attribute("minOgreVersion")) {
    message += ", min. Ogre version " + string(XmlRoot.attribute("minOgreVersion").value());
  }

  if (XmlRoot.attribute("author")) message += ", author " + string(XmlRoot.attribute("author").value());

  LogManager::getSingleton().logMessage(message);

  if (auto element = XmlRoot.child("environment")) ProcessEnvironment(element);

  if (auto element = XmlRoot.child("terrainGroup")) ProcessTerrainGroup(element);

  if (auto element = XmlRoot.child("nodes")) ProcessNodes(element);

  if (auto element = XmlRoot.child("externals")) ProcessExternals(element);

  if (auto element = XmlRoot.child("forest")) ProcessForests(element);

  if (auto element = XmlRoot.child("skyBox")) ProcessSkyBox(element);

  if (auto element = XmlRoot.child("skyDome")) ProcessSkyDome(element);

  if (auto element = XmlRoot.child("skyPlane")) ProcessSkyPlane(element);

  if (auto element = XmlRoot.child("userData")) ProcessUserData(element, AttachNode->getUserObjectBindings());
}

void DotSceneLoaderB::ProcessNodes(pugi::xml_node &XmlNode) {
  // Process node (*)
  for (auto element : XmlNode.children("node")) {
    ProcessNode(element);
  }

  // Process position
  if (auto element = XmlNode.child("position")) {
    AttachNode->setPosition(ParsePosition(element));
    AttachNode->setInitialState();
  }

  // Process rotation
  if (auto element = XmlNode.child("rotation")) {
    AttachNode->setOrientation(ParseRotation(element));
    AttachNode->setInitialState();
  }

  // Process scale
  if (auto element = XmlNode.child("scale")) {
    AttachNode->setScale(ParseScale(element));
    AttachNode->setInitialState();
  }
}

void DotSceneLoaderB::ProcessExternals(pugi::xml_node &XmlNode) {
  //! @todo Implement this
}

void DotSceneLoaderB::ProcessEnvironment(pugi::xml_node &XmlNode) {
  // Process camera
  auto *viewport = Root::getSingleton().getSceneManager("Default")->getCamera("Default")->getViewport();

  // Process fog
  if (auto element = XmlNode.child("fog")) ProcessFog(element);

  // Process skyBox
  if (auto element = XmlNode.child("skyBox")) ProcessSkyBox(element);

  // Process skyDome
  if (auto element = XmlNode.child("skyDome")) ProcessSkyDome(element);

  // Process skyPlane
  if (auto element = XmlNode.child("skyPlane")) ProcessSkyPlane(element);

  // Process colourAmbient
  if (auto element = XmlNode.child("colourAmbient")) OgreScene->setAmbientLight(ParseColour(element));

  if (auto element = XmlNode.child("shadowColor")) OgreScene->setShadowColour(ParseColour(element));

  // Process colourBackground
  if (auto element = XmlNode.child("colourBackground")) viewport->setBackgroundColour(ParseColour(element));
}

void DotSceneLoaderB::ProcessLightRange(pugi::xml_node &XmlNode, Light *light) {
  // Process attributes
  float inner = GetAttribReal(XmlNode, "inner");
  float outer = GetAttribReal(XmlNode, "outer");
  float falloff = GetAttribReal(XmlNode, "falloff", 1.0);

  // Setup the light range
  light->setSpotlightRange(Radian(inner), Radian(outer), falloff);
}

void DotSceneLoaderB::ProcessLightAttenuation(pugi::xml_node &XmlNode, Light *light) {
  // Process attributes
  float range = GetAttribReal(XmlNode, "range");
  float constant = GetAttribReal(XmlNode, "constant");
  float linear = GetAttribReal(XmlNode, "linear");
  float quadratic = GetAttribReal(XmlNode, "quadratic");

  // Setup the light attenuation
  light->setAttenuation(range, constant, linear, quadratic);
}

void DotSceneLoaderB::ProcessLight(pugi::xml_node &XmlNode, SceneNode *ParentNode) {
  // Process attributes
  string name = GetAttrib(XmlNode, "name");
  const size_t MAX_TEX_COUNT = 9;

  // SetUp the light
  Light *light = OgreScene->createLight(name);
  ParentNode->attachObject(light);

  string sValue = GetAttrib(XmlNode, "type");

  if (sValue == "point")
    light->setType(Light::LT_POINT);
  else if (sValue == "directional")
    light->setType(Light::LT_DIRECTIONAL);
  else if (sValue == "spot")
    light->setType(Light::LT_SPOTLIGHT);
  else if (sValue == "radPoint")
    light->setType(Light::LT_POINT);

  light->setVisible(GetAttribBool(XmlNode, "visible", true));
  light->setCastShadows(GetAttribBool(XmlNode, "castShadows", false));

  if (Root::getSingleton().getSceneManager("Default")->getShadowTechnique() != SHADOWTYPE_NONE) {
    auto texture_config = OgreScene->getShadowTextureConfigList()[0];

    if (light->getType() == Light::LT_POINT) light->setCastShadows(false);

    if (OgreScene->getShadowTextureConfigList().size() < MAX_TEX_COUNT && light->getCastShadows()) {
      if (light->getType() == Light::LT_SPOTLIGHT) {
        static ShadowCameraSetupPtr default_scs = LiSPSMShadowCameraSetup::create();

        light->setCustomShadowCameraSetup(default_scs);
        size_t tex_count = OgreScene->getShadowTextureConfigList().size() + 1;
        OgreScene->setShadowTextureCount(tex_count);

        size_t index = tex_count - 1;
        texture_config.height *= pow(2.0, -floor(index / 3.0));
        texture_config.width *= pow(2.0, -floor(index / 3.0));
        OgreScene->setShadowTextureConfig(index, texture_config);
      } else if (light->getType() == Light::LT_DIRECTIONAL) {
        size_t per_light = OgreScene->getShadowTextureCountPerLightType(Light::LT_DIRECTIONAL);
        size_t tex_count = OgreScene->getShadowTextureConfigList().size() + per_light - 1;
        OgreScene->setShadowTextureCount(tex_count);

        for (size_t i = 1; i <= per_light; i++) {
          size_t index = tex_count - i;
          OgreScene->setShadowTextureConfig(index, texture_config);
        }
      }
    } else {
      light->setCastShadows(false);
    }
  } else {
    light->setCastShadows(false);
  }

  light->setPowerScale(GetAttribReal(XmlNode, "powerScale", 1.0));

  // Process colourDiffuse
  if (auto element = XmlNode.child("colourDiffuse")) light->setDiffuseColour(ParseColour(element));

  // Process colourSpecular
  if (auto element = XmlNode.child("colourSpecular")) light->setSpecularColour(ParseColour(element));

  // Process lightAttenuation
  if (light->getType() != Light::LT_DIRECTIONAL)
    if (auto element = XmlNode.child("lightAttenuation")) ProcessLightAttenuation(element, light);

  // Process lightRange
  if (light->getType() != Light::LT_SPOTLIGHT)
    if (auto element = XmlNode.child("lightRange")) ProcessLightRange(element, light);

  // Process userDataReference
  if (auto element = XmlNode.child("userData")) ProcessUserData(element, light->getUserObjectBindings());
}

void DotSceneLoaderB::ProcessCamera(pugi::xml_node &XmlNode, SceneNode *ParentNode) {
  // Process attributes
  string name = GetAttrib(XmlNode, "name");
  float fov = GetAttribReal(XmlNode, "fov", 90.0);
  string projectionType = GetAttrib(XmlNode, "projectionType", "perspective");
  OgreCameraPtr = OgreScene->getCamera("Default");
  ParentNode->attachObject(OgreCameraPtr);

  // Set the field-of-view
  OgreCameraPtr->setFOVy(Radian(fov));

  // Set the projection type
  if (projectionType == "perspective")
    OgreCameraPtr->setProjectionType(PT_PERSPECTIVE);
  else if (projectionType == "orthographic")
    OgreCameraPtr->setProjectionType(PT_ORTHOGRAPHIC);

  // Process clipping
  if (auto element = XmlNode.child("clipping")) {
    float nearDist = GetAttribReal(element, "near");
    OgreCameraPtr->setNearClipDistance(nearDist);

    float farDist = GetAttribReal(element, "far");
    OgreCameraPtr->setFarClipDistance(farDist);
  }

  GetScene().AddCamera(OgreCameraPtr);

  if (GetScene().GetCameraMan().GetStyle() == CameraMan::MANUAL) GetScene().AddSinbad(OgreCameraPtr);

  // Process userDataReference
  if (auto element = XmlNode.child("userData")) ProcessUserData(element, static_cast<MovableObject *>(OgreCameraPtr)->getUserObjectBindings());
}

void DotSceneLoaderB::ProcessNode(pugi::xml_node &XmlNode, SceneNode *ParentNode) {
  // Construct the node's name
  string name = GetAttrib(XmlNode, "name");

  // SetUp the scene node
  SceneNode *node;

  if (name.empty()) {
    // Let Ogre choose the name
    if (ParentNode)
      node = ParentNode->createChildSceneNode();
    else
      node = AttachNode->createChildSceneNode();
  } else {
    // Provide the name
    if (ParentNode)
      node = ParentNode->createChildSceneNode(name);
    else
      node = AttachNode->createChildSceneNode(name);
  }

  // Process other attributes

  // Process position
  if (auto element = XmlNode.child("position")) {
    Vector3 position = ParsePosition(element);
    if (!ParentNode) {
      position.y += GetScene().GetHeight(position.x, position.z);
    }
    node->setPosition(position);
    node->setInitialState();
  }

  // Process rotation
  if (auto element = XmlNode.child("rotation")) {
    node->setOrientation(ParseRotation(element));
    node->setInitialState();
  }

  // Process rotation
  if (auto element = XmlNode.child("direction")) {
    node->setDirection(ParsePosition(element).normalisedCopy());
    node->setInitialState();
  }

  // Process scale
  if (auto element = XmlNode.child("scale")) {
    node->setScale(ParseScale(element));
    node->setInitialState();
  }

  // Process lookTarget
  if (auto element = XmlNode.child("lookTarget")) ProcessLookTarget(element, node);

  // Process trackTarget
  if (auto element = XmlNode.child("trackTarget")) ProcessTrackTarget(element, node);

  // Process node (*)
  for (auto element : XmlNode.children("node")) ProcessNode(element, node);

  // Process entity (*)
  for (auto element : XmlNode.children("entity")) ProcessEntity(element, node);

  // Process light (*)
  for (auto element : XmlNode.children("light")) ProcessLight(element, node);

  for (auto element : XmlNode.children("particleSystem")) ProcessParticleSystem(element, node);

  // Process billboardSet (*)
  for (auto element : XmlNode.children("billboardSet")) ProcessBillboardSet(element, node);

  // Process plane (*)
  for (auto element : XmlNode.children("plane")) ProcessPlane(element, node);

  // Process camera (*)
  for (auto element : XmlNode.children("camera")) ProcessCamera(element, node);

  // Process userDataReference
  if (auto element = XmlNode.child("userData")) ProcessUserData(element, node->getUserObjectBindings());

  // Process node animations (?)
  if (auto element = XmlNode.child("animations")) ProcessNodeAnimations(element, node);
}

void DotSceneLoaderB::ProcessLookTarget(pugi::xml_node &XmlNode, SceneNode *ParentNode) {
  // Process attributes
  string nodeName = GetAttrib(XmlNode, "nodeName");

  Node::TransformSpace relativeTo = Node::TS_PARENT;
  string sValue = GetAttrib(XmlNode, "relativeTo");

  if (sValue == "local")
    relativeTo = Node::TS_LOCAL;
  else if (sValue == "ParentNode")
    relativeTo = Node::TS_PARENT;
  else if (sValue == "world")
    relativeTo = Node::TS_WORLD;

  // Process position
  Vector3 position;

  if (auto element = XmlNode.child("position")) position = ParseVector3(element);

  // Process localDirection
  Vector3 localDirection = Vector3::NEGATIVE_UNIT_Z;

  if (auto element = XmlNode.child("localDirection")) localDirection = ParseVector3(element);

  // Setup the look target
  try {
    if (!nodeName.empty()) {
      SceneNode *pLookNode = OgreScene->getSceneNode(nodeName);
      position = pLookNode->_getDerivedPosition();
    }

    ParentNode->lookAt(position, relativeTo, localDirection);
  } catch (Exception &e) {
    LogManager::getSingleton().logMessage("[DotSceneLoader] Error processing a look target!");
  }
}

void DotSceneLoaderB::ProcessTrackTarget(pugi::xml_node &XmlNode, SceneNode *ParentNode) {
  // Process attributes
  string nodeName = GetAttrib(XmlNode, "nodeName");

  // Process localDirection
  Vector3 localDirection = Vector3::NEGATIVE_UNIT_Z;

  if (auto element = XmlNode.child("localDirection")) localDirection = ParseVector3(element);

  // Process offset
  Vector3 offset = Vector3::ZERO;

  if (auto element = XmlNode.child("offset")) offset = ParseVector3(element);

  // Setup the track target
  try {
    SceneNode *pTrackNode = OgreScene->getSceneNode(nodeName);
    ParentNode->setAutoTracking(true, pTrackNode, localDirection, offset);
  } catch (Exception &e) {
    LogManager::getSingleton().logMessage("[DotSceneLoader] Error processing a track target!");
  }
}

void DotSceneLoaderB::ProcessEntity(pugi::xml_node &XmlNode, SceneNode *ParentNode) {
  // Process attributes
  string name = GetAttrib(XmlNode, "name");
  LogManager::getSingleton().logMessage("[DotSceneLoader] Processing Entity: " + name, LML_TRIVIAL);
  string meshFile = GetAttrib(XmlNode, "meshFile");
  String staticGeometry = GetAttrib(XmlNode, "static");
  String instancedManager = GetAttrib(XmlNode, "instanced");
  string material = GetAttrib(XmlNode, "material");
  bool castShadows = GetAttribBool(XmlNode, "castShadows", true);
  bool visible = GetAttribBool(XmlNode, "visible", true);

  // SetUp the entity
  MovableObject *pEntity = OgreScene->createEntity(name, meshFile);

  try {
    // If the Entity is instanced then the creation path is different
    if (!instancedManager.empty()) {
      LogManager::getSingleton().logMessage("[DotSceneLoader] Adding entity: " + name + " to Instance Manager: " + instancedManager, LML_TRIVIAL);

      // Load the Mesh to get the material name of the first submesh
      MeshPtr mesh = MeshManager::getSingletonPtr()->load(meshFile, GroupName);

      // Get the material name of the entity
      if (!material.empty())
        pEntity = OgreScene->createInstancedEntity(material, instancedManager);
      else
        pEntity = OgreScene->createInstancedEntity(mesh->getSubMesh(0)->getMaterialName(), instancedManager);

      ParentNode->attachObject(static_cast<InstancedEntity *>(pEntity));
    } else {
      pEntity = OgreScene->createEntity(name, meshFile, GroupName);

      static_cast<Entity *>(pEntity)->setCastShadows(castShadows);
      static_cast<Entity *>(pEntity)->setVisible(visible);

      if (!material.empty()) static_cast<Entity *>(pEntity)->setMaterialName(material);

      GetScene().AddEntity(static_cast<Entity *>(pEntity));

      // If the Entity belongs to a Static Geometry group then it doesn't get attached to a node
      // * TODO * : Clean up nodes without attached entities or children nodes? (should be done afterwards if the hierarchy is being processed)
      if (!staticGeometry.empty()) {
        LogManager::getSingleton().logMessage("[DotSceneLoader] Adding entity: " + name + " to Static Group: " + staticGeometry, LML_TRIVIAL);
        OgreScene->getStaticGeometry(staticGeometry)
            ->addEntity(static_cast<Entity *>(pEntity), ParentNode->_getDerivedPosition(), ParentNode->_getDerivedOrientation(),
                        ParentNode->_getDerivedScale());
      } else {
        LogManager::getSingleton().logMessage("[DotSceneLoader] pParent->attachObject(): " + name, LML_TRIVIAL);
        ParentNode->attachObject(static_cast<Entity *>(pEntity));
      }
    }
  } catch (Ogre::Exception &e) {
    LogManager::getSingleton().logError("DotSceneLoader - " + e.getDescription());
    return;
  }

  // Process userDataReference
  if (auto element = XmlNode.child("userData")) {
    ProcessUserData(element, pEntity->getUserObjectBindings());
    GetPhysics().ProcessData(static_cast<Entity *>(pEntity), ParentNode, pEntity->getUserObjectBindings());
  } else {
    GetPhysics().ProcessData(static_cast<Entity *>(pEntity), ParentNode);
  }
}

void DotSceneLoaderB::ProcessParticleSystem(pugi::xml_node &XmlNode, SceneNode *ParentNode) {
  // Process attributes
  string name = GetAttrib(XmlNode, "name");
  string templateName = GetAttrib(XmlNode, "template");
  bool attachedCamera = GetAttribBool(XmlNode, "attachedCamera", false);

  if (templateName.empty()) {
    templateName = GetAttrib(XmlNode, "file");  // compatibility with old scenes
  }

  // SetUp the particle system
  try {
    ParticleSystem *pParticles = OgreScene->createParticleSystem(name, templateName);

    const uint32 WATER_MASK = 0xF00;
    pParticles->setVisibilityFlags(WATER_MASK);

    if (attachedCamera) {
      auto *camera = Root::getSingleton().getSceneManager("Default")->getCamera("Default");
      camera->getParentSceneNode()->createChildSceneNode(Vector3{0, 10, 0})->attachObject(pParticles);
    } else {
      ParentNode->attachObject(pParticles);
    }
  } catch (Exception &e) {
    LogManager::getSingleton().logMessage("[DotSceneLoader] Error creating a particle system!");
  }
}

void DotSceneLoaderB::ProcessBillboardSet(pugi::xml_node &XmlNode, SceneNode *ParentNode) {
  //! @todo Implement this
}

void DotSceneLoaderB::ProcessPlane(pugi::xml_node &XmlNode, SceneNode *ParentNode) {
  string name = GetAttrib(XmlNode, "name");
  float distance = GetAttribReal(XmlNode, "distance", 0.0f);
  float width = GetAttribReal(XmlNode, "width", 1.0f);
  float height = GetAttribReal(XmlNode, "height", width);
  int xSegments = ToInt(GetAttrib(XmlNode, "xSegments"), width / 5.0f);
  int ySegments = ToInt(GetAttrib(XmlNode, "ySegments"), height / 5.0f);
  int numTexCoordSets = ToInt(GetAttrib(XmlNode, "numTexCoordSets"), 1);
  float uTile = GetAttribReal(XmlNode, "uTile", width / 5.0f);
  float vTile = GetAttribReal(XmlNode, "vTile", height / 5.0f);
  string material = GetAttrib(XmlNode, "material", "BaseWhite");
  bool hasNormals = GetAttribBool(XmlNode, "hasNormals", true);
  Vector3 normal = ParseVector3(XmlNode.child("normal"));
  Vector3 up = ParseVector3(XmlNode.child("upVector"));
  bool Reflective = GetAttribBool(XmlNode, "reflection", false);

  MaterialPtr MaterialSPtr = MaterialManager::getSingleton().getByName(material);
  if (!MaterialSPtr) return;

  normal = {0, 1, 0};
  up = {0, 0, 1};
  Plane plane(normal, distance);

  string mesh_name = name + "_mesh";

  auto &OMM = MeshManager::getSingleton();
  MeshPtr plane_mesh = OMM.getByName(mesh_name);
  if (plane_mesh) OMM.remove(plane_mesh);

  MeshPtr res = OMM.createPlane(mesh_name, GroupName, plane, width, height, xSegments, ySegments, hasNormals, numTexCoordSets, uTile, vTile, up);
  res->buildTangentVectors();
  Entity *entity = OgreScene->createEntity(name, mesh_name);
  //  entity->setCastShadows(false);
  entity->setVisibilityFlags(0xF00);
  entity->setMaterialName(material);

  if (Reflective) {
    static unique_ptr<ReflectionCamera> rcamera;

    rcamera.reset();
    rcamera = make_unique<ReflectionCamera>(plane, 512);
    rcamera->SetPlane(plane);

    auto material_unit = MaterialSPtr->getTechnique(0)->getPass(0)->getTextureUnitState("ReflectionMap");

    if (material_unit) {
      material_unit->setTexture(rcamera->reflection_texture);
      material_unit->setTextureAddressingMode(TAM_CLAMP);
      material_unit->setTextureFiltering(FO_LINEAR, FO_LINEAR, FO_POINT);
    }
  }

  ParentNode->attachObject(entity);
  FixMaterial(MaterialSPtr);

  auto *entShape = BtOgre::createBoxCollider(entity);
  auto *bodyState = new BtOgre::RigidBodyState(ParentNode);
  auto *entBody = new btRigidBody(0, bodyState, entShape, btVector3(0, 0, 0));
  entBody->setFriction(1);
  GetPhysics().AddRigidBody(entBody);
}

void DotSceneLoaderB::ProcessForests(pugi::xml_node &XmlNode) {
  const float bound = 95;

  auto *grass = new PagedGeometry(OgreCameraPtr, 15);
  grass->addDetailLevel<GrassPage>(30, 0);
  auto *grassLoader = new GrassLoader(grass);
  grass->setPageLoader(grassLoader);
  grassLoader->setHeightFunction([](float x, float z, void *) { return GetScene().GetHeight(x, z); });

  GrassLayer *layer = grassLoader->addLayer("GrassCustom");
  layer->setFadeTechnique(FADETECH_ALPHA);
  layer->setRenderTechnique(GRASSTECH_CROSSQUADS);
  layer->setMaximumSize(2.0f, 2.0f);
  layer->setAnimationEnabled(true);
  layer->setSwayDistribution(10.0f);
  layer->setSwayLength(1.0f);
  layer->setSwaySpeed(0.5f);
  layer->setDensity(0.5f);
  layer->setMapBounds(TBounds(-bound, -bound, bound, bound));

  GetScene().AddForests(grass, "GrassCustom");

  float x = 0, y = 0, z = 0, yaw, scale = 1.0;
  auto *trees = new PagedGeometry(OgreCameraPtr, 50);

  trees->addDetailLevel<Forests::WindBatchPage>(125, 0);
  //  trees->addDetailLevel<Forests::BatchPage>(200, 0);
  trees->addDetailLevel<Forests::ImpostorPage>(400, 0);

  auto *treeLoader = new TreeLoader3D(trees, TBounds(-bound, -bound, bound, bound));
  FixTransparentShadowCaster("3D-Diggers/fir01");
  FixTransparentShadowCaster("leaf08png");

  trees->setPageLoader(treeLoader);
  Entity *fir2EntPtr = OgreScene->createEntity("fir1", "Tree.mesh");
  Entity *fir1EntPtr = OgreScene->createEntity("fir2", "fir05_30.mesh");

  trees->setCustomParam(fir1EntPtr->getName(), "windFactorX", 30);
  trees->setCustomParam(fir1EntPtr->getName(), "windFactorY", 0.01);
  trees->setCustomParam(fir2EntPtr->getName(), "windFactorX", 15);
  trees->setCustomParam(fir2EntPtr->getName(), "windFactorY", 0.01);

  for (int i = 0; i < 50; i++) {
    yaw = Math::RangeRandom(0, 360);
    Quaternion quat;

    if (Math::RangeRandom(0, 1) <= 0.8f) {
      x = Math::RangeRandom(-bound, bound);
      z = Math::RangeRandom(-bound, bound);
      if (x < -bound)
        x = -bound;
      else if (x > bound)
        x = bound;
      if (z < -bound)
        z = -bound;
      else if (z > bound)
        z = bound;
    } else {
      x = Math::RangeRandom(-bound, bound);
      z = Math::RangeRandom(-bound, bound);
    }

    y = GetScene().GetHeight(x, z);

    scale = Math::RangeRandom(0.9f, 1.1f);
    quat.FromAngleAxis(Degree(yaw), Vector3::UNIT_Y);

    treeLoader->addTree(fir1EntPtr, Vector3(x, y, z), Degree(yaw), scale * 0.2);

    //    if (Math::RangeRandom(0, 1) <= 0.8f) {
    //      x = Math::RangeRandom(-bound, bound);
    //      z = Math::RangeRandom(-bound, bound);
    //      if (x < -bound)
    //        x = -bound;
    //      else if (x > bound)
    //        x = bound;
    //      if (z < -bound)
    //        z = -bound;
    //      else if (z > bound)
    //        z = bound;
    //    } else {
    //      x = Math::RangeRandom(-bound, bound);
    //      z = Math::RangeRandom(-bound, bound);
    //    }
    //
    //    y = GetScene().GetHeight(x, z);
    //
    //    scale = Math::RangeRandom(0.9f, 1.1f);
    //    quat.FromAngleAxis(Degree(yaw), Vector3::UNIT_Y);
    //
    //    treeLoader->addTree(fir2EntPtr, Vector3(x, y, z), Degree(yaw), scale * 0.75);
  }

  trees->update();
  trees->setCastsShadows(false);
  GetScene().AddForests(trees);

  GetScene().AddMaterial("3D-Diggers/fir01_Batched");
  GetScene().AddMaterial("3D-Diggers/fir02_Batched");
  GetScene().AddMaterial("leaf08png_Batched");
  GetScene().AddMaterial("wood03png_Batched");

  ProcessStaticGeometry(XmlNode);
}

void DotSceneLoaderB::ProcessStaticGeometry(pugi::xml_node &XmlNode) {
  // create our grass mesh, and Init a grass entity from it
  Entity *rock = OgreScene->createEntity("Rock", "rock.mesh");
  FixEntityMaterial(rock);

  // Init a static geometry field, which we will populate with grass
  auto *rocks = OgreScene->createStaticGeometry("Rocks");

  const float bounds = 95.0f;
  // add grass uniformly throughout the field, with some random variations
  for (int i = 0; i < 250; i++) {
    Vector3 pos(Math::RangeRandom(-bounds, bounds), 0, Math::RangeRandom(-bounds, bounds));
    pos.y += GetScene().GetHeight(pos.x, pos.z) - 0.1;
    Quaternion ori(Degree(Math::RangeRandom(0, 359)), Vector3::UNIT_Y);
    Vector3 scale(2.0, 2.0 * Math::RangeRandom(0.85, 1.15), 2.0);

    auto *node = RootNode->createChildSceneNode(pos, ori);
    node->scale(scale);
    GetPhysics().ProcessData(rock, node);
    OgreScene->destroySceneNode(node);

    rocks->addEntity(rock, pos, ori, scale);
  }

  rocks->setRegionDimensions(Vector3(25.0));
  rocks->build();
  rocks->setCastShadows(true);
}

void DotSceneLoaderB::ProcessTerrainGroup(pugi::xml_node &XmlNode) {
  float worldSize = GetAttribReal(XmlNode, "worldSize");
  int mapSize = GetAttribInt(XmlNode, "mapSize");
  int inputScale = StringConverter::parseInt(XmlNode.attribute("inputScale").value());
  int tuningMaxPixelError = GetAttribInt(XmlNode, "tuningMaxPixelError", 8);

  auto *TGO = TerrainGlobalOptions::getSingletonPtr();
  if (!TGO) TGO = new TerrainGlobalOptions();
  TGO->setUseVertexCompressionWhenAvailable(true);
  TGO->setCastsDynamicShadows(false);
  TGO->setCompositeMapDistance(100);
  TGO->setMaxPixelError(8.0);
  TGO->setUseRayBoxDistanceCalculation(false);
  TGO->setDefaultMaterialGenerator(make_shared<TerrainMaterialGeneratorB>());

  TerrainGroup *OgreTerrainPtr = new TerrainGroup(OgreScene, Terrain::ALIGN_X_Z, mapSize, worldSize);
  OgreTerrainPtr->setOrigin(Vector3::ZERO);
  OgreTerrainPtr->setResourceGroup(GroupName);

  Terrain::ImportData &defaultimp = OgreTerrainPtr->getDefaultImportSettings();
  //  defaultimp.terrainSize = mapSize;
  //  defaultimp.worldSize = worldSize;
  defaultimp.inputScale = inputScale;
  defaultimp.minBatchSize = 17;
  defaultimp.maxBatchSize = 33;

  for (auto &pPageElement : XmlNode.children("terrain")) {
    int pageX = StringConverter::parseInt(pPageElement.attribute("x").value());
    int pageY = StringConverter::parseInt(pPageElement.attribute("y").value());
    string cached = pPageElement.attribute("heightmap").value();
    Image image;

    if (ResourceGroupManager::getSingleton().resourceExists(GroupName, cached)) {
      image.load(cached, GroupName);
      OgreTerrainPtr->defineTerrain(pageX, pageY, &image);
    } else {
      OgreTerrainPtr->defineTerrain(pageX, pageY, 0.0f);
    }
  }

  OgreTerrainPtr->loadAllTerrains(true);

  OgreTerrainPtr->freeTemporaryResources();

  auto terrainIterator = OgreTerrainPtr->getTerrainIterator();

  while (terrainIterator.hasMoreElements()) {
    auto *terrain = terrainIterator.getNext()->instance;

    GetPhysics().CreateTerrainHeightfieldShape(terrain->getSize(), terrain->getHeightData(), terrain->getMinHeight(), terrain->getMaxHeight(),
                                               terrain->getPosition(), terrain->getWorldSize() / (static_cast<float>(terrain->getSize() - 1)));
  }

  GetScene().AddTerrain(OgreTerrainPtr);
}

void DotSceneLoaderB::ProcessFog(pugi::xml_node &XmlNode) {
  // Process attributes
  float expDensity = GetAttribReal(XmlNode, "density", 0.001);
  float linearStart = GetAttribReal(XmlNode, "start", 0.0);
  float linearEnd = GetAttribReal(XmlNode, "end", 1.0);
  ColourValue colour = ColourValue::White;

  FogMode mode = FOG_EXP;
  string sMode = GetAttrib(XmlNode, "mode");

  if (auto element = XmlNode.child("colour")) colour = ParseColour(element);

  // Setup the fog
  OgreScene->setFog(mode, colour, expDensity, linearStart, linearEnd);
}

void DotSceneLoaderB::ProcessSkyBox(pugi::xml_node &XmlNode) {
  // Process attributes
  string material = GetAttrib(XmlNode, "material", "SkyBox");
  string cubemap = GetAttrib(XmlNode, "cubemap", "");
  float distance = GetAttribReal(XmlNode, "distance", 500);
  bool drawFirst = GetAttribBool(XmlNode, "drawFirst", true);
  bool active = GetAttribBool(XmlNode, "active", true);

  // Process rotation
  Quaternion rotation = Quaternion::IDENTITY;

  if (auto element = XmlNode.child("rotation")) rotation = ParseRotation(element);

  MaterialPtr material_ptr = MaterialManager::getSingleton().getByName(material);

  if (material_ptr->getTechnique(0)->getPass(0)->getNumTextureUnitStates() > 0 && !cubemap.empty()) {
    auto texture_unit = material_ptr->getTechnique(0)->getPass(0)->getTextureUnitState(0);
    if (texture_unit) texture_unit->setTextureName(cubemap);
  }

  // Setup the sky box
  OgreScene->setSkyBox(true, material, distance, drawFirst, rotation, GroupName);
  GetScene().AddSkyBox();
}

void DotSceneLoaderB::ProcessSkyDome(pugi::xml_node &XmlNode) {
  // Process attributes
  string material = XmlNode.attribute("material").value();
  float curvature = GetAttribReal(XmlNode, "curvature", 10);
  float tiling = GetAttribReal(XmlNode, "tiling", 8);
  float distance = GetAttribReal(XmlNode, "distance", 500);
  bool drawFirst = GetAttribBool(XmlNode, "drawFirst", true);
  bool active = GetAttribBool(XmlNode, "active", false);

  // Process rotation
  Quaternion rotation = Quaternion::IDENTITY;

  if (auto element = XmlNode.child("rotation")) rotation = ParseRotation(element);

  // Setup the sky dome
  OgreScene->setSkyDome(true, material, curvature, tiling, distance, drawFirst, rotation, 16, 16, -1, GroupName);
}

void DotSceneLoaderB::ProcessSkyPlane(pugi::xml_node &XmlNode) {
  // Process attributes
  string material = GetAttrib(XmlNode, "material");
  float planeX = GetAttribReal(XmlNode, "planeX", 0);
  float planeY = GetAttribReal(XmlNode, "planeY", -1);
  float planeZ = GetAttribReal(XmlNode, "planeZ", 0);
  float planeD = GetAttribReal(XmlNode, "planeD", 5000);
  float scale = GetAttribReal(XmlNode, "scale", 1000);
  float bow = GetAttribReal(XmlNode, "bow", 0);
  float tiling = GetAttribReal(XmlNode, "tiling", 10);
  bool drawFirst = GetAttribBool(XmlNode, "drawFirst", true);

  // Setup the sky plane
  Plane plane;
  plane.normal = Vector3(planeX, planeY, planeZ);
  plane.d = planeD;
  OgreScene->setSkyPlane(true, plane, material, scale, tiling, drawFirst, bow, 1, 1, GroupName);
}

void DotSceneLoaderB::ProcessUserData(pugi::xml_node &XmlNode, UserObjectBindings &user_object_bindings) {
  // Process node (*)
  for (auto element : XmlNode.children("property")) {
    string name = GetAttrib(element, "name");
    string type = GetAttrib(element, "type");
    string data = GetAttrib(element, "data");

    Any value;

    if (type == "bool")
      value = (ToInt(data) != 0);
    else if (type == "float")
      value = ToFloat(data);
    else if (type == "int")
      value = ToInt(data);
    else
      value = data;

    user_object_bindings.setUserAny(name, value);
  }
}

void DotSceneLoaderB::ProcessNodeAnimations(pugi::xml_node &XMLNode, SceneNode *pParent) {
  LogManager::getSingleton().logMessage("[DotSceneLoader] Processing Node Animations for SceneNode: " + pParent->getName(), LML_TRIVIAL);

  // Process node animations (*)
  for (auto pElement : XMLNode.children("animation")) {
    ProcessNodeAnimation(pElement, pParent);
  }
}

void DotSceneLoaderB::ProcessNodeAnimation(pugi::xml_node &XMLNode, SceneNode *pParent) {
  // Process node animation (*)

  // Construct the animation name
  String name = GetAttrib(XMLNode, "name");

  LogManager::getSingleton().logMessage("[DotSceneLoader] Processing Node Animation: " + name, LML_TRIVIAL);

  Real length = GetAttribReal(XMLNode, "length");

  Animation *anim = OgreScene->createAnimation(name, length);

  bool enable = GetAttribBool(XMLNode, "enable", false);
  bool loop = GetAttribBool(XMLNode, "loop", false);

  String interpolationMode = GetAttrib(XMLNode, "interpolationMode");

  if (interpolationMode == "linear")
    anim->setInterpolationMode(Animation::IM_LINEAR);
  else if (interpolationMode == "spline")
    anim->setInterpolationMode(Animation::IM_SPLINE);
  else
    LogManager::getSingleton().logError("DotSceneLoader - Invalid interpolationMode: " + interpolationMode);

  String rotationInterpolationMode = GetAttrib(XMLNode, "rotationInterpolationMode");

  if (rotationInterpolationMode == "linear")
    anim->setRotationInterpolationMode(Animation::RIM_LINEAR);
  else if (rotationInterpolationMode == "spherical")
    anim->setRotationInterpolationMode(Animation::RIM_SPHERICAL);
  else
    LogManager::getSingleton().logError("DotSceneLoader - Invalid rotationInterpolationMode: " + rotationInterpolationMode);

  // create a track to animate the camera's node
  NodeAnimationTrack *track = anim->createNodeTrack(0, pParent);

  // Process keyframes (*)
  for (auto pElement : XMLNode.children("keyframe")) {
    ProcessKeyframe(pElement, track);
  }

  // create a new animation state to track this
  auto animState = OgreScene->createAnimationState(name);
  animState->setEnabled(enable);
  animState->setLoop(loop);
}

void DotSceneLoaderB::ProcessKeyframe(pugi::xml_node &XMLNode, NodeAnimationTrack *pTrack) {
  // Process node animation keyframe (*)
  Real time = GetAttribReal(XMLNode, "time");

  LogManager::getSingleton().logMessage("[DotSceneLoader] Processing Keyframe: " + StringConverter::toString(time), LML_TRIVIAL);

  auto keyframe = pTrack->createNodeKeyFrame(time);

  // Process translation (?)
  if (auto pElement = XMLNode.child("position")) {
    Vector3 translation = ParseVector3(pElement);
    keyframe->setTranslate(translation);
  }

  // Process rotation (?)
  // Quaternion rotation = Quaternion::IDENTITY;
  if (auto pElement = XMLNode.child("rotation")) {
    Quaternion rotation = ParseQuaternion(pElement);
    keyframe->setRotation(rotation);
  }

  // Process scale (?)
  // Vector3 scale = parseVector3(XMLNode.child("scale"));
  if (auto pElement = XMLNode.child("scale")) {
    Vector3 scale = ParseVector3(pElement);
    keyframe->setScale(scale);
  }
}

struct DotSceneCodec : public Codec {
  string magicNumberToFileExt(const char *magicNumberPtr, size_t maxbytes) const { return ""; }
  string getType() const override { return "scene"; }
  void decode(const DataStreamPtr &stream, const Any &output) const override {
    DataStreamPtr _stream(stream);
    DotSceneLoaderB Loader;
    Loader.Load(_stream, ResourceGroupManager::getSingleton().getWorldResourceGroupName(), any_cast<SceneNode *>(output));
  }

  void encodeToFile(const Any &input, const String &outFileName) const override {
    DotSceneLoaderB Loader;
    Loader.ExportScene(any_cast<SceneNode *>(input), outFileName);
  }
};

const String &DotScenePluginB::getName() const {
  static String name = "DotScene Loader B";
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

}  // namespace Glue
