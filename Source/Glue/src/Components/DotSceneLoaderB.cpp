// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Components/DotSceneLoaderB.h"
#include "BtOgre/BtOgre.h"
#include "Components/ComponentLocator.h"
#include "MeshUtils.h"
#include "Objects/SinbadCharacterController.h"
#include "PBRUtils.h"
#include "XmlParser.h"
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

static void Write(pugi::xml_node &node, const Ogre::Vector3 &v) {
  node.append_attribute("x") = ToCString(v.x);
  node.append_attribute("y") = ToCString(v.y);
  node.append_attribute("z") = ToCString(v.z);
}

static void Write(pugi::xml_node &node, const Ogre::ColourValue &c) {
  node.append_attribute("r") = ToCString(c.r);
  node.append_attribute("g") = ToCString(c.g);
  node.append_attribute("b") = ToCString(c.b);
  node.append_attribute("a") = ToCString(c.a);
}

DotSceneLoaderB::DotSceneLoaderB() { camera_man = make_unique<CameraMan>(); }

DotSceneLoaderB::~DotSceneLoaderB() { Cleanup(); }

CameraMan &DotSceneLoaderB::GetCamera() const { return *camera_man; }

void DotSceneLoaderB::Update(float PassedTime) {
  if (Paused) return;
  if (camera_man) camera_man->Update(PassedTime);
  //  if (terrain) terrain->Update(PassedTime);
  //  if (forest) forest->Update(PassedTime);
  if (sinbad) sinbad->Update(PassedTime);
}

void DotSceneLoaderB::Load(Ogre::DataStreamPtr &stream, const string &group_name, Ogre::SceneNode *root_node) {
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

  //if (!terrain) terrain = make_unique<Landscape>();
  //if (!forest) forest = make_unique<Vegetation>();
  //forest->SetHeighFunc([](float x, float z) { return terrain->GetHeigh(x, z); });

  // Process the scene
  ProcessScene(XMLRoot);
}

void DotSceneLoaderB::WriteNode(pugi::xml_node &parentXML, const Ogre::SceneNode *node) {
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
      Write(diffuse, l->getDiffuseColour());
      auto specular = light.append_child("colourSpecular");
      Write(specular, l->getSpecularColour());
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

void DotSceneLoaderB::ExportScene(Ogre::SceneNode *rootNode, const string &outFileName) {
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
  //terrain.reset();
  // forest.reset();
  sinbad.reset();
  //  Pbr::Cleanup();
  if (ogre_scene) ogre_scene->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
  if (ogre_scene) ogre_scene->clearScene();
  if (camera_man) camera_man->SetStyle(CameraMan::Style::MANUAL);
  Ogre::ResourceGroupManager::getSingleton().unloadResourceGroup(group_name);
}

//float DotSceneLoaderB::GetHeight(float x, float z) { return terrain ? terrain->GetHeigh(x, z) : 0; }
float DotSceneLoaderB::GetHeight(float x, float z) { return 0; }

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

  Ogre::LogManager::getSingleton().logMessage(message);

  if (auto element = XmlRoot.child("environment")) {
    ProcessEnvironment(element);
  }

  if (auto element = XmlRoot.child("terrainGroup")) {
    ProcessLandscape(element);
  }

  if (auto element = XmlRoot.child("nodes")) {
    ProcessNodes(element);
  }

  if (auto element = XmlRoot.child("externals")) {
    ProcessExternals(element);
  }

  if (auto element = XmlRoot.child("forest")) {
    ProcessForest(element);
  }

  if (auto element = XmlRoot.child("userData")) {
    ProcessUserData(element, attach_node->getUserObjectBindings());
  }
}

void DotSceneLoaderB::ProcessNodes(pugi::xml_node &XmlNode) {
  // Process node (*)
  for (auto element : XmlNode.children("node")) {
    ProcessNode(element);
  }

  // Process position
  if (auto element = XmlNode.child("position")) {
    attach_node->setPosition(ParsePosition(element));
    attach_node->setInitialState();
  }

  // Process rotation
  if (auto element = XmlNode.child("rotation")) {
    attach_node->setOrientation(ParseRotation(element));
    attach_node->setInitialState();
  }

  // Process scale
  if (auto element = XmlNode.child("scale")) {
    attach_node->setScale(ParseScale(element));
    attach_node->setInitialState();
  }
}

void DotSceneLoaderB::ProcessExternals(pugi::xml_node &XmlNode) {
  //! @todo Implement this
}

void DotSceneLoaderB::ProcessEnvironment(pugi::xml_node &XmlNode) {
  // Process camera
  auto *viewport = Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default")->getViewport();

  // Process fog
  if (auto element = XmlNode.child("fog")) {
    ProcessFog(element);
  }

  // Process skyBox
  if (auto element = XmlNode.child("skyBox")) {
    ProcessSkyBox(element);
  }

  // Process skyDome
  if (auto element = XmlNode.child("skyDome")) {
    ProcessSkyDome(element);
  }

  // Process skyPlane
  if (auto element = XmlNode.child("skyPlane")) {
    ProcessSkyPlane(element);
  }

  // Process colourAmbient
  if (auto element = XmlNode.child("colourAmbient")) {
    ogre_scene->setAmbientLight(ParseColour(element));
  }

  if (auto element = XmlNode.child("shadowColor")) {
    ogre_scene->setShadowColour(ParseColour(element));
  }

  // Process colourBackground
  if (auto element = XmlNode.child("colourBackground")) {
    viewport->setBackgroundColour(ParseColour(element));
  }
}

void DotSceneLoaderB::ProcessLightRange(pugi::xml_node &XmlNode, Ogre::Light *light) {
  // Process attributes
  float inner = GetAttribReal(XmlNode, "inner");
  float outer = GetAttribReal(XmlNode, "outer");
  float falloff = GetAttribReal(XmlNode, "falloff", 1.0);

  // Setup the light range
  light->setSpotlightRange(Ogre::Radian(inner), Ogre::Radian(outer), falloff);
}

void DotSceneLoaderB::ProcessLightAttenuation(pugi::xml_node &XmlNode, Ogre::Light *light) {
  // Process attributes
  float range = GetAttribReal(XmlNode, "range");
  float constant = GetAttribReal(XmlNode, "constant");
  float linear = GetAttribReal(XmlNode, "linear");
  float quadratic = GetAttribReal(XmlNode, "quadratic");

  // Setup the light attenuation
  light->setAttenuation(range, constant, linear, quadratic);
}

void DotSceneLoaderB::ProcessLight(pugi::xml_node &XmlNode, Ogre::SceneNode *ParentNode) {
  // Process attributes
  string name = GetAttrib(XmlNode, "name");
  const size_t MAX_TEX_COUNT = 9;

  // SetUp the light
  Ogre::Light *light = ogre_scene->createLight(name);
  ParentNode->attachObject(light);

  string sValue = GetAttrib(XmlNode, "type");

  if (sValue == "point") {
    light->setType(Ogre::Light::LT_POINT);
  } else if (sValue == "directional") {
    light->setType(Ogre::Light::LT_DIRECTIONAL);
  } else if (sValue == "spot") {
    light->setType(Ogre::Light::LT_SPOTLIGHT);
  } else if (sValue == "radPoint") {
    light->setType(Ogre::Light::LT_POINT);
  }

  light->setVisible(GetAttribBool(XmlNode, "visible", true));
  light->setCastShadows(GetAttribBool(XmlNode, "castShadows", false));

  if (Ogre::Root::getSingleton().getSceneManager("Default")->getShadowTechnique() != Ogre::SHADOWTYPE_NONE) {
    auto texture_config = ogre_scene->getShadowTextureConfigList()[0];

    if (ogre_scene->getShadowTextureConfigList().size() < MAX_TEX_COUNT) {
      if (light->getType() == Ogre::Light::LT_POINT) {
        light->setCastShadows(false);
      } else if (light->getType() == Ogre::Light::LT_SPOTLIGHT && light->getCastShadows()) {
        int camera_type = 4;

        ConfPtr->Get("camera_type", camera_type);
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

  light->setPowerScale(GetAttribReal(XmlNode, "powerScale", 1.0));

  // Process colourDiffuse
  if (auto element = XmlNode.child("colourDiffuse")) {
    light->setDiffuseColour(ParseColour(element));
  }

  // Process colourSpecular
  if (auto element = XmlNode.child("colourSpecular")) {
    light->setSpecularColour(ParseColour(element));
  }

  if (light->getType() != Ogre::Light::LT_DIRECTIONAL) {
    // Process lightAttenuation
    if (auto element = XmlNode.child("lightAttenuation")) {
      ProcessLightAttenuation(element, light);
    }
  }

  if (light->getType() != Ogre::Light::LT_SPOTLIGHT) {
    // Process lightRange
    if (auto element = XmlNode.child("lightRange")) {
      ProcessLightRange(element, light);
    }
  }

  // Process userDataReference
  if (auto element = XmlNode.child("userData")) {
    ProcessUserData(element, light->getUserObjectBindings());
  }
}

void DotSceneLoaderB::ProcessCamera(pugi::xml_node &XmlNode, Ogre::SceneNode *ParentNode) {
  // Process attributes
  string name = GetAttrib(XmlNode, "name");
  float fov = GetAttribReal(XmlNode, "fov", 90);
  float aspectRatio = GetAttribReal(XmlNode, "aspectRatio", 0);
  string projectionType = GetAttrib(XmlNode, "projectionType", "perspective");

  // SetUp the camera
  auto *camera = Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default");

  if (!camera_man) camera_man = make_unique<CameraMan>();

  camera_man->AttachCamera(ParentNode, camera);

  if (camera_man->GetStyle() == CameraMan::Style::FPS) {
    auto *scene = Ogre::Root::getSingleton().getSceneManager("Default");
    auto *actor = scene->createEntity("Actor", "Icosphere.mesh");
    actor->setCastShadows(false);
    actor->setVisible(false);
    ParentNode->attachObject(actor);
    unique_ptr<BtOgre::StaticMeshToShapeConverter> converter;
    btVector3 inertia;
    btRigidBody *entBody;
    converter = make_unique<BtOgre::StaticMeshToShapeConverter>(actor);
    auto *entShape = converter->createCapsule();
    GetAudio().SetListener(camera->getParentSceneNode());
    float mass = 100.0f;
    entShape->calculateLocalInertia(mass, inertia);
    auto *bodyState = new BtOgre::RigidBodyState(ParentNode);
    entBody = new btRigidBody(mass, bodyState, entShape, inertia);
    entBody->setAngularFactor(0);
    entBody->activate(true);
    entBody->forceActivationState(DISABLE_DEACTIVATION);
    entBody->setActivationState(DISABLE_DEACTIVATION);
    entBody->setFriction(1.0);
    entBody->setUserIndex(1);
    GetPhysics().AddRigidBody(entBody);
    camera_man->SetRigidBody(entBody);
    camera_man->AttachNode(ParentNode);
  } else {
    sinbad = make_unique<SinbadCharacterController>(ogre_scene->getCamera("Default"));
    camera_man->AttachNode(ParentNode, sinbad->GetBodyNode());
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
  if (auto element = XmlNode.child("clipping")) {
    float nearDist = GetAttribReal(element, "near");
    camera->setNearClipDistance(nearDist);

    float farDist = GetAttribReal(element, "far");
    camera->setFarClipDistance(farDist);
  }

  // Process userDataReference
  if (auto element = XmlNode.child("userData")) {
    ProcessUserData(element, static_cast<Ogre::MovableObject *>(camera)->getUserObjectBindings());
  }
}

void DotSceneLoaderB::ProcessNode(pugi::xml_node &XmlNode, Ogre::SceneNode *ParentNode) {
  // Construct the node's name
  string name = GetAttrib(XmlNode, "name");

  // SetUp the scene node
  Ogre::SceneNode *node;

  if (name.empty()) {
    // Let Ogre choose the name
    if (ParentNode) {
      node = ParentNode->createChildSceneNode();
    } else {
      node = attach_node->createChildSceneNode();
    }
  } else {
    // Provide the name
    if (ParentNode) {
      node = ParentNode->createChildSceneNode(name);
    } else {
      node = attach_node->createChildSceneNode(name);
    }
  }

  // Process other attributes

  // Process position
  if (auto element = XmlNode.child("position")) {
    Ogre::Vector3 position = ParsePosition(element);
    if (!ParentNode) {
      position.y += GetHeight(position.x, position.z);
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
  if (auto element = XmlNode.child("lookTarget")) {
    ProcessLookTarget(element, node);
  }

  // Process trackTarget
  if (auto element = XmlNode.child("trackTarget")) {
    ProcessTrackTarget(element, node);
  }

  // Process node (*)
  for (auto element : XmlNode.children("node")) {
    ProcessNode(element, node);
  }

  // Process entity (*)
  for (auto element : XmlNode.children("entity")) {
    ProcessEntity(element, node);
  }

  // Process light (*)
  for (auto element : XmlNode.children("light")) {
    ProcessLight(element, node);
  }

  // Process particleSystem (*)
  for (auto element : XmlNode.children("particleSystem")) {
    ProcessParticleSystem(element, node);
  }

  // Process billboardSet (*)
  for (auto element : XmlNode.children("billboardSet")) {
    ProcessBillboardSet(element, node);
  }

  // Process plane (*)
  for (auto element : XmlNode.children("plane")) {
    ProcessPlane(element, node);
  }

  // Process camera (*)
  for (auto element : XmlNode.children("camera")) {
    ProcessCamera(element, node);
  }

  // Process userDataReference
  if (auto element = XmlNode.child("userData")) {
    ProcessUserData(element, node->getUserObjectBindings());
  }
}

void DotSceneLoaderB::ProcessLookTarget(pugi::xml_node &XmlNode, Ogre::SceneNode *ParentNode) {
  // Process attributes
  string nodeName = GetAttrib(XmlNode, "nodeName");

  Ogre::Node::TransformSpace relativeTo = Ogre::Node::TS_PARENT;
  string sValue = GetAttrib(XmlNode, "relativeTo");

  if (sValue == "local") {
    relativeTo = Ogre::Node::TS_LOCAL;
  } else if (sValue == "ParentNode") {
    relativeTo = Ogre::Node::TS_PARENT;
  } else if (sValue == "world") {
    relativeTo = Ogre::Node::TS_WORLD;
  }

  // Process position
  Ogre::Vector3 position;

  if (auto element = XmlNode.child("position")) {
    position = ParseVector3(element);
  }

  // Process localDirection
  Ogre::Vector3 localDirection = Ogre::Vector3::NEGATIVE_UNIT_Z;

  if (auto element = XmlNode.child("localDirection")) {
    localDirection = ParseVector3(element);
  }

  // Setup the look target
  try {
    if (!nodeName.empty()) {
      Ogre::SceneNode *pLookNode = ogre_scene->getSceneNode(nodeName);
      position = pLookNode->_getDerivedPosition();
    }

    ParentNode->lookAt(position, relativeTo, localDirection);
  } catch (Ogre::Exception &e) {
    Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error processing a look target!");
  }
}

void DotSceneLoaderB::ProcessTrackTarget(pugi::xml_node &XmlNode, Ogre::SceneNode *ParentNode) {
  // Process attributes
  string nodeName = GetAttrib(XmlNode, "nodeName");

  // Process localDirection
  Ogre::Vector3 localDirection = Ogre::Vector3::NEGATIVE_UNIT_Z;

  if (auto element = XmlNode.child("localDirection")) {
    localDirection = ParseVector3(element);
  }

  // Process offset
  Ogre::Vector3 offset = Ogre::Vector3::ZERO;

  if (auto element = XmlNode.child("offset")) {
    offset = ParseVector3(element);
  }

  // Setup the track target
  try {
    Ogre::SceneNode *pTrackNode = ogre_scene->getSceneNode(nodeName);
    ParentNode->setAutoTracking(true, pTrackNode, localDirection, offset);
  } catch (Ogre::Exception &e) {
    Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error processing a track target!");
  }
}

void DotSceneLoaderB::ProcessEntity(pugi::xml_node &XmlNode, Ogre::SceneNode *ParentNode) {
  // Process attributes
  string name = GetAttrib(XmlNode, "name");
  string id = GetAttrib(XmlNode, "id");
  string meshFile = GetAttrib(XmlNode, "meshFile");
  string material_name = GetAttrib(XmlNode, "material");
  bool cast_shadows = GetAttribBool(XmlNode, "castShadows", true);
  bool active_ibl = GetAttribBool(XmlNode, "activeIBL", false);
  bool planar_reflection = GetAttribBool(XmlNode, "planarReflection", false);

  // SetUp the entity
  Ogre::Entity *entity = ogre_scene->createEntity(name, meshFile);

  try {
    ParentNode->attachObject(entity);
    UpdateEntityMaterial(entity, cast_shadows, material_name, planar_reflection, active_ibl);

    // Process userDataReference
    if (auto element = XmlNode.child("userData")) {
      ProcessUserData(element, entity->getUserObjectBindings());
      GetPhysics().ProcessData(entity->getUserObjectBindings(), entity, ParentNode);
    } else {
      GetPhysics().ProcessData(entity, ParentNode);
    }
  } catch (Ogre::Exception &e) {
    Ogre::LogManager::getSingleton().logMessage(e.getFullDescription());
    Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error loading an entity!");
  }
}

void DotSceneLoaderB::ProcessParticleSystem(pugi::xml_node &XmlNode, Ogre::SceneNode *ParentNode) {
  // Process attributes
  string name = GetAttrib(XmlNode, "name");
  string templateName = GetAttrib(XmlNode, "template");
  bool attachedCamera = GetAttribBool(XmlNode, "attachedCamera", false);

  if (templateName.empty()) {
    templateName = GetAttrib(XmlNode, "file");  // compatibility with old scenes
  }

  // SetUp the particle system
  try {
    Ogre::ParticleSystem *pParticles = ogre_scene->createParticleSystem(name, templateName);
    PBR::UpdatePbrParams(pParticles->getMaterialName());

    const Ogre::uint32 WATER_MASK = 0xF00;
    pParticles->setVisibilityFlags(WATER_MASK);

    if (attachedCamera) {
      auto *camera = Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default");
      camera->getParentSceneNode()->createChildSceneNode(Ogre::Vector3{0, 10, 0})->attachObject(pParticles);
    } else {
      ParentNode->attachObject(pParticles);
    }
  } catch (Ogre::Exception &e) {
    Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error creating a particle system!");
  }
}

void DotSceneLoaderB::ProcessBillboardSet(pugi::xml_node &XmlNode, Ogre::SceneNode *ParentNode) {
  //! @todo Implement this
}

void DotSceneLoaderB::ProcessPlane(pugi::xml_node &XmlNode, Ogre::SceneNode *ParentNode) {
  string name = GetAttrib(XmlNode, "name");
  float distance = GetAttribReal(XmlNode, "distance", 0.0f);
  float width = GetAttribReal(XmlNode, "width", 1.0f);
  float height = GetAttribReal(XmlNode, "height", width);
  int xSegments = ToInt(GetAttrib(XmlNode, "xSegments"), width / 5.0f);
  int ySegments = ToInt(GetAttrib(XmlNode, "ySegments"), height / 5.0f);
  int numTexCoordSets = ToInt(GetAttrib(XmlNode, "numTexCoordSets"), 1);
  float uTile = GetAttribReal(XmlNode, "uTile", width / 10.0f);
  float vTile = GetAttribReal(XmlNode, "vTile", height / 10.0f);
  string material = GetAttrib(XmlNode, "material", "BaseWhite");
  bool hasNormals = GetAttribBool(XmlNode, "hasNormals", true);
  Ogre::Vector3 normal = ParseVector3(XmlNode.child("normal"));
  Ogre::Vector3 up = ParseVector3(XmlNode.child("upVector"));
  bool reflection = GetAttribBool(XmlNode, "reflection", false);

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
    PBR::UpdatePbrParams(material);
    if (material_ptr->getReceiveShadows()) PBR::UpdatePbrShadowReceiver(material);
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

  ParentNode->attachObject(entity);

  unique_ptr<BtOgre::StaticMeshToShapeConverter> converter = make_unique<BtOgre::StaticMeshToShapeConverter>(entity);

  auto *entShape = converter->createTrimesh();
  auto *bodyState = new BtOgre::RigidBodyState(ParentNode);
  auto *entBody = new btRigidBody(0, bodyState, entShape, btVector3(0, 0, 0));
  entBody->setFriction(1);
  GetPhysics().AddRigidBody(entBody);

  const Ogre::uint32 WATER_MASK = 0xF00;
  entity->setVisibilityFlags(WATER_MASK);
}

void DotSceneLoaderB::ProcessForest(pugi::xml_node &XmlNode) {
//  forest->ProcessForest();
}

void DotSceneLoaderB::ProcessLandscape(pugi::xml_node &XmlNode) {
  //if (terrain) terrain->ProcessTerrainGroup(XmlNode);
}

void DotSceneLoaderB::ProcessFog(pugi::xml_node &XmlNode) {
  // Process attributes
  float expDensity = GetAttribReal(XmlNode, "density", 0.001);
  float linearStart = GetAttribReal(XmlNode, "start", 0.0);
  float linearEnd = GetAttribReal(XmlNode, "end", 1.0);
  Ogre::ColourValue colour = Ogre::ColourValue::White;

  Ogre::FogMode mode = Ogre::FOG_NONE;
  string sMode = GetAttrib(XmlNode, "mode");

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

  if (auto element = XmlNode.child("colour")) {
    colour = ParseColour(element);
  }

  // Setup the fog
  ogre_scene->setFog(mode, colour, expDensity, linearStart, linearEnd);
}

void DotSceneLoaderB::ProcessSkyBox(pugi::xml_node &XmlNode) {
  // Process attributes
  string material = GetAttrib(XmlNode, "material", "SkyBox");
  string cubemap = GetAttrib(XmlNode, "cubemap", "OutputCube.dds");
  float distance = GetAttribReal(XmlNode, "distance", 500);
  bool drawFirst = GetAttribBool(XmlNode, "drawFirst", true);
  bool active = GetAttribBool(XmlNode, "active", true);

  if (!active) {
    return;
  }

  // Process rotation
  Ogre::Quaternion rotation = Ogre::Quaternion::IDENTITY;

  if (auto element = XmlNode.child("rotation")) {
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

void DotSceneLoaderB::ProcessSkyDome(pugi::xml_node &XmlNode) {
  // Process attributes
  string material = XmlNode.attribute("material").value();
  float curvature = GetAttribReal(XmlNode, "curvature", 10);
  float tiling = GetAttribReal(XmlNode, "tiling", 8);
  float distance = GetAttribReal(XmlNode, "distance", 500);
  bool drawFirst = GetAttribBool(XmlNode, "drawFirst", true);
  bool active = GetAttribBool(XmlNode, "active", false);

  if (!active) {
    return;
  }

  // Process rotation
  Ogre::Quaternion rotation = Ogre::Quaternion::IDENTITY;

  if (auto element = XmlNode.child("rotation")) {
    rotation = ParseRotation(element);
  }

  // Setup the sky dome
  ogre_scene->setSkyDome(true, material, curvature, tiling, distance, drawFirst, rotation, 16, 16, -1, group_name);
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
  Ogre::Plane plane;
  plane.normal = Ogre::Vector3(planeX, planeY, planeZ);
  plane.d = planeD;
  ogre_scene->setSkyPlane(true, plane, material, scale, tiling, drawFirst, bow, 1, 1, group_name);
}

void DotSceneLoaderB::ProcessUserData(pugi::xml_node &XmlNode, Ogre::UserObjectBindings &user_object_bindings) {
  // Process node (*)
  for (auto element : XmlNode.children("property")) {
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
    GetLoader().Load(_stream, ResourceGroupManager::getSingleton().getWorldResourceGroupName(),
                     any_cast<SceneNode *>(output));
  }

  void encodeToFile(const Ogre::Any &input, const Ogre::String &outFileName) const override {
    DotSceneLoaderB loader;
    loader.ExportScene(Ogre::any_cast<Ogre::SceneNode *>(input), outFileName);
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

}  // namespace Glue
