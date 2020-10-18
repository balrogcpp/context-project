//MIT License
//
//Copyright (c) 2020 Andrey Vasiliev
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#include "pcheader.h"
#include "DotSceneLoaderB.h"
#include "Physics.h"
#include "ShaderUtils.h"
#include "XmlUtils.h"
#include "Sound.h"

using namespace xio;

namespace xio {
///---------------------------------------------------------------------------------------------------------------------
DotSceneLoaderB::DotSceneLoaderB() {
  if (Ogre::SceneLoaderManager::getSingleton()._getSceneLoader("DotScene"))
    Ogre::SceneLoaderManager::getSingleton().unregisterSceneLoader("DotScene");

  if (Ogre::SceneLoaderManager::getSingleton()._getSceneLoader("DotSceneB"))
    Ogre::SceneLoaderManager::getSingleton().unregisterSceneLoader("DotSceneB");

  Ogre::SceneLoaderManager::getSingleton().registerSceneLoader("DotSceneB", {".scene", ".xml"}, this);
}
///---------------------------------------------------------------------------------------------------------------------
DotSceneLoaderB::~DotSceneLoaderB() {}
///---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::Create() {
  if (!terrain_) terrain_ = std::make_unique<Landscape>();
  if (!forest_) forest_ = std::make_unique<Forest>();
  forest_->SetHeighFunc([](float x, float z) { return terrain_->GetHeigh(x, z); });
}
///---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::Clean() {
  terrain_.reset();
  forest_.reset();
  scene_->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
  scene_->clearScene();
}
///---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::Loop(float time) {
  camera_->Update(time);
}
///---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::load(Ogre::DataStreamPtr &stream, const std::string &group_name, Ogre::SceneNode *root_node) {
  group_name_ = group_name;
  scene_ = root_node->getCreator();

  pugi::xml_document XMLDoc; /// character type defaults to char

  auto result = XMLDoc.load_buffer(stream->getAsString().c_str(), stream->size());

  if (!result) {
    Ogre::LogManager::getSingleton().stream(Ogre::LML_CRITICAL) << "[DotSceneLoader] " << result.description();
    return;
  }

  /// Grab the scene node
  auto XMLRoot = XMLDoc.child("scene");

  /// Validate the File
  if (!XMLRoot.attribute("formatVersion")) {
    Ogre::LogManager::getSingleton().logError("[DotSceneLoader] Invalid .scene File. Missing <scene formatVersion='x.y' >");
    return;
  }

  /// figure out where to attach any nodes we Create
  attach_node_ = root_node;
  root_ = Ogre::Root::getSingletonPtr();
  root_node_ = attach_node_;

  Create();

  /// Process the scene
  ProcessScene_(XMLRoot);
}
///---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::Load(const std::string &filename, const std::string &group_name, Ogre::SceneNode *root_node) {
  group_name_ = group_name;
  scene_ = root_node->getCreator();

  pugi::xml_document XMLDoc; /// character type defaults to char

  auto result = XMLDoc.load_file(filename.c_str());

  if (!result) {
    Ogre::LogManager::getSingleton().stream(Ogre::LML_CRITICAL) << "[DotSceneLoader] " << result.description();
    return;
  }

  /// Grab the scene node
  auto XMLRoot = XMLDoc.child("scene");

  /// Validate the File
  if (!XMLRoot.attribute("formatVersion")) {
    Ogre::LogManager::getSingleton().logError(
        "[DotSceneLoader] Invalid .scene File. Missing <scene formatVersion='x.y' >");
    return;
  }

  /// figure out where to attach any nodes we Create
  attach_node_ = root_node;

  Create();

  /// Process the scene
  ProcessScene_(XMLRoot);
}
///---------------------------------------------------------------------------------------------------------------------
float DotSceneLoaderB::GetHeigh(float x, float z) {
  if (terrain_)
    return terrain_->GetHeigh(x, z);
  else
    return 0.0;
};
///---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessScene_(pugi::xml_node &xml_root) {
  /// Process the scene parameters
  std::string version = GetAttrib(xml_root, "formatVersion", "unknown");

  std::string message = "[DotSceneLoader] Parsing dotScene file with version " + version;

  if (xml_root.attribute("sceneManager"))
    message += ", scene manager " + std::string(xml_root.attribute("sceneManager").value());

  if (xml_root.attribute("minOgreVersion"))
    message += ", min. Ogre version " + std::string(xml_root.attribute("minOgreVersion").value());

  if (xml_root.attribute("author"))
    message += ", author " + std::string(xml_root.attribute("author").value());

  Ogre::LogManager::getSingleton().logMessage(message);

  if (auto element = xml_root.child("light"))
    ProcessLight_(element);

  if (auto element = xml_root.child("environment"))
    ProcessEnvironment_(element);

  if (auto element = xml_root.child("terrainGroup"))
    ProcessLandscape_(element);

  if (auto element = xml_root.child("nodes"))
    ProcessNodes_(element);

  if (auto element = xml_root.child("externals"))
    ProcessExternals_(element);

  if (auto element = xml_root.child("forest"))
    ProcessForest_(element);

  if (auto element = xml_root.child("userData"))
    ProcessUserData_(element, attach_node_->getUserObjectBindings());
}
///---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessNodes_(pugi::xml_node &xml_node) {
  /// Process node (*)
  for (auto element : xml_node.children("node")) {
    ProcessNode_(element);
  }

  /// Process position
  if (auto element = xml_node.child("position")) {
    attach_node_->setPosition(ParsePosition(element));
    attach_node_->setInitialState();
  }

  /// Process rotation
  if (auto element = xml_node.child("rotation")) {
    attach_node_->setOrientation(ParseQuaternion(element));
    attach_node_->setInitialState();
  }

  /// Process scale
  if (auto element = xml_node.child("scale")) {
    attach_node_->setScale(ParseScale(element));
    attach_node_->setInitialState();
  }
}
///---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessExternals_(pugi::xml_node &xml_node) {
  ///! @todo Implement this
}
///---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessEnvironment_(pugi::xml_node &xml_node) {
  /// Process camera
  auto *viewport = Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default")->getViewport();

  /// Process fog
  if (auto element = xml_node.child("fog"))
    ProcessFog_(element);

  /// Process skyBox
  if (auto element = xml_node.child("skyBox"))
    ProcessSkyBox_(element);

  /// Process skyDome
  if (auto element = xml_node.child("skyDome"))
    ProcessSkyDome_(element);

  /// Process skyPlane
  if (auto element = xml_node.child("skyPlane"))
    ProcessSkyPlane_(element);

  /// Process colourAmbient
  if (auto element = xml_node.child("colourAmbient"))
    scene_->setAmbientLight(ParseColour(element));

  if (auto element = xml_node.child("shadowColor"))
    scene_->setShadowColour(ParseColour(element));

  /// Process colourBackground
  if (auto element = xml_node.child("colourBackground"))
    viewport->setBackgroundColour(ParseColour(element));
}
///---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessLight_(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  /// Process attributes
  static long counter = 0;
  auto *scene = Ogre::Root::getSingleton().getSceneManager("Default");
  std::string name = GetAttrib(xml_node, "name");

  if (!parent) {
    parent = scene->createSceneNode();
  }

  /// Create the light
  name += std::to_string(counter);
  counter++;
  Ogre::Light *light = scene_->createLight(name);

  if (parent) {
    parent->attachObject(light);
  }

  std::string sValue = GetAttrib(xml_node, "type");

  if (sValue == "point") {
    light->setType(Ogre::Light::LT_POINT);
  } else if (sValue == "directional") {
    light->setType(Ogre::Light::LT_DIRECTIONAL);
  } else if (sValue == "spot") {
    light->setType(Ogre::Light::LT_SPOTLIGHT);
  } else if (sValue == "radPoint") {
    light->setType(Ogre::Light::LT_POINT);
  }

  /// lights are oriented using SceneNodes that expect -Z to be the default direction
  /// exporters should not write normal or direction if they attach lights to nodes
  if (auto element = xml_node.child("rotation")) {
    parent->rotate(ParseQuaternion(element));
  }

  light->setVisible(GetAttribBool(xml_node, "visible", true));
  light->setCastShadows(GetAttribBool(xml_node, "castShadows", false));
  light->setPowerScale(GetAttribReal(xml_node, "powerScale", 1.0));

  /// Process colourDiffuse
  if (auto element = xml_node.child("colourDiffuse")) {
    light->setDiffuseColour(ParseColour(element));
  }

  /// Process colourSpecular
  if (auto element = xml_node.child("colourSpecular")) {
    light->setSpecularColour(ParseColour(element));
  }

  if (sValue != "directional") {
    /// Process lightRange
    if (auto element = xml_node.child("lightRange")) {
      ProcessLightRange_(element, light);
    }

    /// Process lightAttenuation
    if (auto element = xml_node.child("lightAttenuation")) {
      ProcessLightAttenuation_(element, light);
    }
  }

  /// Process userDataReference
  if (auto element = xml_node.child("userData")) {
    ProcessUserData_(element, light->getUserObjectBindings());
  }
}
///---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessCamera_(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  /// Process attributes
  std::string name = GetAttrib(xml_node, "name");
  float fov = GetAttribReal(xml_node, "fov", 45);
  float aspectRatio = GetAttribReal(xml_node, "aspectRatio", 0);
  std::string projectionType = GetAttrib(xml_node, "projectionType", "perspective");
  parent->translate(Ogre::Vector3(0, 1, 0));

  /// Create the camera
  auto *pCamera = Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default");

  if (!camera_) {
    camera_ = std::make_unique<Camera>();
    input_->RegObserver(camera_.get());
  }

  camera_->SetStyle(Camera::FPS);
  camera_->RegCamera(parent, pCamera);

  auto *scene = Ogre::Root::getSingleton().getSceneManager("Default");
  auto *actor = scene->createEntity("Actor", "Icosphere.mesh");
  actor->setCastShadows(false);
  actor->setVisible(false);
  parent->attachObject(actor);
  std::unique_ptr<BtOgre::StaticMeshToShapeConverter> converter;
  btVector3 inertia;
  btRigidBody *entBody;
  converter = std::make_unique<BtOgre::StaticMeshToShapeConverter>(actor);
  auto *entShape = converter->createCapsule();
  sound_->SetListener(pCamera->getParentSceneNode());
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
  physics_->AddRigidBody(entBody);
  camera_->SetRigidBody(entBody);

  /// Set the field-of-view
  pCamera->setFOVy(Ogre::Radian(fov));

  /// Set the aspect ratio
  if (aspectRatio != 0)
    pCamera->setAspectRatio(aspectRatio);

  /// Set the projection type
  if (projectionType == "perspective") {
    pCamera->setProjectionType(Ogre::PT_PERSPECTIVE);
  } else if (projectionType == "orthographic") {
    pCamera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
  }

  /// Process clipping
  if (auto element = xml_node.child("clipping")) {
    float nearDist = GetAttribReal(element, "near");
    pCamera->setNearClipDistance(nearDist);

    float farDist = GetAttribReal(element, "far");
    pCamera->setFarClipDistance(farDist);
  }

  /// Process userDataReference
  if (auto element = xml_node.child("userData")) {
    ProcessUserData_(element, static_cast<Ogre::MovableObject *>(pCamera)->getUserObjectBindings());
  }
}
///---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessNode_(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  /// Construct the node's name
  static long counter = 0;
  std::string name = GetAttrib(xml_node, "name");

  /// Create the scene node
  Ogre::SceneNode *node;

  if (name.empty()) {
    /// Let Ogre choose the name
    if (parent) {
      node = parent->createChildSceneNode();
    } else {
      node = attach_node_->createChildSceneNode();
    }
  } else {
    /// Provide the name
    name += std::to_string(counter);
    counter++;
    if (parent) {
      node = parent->createChildSceneNode(name);
    } else {
      node = attach_node_->createChildSceneNode(name);
    }
  }

  /// Process other attributes

  /// Process position
  if (auto element = xml_node.child("position")) {
    Ogre::Vector3 position = ParsePosition(element);
    position.y += GetHeigh(position.x, position.z);
    node->setPosition(position);
    node->setInitialState();
  }

  /// Process rotation
  if (auto element = xml_node.child("rotation")) {
    node->setOrientation(ParseQuaternion(element));
    node->setInitialState();
  }

  /// Process rotation
  if (auto element = xml_node.child("direction")) {
    node->setDirection(ParsePosition(element).normalisedCopy());
    node->setInitialState();
  }

  /// Process scale
  if (auto element = xml_node.child("scale")) {
    node->setScale(ParseScale(element));
    node->setInitialState();
  }

  /// Process lookTarget
  if (auto element = xml_node.child("lookTarget"))
    ProcessLookTarget_(element, node);

  /// Process trackTarget
  if (auto element = xml_node.child("trackTarget"))
    ProcessTrackTarget_(element, node);

  /// Process camera (*)
  for (auto element : xml_node.children("camera")) {
    ProcessCamera_(element, node);
  }

  /// Process node (*)
  for (auto element : xml_node.children("node")) {
    ProcessNode_(element, node);
  }

  /// Process entity (*)
  for (auto element : xml_node.children("entity")) {
    ProcessEntity_(element, node);
  }

  /// Process light (*)
  for (auto element : xml_node.children("light")) {
    ProcessLight_(element, node);
  }

  /// Process particleSystem (*)
  for (auto element : xml_node.children("particleSystem")) {
    ProcessParticleSystem_(element, node);
  }

  /// Process billboardSet (*)
  for (auto element : xml_node.children("billboardSet")) {
    ProcessBillboardSet_(element, node);
  }

  /// Process plane (*)
  for (auto element : xml_node.children("plane")) {
    ProcessPlane_(element, node);
  }

  /// Process userDataReference
  if (auto element = xml_node.child("userData")) {
    ProcessUserData_(element, node->getUserObjectBindings());
  }
}
///---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessLookTarget_(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  ///! @todo Is this correct? Cause I don't have a clue actually

  /// Process attributes
  std::string nodeName = GetAttrib(xml_node, "nodeName");

  Ogre::Node::TransformSpace relativeTo = Ogre::Node::TS_PARENT;
  std::string sValue = GetAttrib(xml_node, "relativeTo");

  if (sValue == "local") {
    relativeTo = Ogre::Node::TS_LOCAL;
  } else if (sValue == "parent") {
    relativeTo = Ogre::Node::TS_PARENT;
  } else if (sValue == "world") {
    relativeTo = Ogre::Node::TS_WORLD;
  }

  /// Process position
  Ogre::Vector3 position;

  if (auto element = xml_node.child("position")) {
    position = ParseVector3(element);
  }

  /// Process localDirection
  Ogre::Vector3 localDirection = Ogre::Vector3::NEGATIVE_UNIT_Z;

  if (auto element = xml_node.child("localDirection")) {
    localDirection = ParseVector3(element);
  }

  /// Setup the look target
  try {
    if (!nodeName.empty()) {
      Ogre::SceneNode *pLookNode = scene_->getSceneNode(nodeName);
      position = pLookNode->_getDerivedPosition();
    }

    parent->lookAt(position, relativeTo, localDirection);
  }
  catch (Ogre::Exception &e) {
    Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error processing a look target!");
  }
}
///---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessTrackTarget_(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  /// Process attributes
  std::string nodeName = GetAttrib(xml_node, "nodeName");

  /// Process localDirection
  Ogre::Vector3 localDirection = Ogre::Vector3::NEGATIVE_UNIT_Z;

  if (auto element = xml_node.child("localDirection"))
    localDirection = ParseVector3(element);

  /// Process offset
  Ogre::Vector3 offset = Ogre::Vector3::ZERO;

  if (auto element = xml_node.child("offset"))
    offset = ParseVector3(element);

  /// Setup the track target
  try {
    Ogre::SceneNode *pTrackNode = scene_->getSceneNode(nodeName);
    parent->setAutoTracking(true, pTrackNode, localDirection, offset);
  }
  catch (Ogre::Exception &e) {
    Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error processing a track target!");
  }
}
///---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessEntity_(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  /// Process attributes
  static long long counter = 0;
  std::string name = GetAttrib(xml_node, "name");
  std::string id = GetAttrib(xml_node, "id");
  std::string meshFile = GetAttrib(xml_node, "meshFile");
  std::string material = GetAttrib(xml_node, "material");
  bool castShadows = GetAttribBool(xml_node, "castShadows", true);

  /// Create the entity
  Ogre::Entity *entity = nullptr;
  try {
    name += std::to_string(counter);
    counter++;
    entity = scene_->createEntity(name, meshFile);
    entity->setCastShadows(castShadows);
    parent->attachObject(entity);

    auto mesh = entity->getMesh();

    std::string shadow_technique = "texture";

    if (shadow_technique == "stencil") {
      if (!entity->getMesh()->isEdgeListBuilt()) {
        entity->getMesh()->buildEdgeList();
      }

      for (int j = 0; j < entity->getNumManualLodLevels(); j++) {
        auto *lod = entity->getManualLodLevel(j);
        EnsureHasTangents(lod->getMesh());
        if (shadow_technique == "stencil") {
          if (!lod->getMesh()->isEdgeListBuilt()) {
            lod->getMesh()->buildEdgeList();
          }
        }
      }
    }

    EnsureHasTangents(entity->getMesh());
    for (auto &submesh : mesh->getSubMeshes()) {
      Ogre::MaterialPtr material_ptr;

      if (!material.empty()) {
        entity->setMaterialName(material);
        material_ptr = Ogre::MaterialManager::getSingleton().getByName(material);
      } else if (submesh->getMaterial()) {
        material_ptr = submesh->getMaterial();
      }

      if (material_ptr) {
        UpdatePbrParams(material_ptr);

        if (entity->getCastShadows())
          UpdatePbrShadowCaster(material_ptr);

        if (material_ptr->getReceiveShadows())
          UpdatePbrShadowReceiver(material_ptr);

        auto ibl_texture = material_ptr->getTechnique(0)->getPass(0)->getTextureUnitState("IBL_Specular");
        auto ibl_texture2 = material_ptr->getTechnique(0)->getPass(0)->getTextureUnitState("IBL_Diffuse");
        if (ibl_texture || ibl_texture2) {
          if (!ccamera_) {
            auto *root = root_node_->createChildSceneNode(Ogre::Vector3{0, GetHeigh(0, 0) + 1, 0});
            ccamera_ = std::make_unique<CubeMapCamera>(root, 256);
          }

          UpdatePbrIbl(material_ptr);
        }
      }
    }
    /// Process userDataReference
    if (auto element = xml_node.child("userData")) {
      ProcessUserData_(element, entity->getUserObjectBindings());
      physics_->ProcessData(entity->getUserObjectBindings(), entity, parent);
    }
  }
  catch (Ogre::Exception &e) {
    Ogre::LogManager::getSingleton().logMessage(e.getFullDescription());
    Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error loading an entity!");
  }
}///---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessParticleSystem_(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  /// Process attributes
  std::string name = GetAttrib(xml_node, "name");
  std::string templateName = GetAttrib(xml_node, "template");
  bool attachedCamera = GetAttribBool(xml_node, "attachedCamera", false);

  if (templateName.empty()) {
    templateName = GetAttrib(xml_node, "file"); /// compatibility with old scenes
  }

  /// Create the particle system
  try {
    Ogre::ParticleSystem *pParticles = scene_->createParticleSystem(name, templateName);
    UpdatePbrParams(pParticles->getMaterialName());

    const Ogre::uint32 WATER_MASK = 0xF00;
    pParticles->setVisibilityFlags(WATER_MASK);

    if (attachedCamera) {
      auto *camera = Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default");
      camera->getParentSceneNode()->createChildSceneNode(Ogre::Vector3{0, 10, 0})->attachObject(pParticles);
    } else {
      parent->attachObject(pParticles);
    }
  }
  catch (Ogre::Exception &e) {
    Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error creating a particle system!");
  }
}
///---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessBillboardSet_(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  ///! @todo Implement this
}
///---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessPlane_(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  std::string name = GetAttrib(xml_node, "name");
  float distance = GetAttribReal(xml_node, "distance", 0.0f);
  float width = GetAttribReal(xml_node, "width", 1.0f);
  float height = GetAttribReal(xml_node, "height", width);
  int xSegments = Ogre::StringConverter::parseInt(GetAttrib(xml_node, "xSegments"), 10);
  int ySegments = Ogre::StringConverter::parseInt(GetAttrib(xml_node, "ySegments"), 10);
  int numTexCoordSets = Ogre::StringConverter::parseInt(GetAttrib(xml_node, "numTexCoordSets"), 1);
  float uTile = GetAttribReal(xml_node, "uTile", width / 5.0f);
  float vTile = GetAttribReal(xml_node, "vTile", height / 5.0f);
  std::string material = GetAttrib(xml_node, "material", "BaseWhite");
  bool hasNormals = GetAttribBool(xml_node, "hasNormals", true);
  Ogre::Vector3 normal = ParseVector3(xml_node.child("normal"));
  Ogre::Vector3 up = ParseVector3(xml_node.child("upVector"));
  bool reflection = GetAttribBool(xml_node, "reflection", false);

  normal = {0, 1, 0};
  up = {0, 0, 1};
  Ogre::Plane plane(normal, distance);

  rcamera_.reset();
  rcamera_ = std::make_unique<ReflectionCamera>(plane, 1024);

  std::string mesh_name = name + "mesh";

  Ogre::MeshPtr plane_mesh = Ogre::MeshManager::getSingleton().getByName(mesh_name,
                                                                         Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
  if (plane_mesh)
    Ogre::MeshManager::getSingleton().remove(plane_mesh);
  Ogre::MeshPtr res =
      Ogre::MeshManager::getSingletonPtr()->createPlane(mesh_name, group_name_, plane, width, height, xSegments,
                                                        ySegments, hasNormals, numTexCoordSets, uTile, vTile, up);
  res->buildTangentVectors();
  Ogre::Entity *entity = scene_->createEntity(name, mesh_name);
  entity->setCastShadows(false);

  if (material.empty())
    return;

  if (!material.empty()) {
    entity->setMaterialName(material);
    Ogre::MaterialPtr material_ptr = Ogre::MaterialManager::getSingleton().getByName(material);
    UpdatePbrParams(material);
    if (material_ptr->getReceiveShadows())
      UpdatePbrShadowReceiver(material);
//    UpdatePbrShadowCaster(material);
  }

  if (reflection) {
    Ogre::MaterialPtr material_ptr = Ogre::MaterialManager::getSingleton().getByName(material);

    auto material_unit = material_ptr->getTechnique(0)->getPass(0)->getTextureUnitState("ReflectionMap");

    if (material_unit) {
      material_unit->setTexture(rcamera_->GetReflectionTex());
      material_unit->setTextureAddressingMode(Ogre::TAM_CLAMP);
      material_unit->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_POINT);
    }
  }

  rcamera_->SetPlane(plane);
  parent->attachObject(entity);

  std::unique_ptr<BtOgre::StaticMeshToShapeConverter>
      converter = std::make_unique<BtOgre::StaticMeshToShapeConverter>(entity);
///  auto *entShape = converter->createTrimesh();
  auto *entShape = converter->createBox();
  auto *bodyState = new BtOgre::RigidBodyState(parent);
  btRigidBody *entBody = new btRigidBody(0, bodyState, entShape, btVector3(0, 0, 0));
  entBody->setFriction(1);
  physics_->AddRigidBody(entBody);

  const Ogre::uint32 WATER_MASK = 0xF00;
//  entity->setVisibilityFlags(WATER_MASK);
}
///---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessForest_(pugi::xml_node &xml_node) {
  forest_->ProcessForest();
}
///---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessLandscape_(pugi::xml_node &xml_node) {
  if (terrain_) terrain_->ProcessTerrainGroup(xml_node);
}
///---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessFog_(pugi::xml_node &xml_node) {
  /// Process attributes
  float expDensity = GetAttribReal(xml_node, "density", 0.001);
  float linearStart = GetAttribReal(xml_node, "start", 0.0);
  float linearEnd = GetAttribReal(xml_node, "end", 1.0);
  Ogre::ColourValue colour = Ogre::ColourValue::White;

  Ogre::FogMode mode = Ogre::FOG_NONE;
  std::string sMode = GetAttrib(xml_node, "mode");

  if (sMode == "none") {
    mode = Ogre::FOG_NONE;
  } else if (sMode == "exp") {
    mode = Ogre::FOG_EXP;
  } else if (sMode == "exp2") {
    mode = Ogre::FOG_EXP2;
  } else if (sMode == "linear") {
    mode = Ogre::FOG_LINEAR;
  } else {
    mode = (Ogre::FogMode) Ogre::StringConverter::parseInt(sMode);
  }

  if (auto element = xml_node.child("colour")) {
    colour = ParseColour(element);
  }

  /// Setup the fog
  scene_->setFog(mode, colour, expDensity, linearStart, linearEnd);
}
///---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessSkyBox_(pugi::xml_node &xml_node) {
  /// Process attributes
  std::string material = GetAttrib(xml_node, "material", "Skybox");
  std::string cubemap = GetAttrib(xml_node, "cubemap", "OutputCube.dds");
  float distance = GetAttribReal(xml_node, "distance", 500);
  bool drawFirst = GetAttribBool(xml_node, "drawFirst", true);
  bool active = GetAttribBool(xml_node, "active", true);

  if (!active) {
    return;
  }

  /// Process rotation
  Ogre::Quaternion rotation = Ogre::Quaternion::IDENTITY;

  if (auto element = xml_node.child("rotation")) {
    rotation = ParseQuaternion(element);
  }

  Ogre::MaterialPtr material_ptr = Ogre::MaterialManager::getSingleton().getByName(material);

  if (material_ptr->getTechnique(0)->getPass(0)->getNumTextureUnitStates() > 0) {
    auto texture_unit = material_ptr->getTechnique(0)->getPass(0)->getTextureUnitState(0);

    if (texture_unit) {
      texture_unit->setTextureName(cubemap);
    }
  }

  /// Setup the sky box
  scene_->setSkyBox(true, material, distance, drawFirst, rotation, group_name_);
}
///---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessSkyDome_(pugi::xml_node &xml_node) {
  /// Process attributes
  std::string material = xml_node.attribute("material").value();
  float curvature = GetAttribReal(xml_node, "curvature", 10);
  float tiling = GetAttribReal(xml_node, "tiling", 8);
  float distance = GetAttribReal(xml_node, "distance", 500);
  bool drawFirst = GetAttribBool(xml_node, "drawFirst", true);
  bool active = GetAttribBool(xml_node, "active", false);

  if (!active) {
    return;
  }

  /// Process rotation
  Ogre::Quaternion rotation = Ogre::Quaternion::IDENTITY;

  if (auto element = xml_node.child("rotation")) {
    rotation = ParseQuaternion(element);
  }

  /// Setup the sky dome
  scene_->setSkyDome(true,
                     material,
                     curvature,
                     tiling,
                     distance,
                     drawFirst,
                     rotation,
                     16,
                     16,
                     -1,
                     group_name_);
}
///---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessSkyPlane_(pugi::xml_node &xml_node) {
  /// Process attributes
  std::string material = GetAttrib(xml_node, "material");
  float planeX = GetAttribReal(xml_node, "planeX", 0);
  float planeY = GetAttribReal(xml_node, "planeY", -1);
  float planeZ = GetAttribReal(xml_node, "planeZ", 0);
  float planeD = GetAttribReal(xml_node, "planeD", 5000);
  float scale = GetAttribReal(xml_node, "scale", 1000);
  float bow = GetAttribReal(xml_node, "bow", 0);
  float tiling = GetAttribReal(xml_node, "tiling", 10);
  bool drawFirst = GetAttribBool(xml_node, "drawFirst", true);

  /// Setup the sky plane
  Ogre::Plane plane;
  plane.normal = Ogre::Vector3(planeX, planeY, planeZ);
  plane.d = planeD;
  scene_->setSkyPlane(true, plane, material, scale, tiling, drawFirst, bow, 1, 1, group_name_);
}
///---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessLightRange_(pugi::xml_node &xml_node, Ogre::Light *light) {
  /// Process attributes
  float inner = GetAttribReal(xml_node, "inner");
  float outer = GetAttribReal(xml_node, "outer");
  float falloff = GetAttribReal(xml_node, "falloff", 1.0);

  /// Setup the light range
  light->setSpotlightRange(Ogre::Angle(inner), Ogre::Angle(outer), falloff);
}
///---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessLightAttenuation_(pugi::xml_node &xml_node, Ogre::Light *light) {
  /// Process attributes
  float range = GetAttribReal(xml_node, "range");
  float constant = GetAttribReal(xml_node, "constant");
  float linear = GetAttribReal(xml_node, "linear");
  float quadratic = GetAttribReal(xml_node, "quadratic");

  /// Setup the light attenuation
  light->setAttenuation(range, constant, linear, quadratic);
}
///---------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessUserData_(pugi::xml_node &xml_node, Ogre::UserObjectBindings &user_object_bindings) {
  /// Process node (*)
  for (auto element : xml_node.children("property")) {
    std::string name = GetAttrib(element, "name");
    std::string type = GetAttrib(element, "type");
    std::string data = GetAttrib(element, "data");

    Ogre::Any value;

    if (type == "bool") {
      value = Ogre::StringConverter::parseBool(data);
    } else if (type == "float") {
      value = Ogre::StringConverter::parseReal(data);
    } else if (type == "int") {
      value = Ogre::StringConverter::parseInt(data);
    } else {
      value = data;
    }

    user_object_bindings.setUserAny(name, value);
  }
}
}