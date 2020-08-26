/*
MIT License

Copyright (c) 2020 Andrey Vasiliev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "pcheader.h"

#include "DotSceneLoaderB.h"

#include "Application.h"
#include "CameraMan.h"
#include "TerrainMaterialGeneratorB.h"
#include "CubeMapCamera.h"
#include "ReflectionCamera.h"
#include "Utils.h"

using namespace utils;

namespace Context {
//----------------------------------------------------------------------------------------------------------------------
float DotSceneLoaderB::GetHeigh(float x, float z) {
//  if (terrain_created_) {
//    return DotSceneLoaderB::Instance().ogre_terrain_group_->getHeightAtWorldPosition(x, 1000, z);
//  } else {
//    return 0.0f;
//  }
  return 0.0;
}
//----------------------------------------------------------------------------------------------------------------------
DotSceneLoaderB::DotSceneLoaderB() {
  if (Ogre::SceneLoaderManager::getSingleton()._getSceneLoader("DotScene")) {
    Ogre::SceneLoaderManager::getSingleton().unregisterSceneLoader("DotScene");
  }
  if (Ogre::SceneLoaderManager::getSingleton()._getSceneLoader("DotSceneB")) {
    Ogre::SceneLoaderManager::getSingleton().unregisterSceneLoader("DotSceneB");
  }

  Ogre::SceneLoaderManager::getSingleton().registerSceneLoader("DotSceneB", {".scene", ".xml"}, this);

//  ConfiguratorJson::Instance().Assign(physics_enable_, "physics_enable");
//  ConfiguratorJson::Instance().Assign(lod_generator_enable_, "lod_generator_enable");
//  ConfiguratorJson::Instance().Assign(terrain_cast_shadows_, "terrain_cast_shadows");
//  ConfiguratorJson::Instance().Assign(terrain_raybox_calculation_, "terrain_raybox_calculation");
}
//----------------------------------------------------------------------------------------------------------------------
DotSceneLoaderB::~DotSceneLoaderB() {
  if (ogre_terrain_group_) {
    ogre_terrain_group_->removeAllTerrains();
    ogre_terrain_group_.reset();
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::load(Ogre::DataStreamPtr &stream, const std::string &groupName, Ogre::SceneNode *rootNode) {
  group_name_ = groupName;
  scene_manager_ = rootNode->getCreator();

  pugi::xml_document XMLDoc; // character type defaults to char

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
        "[DotSceneLoader] Invalid .scene File. Missing <scene formatVersion='x.y' >");
    return;
  }

  // figure out where to attach any nodes we Create
  attach_node_ = rootNode;

  // Process the scene
  ProcessScene_(XMLRoot);
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::Load(const std::string &filename, const std::string &group_name, Ogre::SceneNode *root_node) {
  group_name_ = group_name;
  scene_manager_ = root_node->getCreator();

  pugi::xml_document XMLDoc; // character type defaults to char

  auto result = XMLDoc.load_file(filename.c_str());

  if (!result) {
    Ogre::LogManager::getSingleton().stream(Ogre::LML_CRITICAL) << "[DotSceneLoader] " << result.description();
    return;
  }

  // Grab the scene node
  auto XMLRoot = XMLDoc.child("scene");

  // Validate the File
  if (!XMLRoot.attribute("formatVersion")) {
    Ogre::LogManager::getSingleton().logError(
        "[DotSceneLoader] Invalid .scene File. Missing <scene formatVersion='x.y' >");
    return;
  }

  // figure out where to attach any nodes we Create
  attach_node_ = root_node;

  // Process the scene
  ProcessScene_(XMLRoot);
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessScene_(pugi::xml_node &xml_root) {
  // Process the scene parameters
  std::string version = GetAttrib(xml_root, "formatVersion", "unknown");

  std::string message = "[DotSceneLoader] Parsing dotScene file with version " + version;

  if (xml_root.attribute("sceneManager"))
    message += ", scene manager " + std::string(xml_root.attribute("sceneManager").value());

  if (xml_root.attribute("minOgreVersion"))
    message += ", min. Ogre version " + std::string(xml_root.attribute("minOgreVersion").value());

  if (xml_root.attribute("author"))
    message += ", author " + std::string(xml_root.attribute("author").value());

  Ogre::LogManager::getSingleton().logMessage(message);

  // Process environment (?)
  // Process terrain (?)
  // Process light (?)
  if (auto element = xml_root.child("light")) {
    ProcessLight_(element);
  }

  if (auto element = xml_root.child("environment")) {
    ProcessEnvironment_(element);
  }

  if (auto element = xml_root.child("terrainGroup")) {
    ProcessTerrainGroup_(element);
  }

  // Process nodes (?)
  if (auto element = xml_root.child("nodes")) {
    ProcessNodes_(element);
  }

  // Process externals (?)
  if (auto element = xml_root.child("externals")) {
    ProcessExternals_(element);
  }

  // Process externals (?)
  if (auto element = xml_root.child("forest")) {
    ProcessForest_(element);
  }

  // Process userDataReference (?)
  if (auto element = xml_root.child("userData")) {
    ProcessUserData_(element, attach_node_->getUserObjectBindings());
  }

  // Process camera (?)
  if (auto element = xml_root.child("camera")) {
    ProcessCamera_(element);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessNodes_(pugi::xml_node &xml_node) {
  // Process node (*)
  for (auto element : xml_node.children("node")) {
    ProcessNode_(element);
  }

  // Process position (?)
  if (auto element = xml_node.child("position")) {
    attach_node_->setPosition(ParsePosition(element));
    attach_node_->setInitialState();
  }

  // Process rotation (?)
  if (auto element = xml_node.child("rotation")) {
    attach_node_->setOrientation(ParseQuaternion(element));
    attach_node_->setInitialState();
  }

  // Process scale (?)
  if (auto element = xml_node.child("scale")) {
    attach_node_->setScale(ParseScale(element));
    attach_node_->setInitialState();
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessExternals_(pugi::xml_node &xml_node) {
  //! @todo Implement this
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessEnvironment_(pugi::xml_node &xml_node) {
  // Process camera (?)
  auto *viewport = Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default")->getViewport();

  if (auto element = xml_node.child("camera")) {
    ProcessCamera_(element);
  }

  // Process fog (?)
  if (auto element = xml_node.child("fog")) {
    ProcessFog_(element);
  }

  // Process skyBox (?)
  if (auto element = xml_node.child("skyBox")) {
    ProcessSkyBox_(element);
  }

  // Process skyDome (?)
  if (auto element = xml_node.child("skyDome")) {
    ProcessSkyDome_(element);
  }

  // Process skyPlane (?)
  if (auto element = xml_node.child("skyPlane")) {
    ProcessSkyPlane_(element);
  }

  // Process colourAmbient (?)
  if (auto element = xml_node.child("colourAmbient")) {
    scene_manager_->setAmbientLight(ParseColour(element));
  }

  // Process colourBackground (?)
  if (auto element = xml_node.child("colourBackground")) {
    viewport->setBackgroundColour(ParseColour(element));
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::CreateTerrainHeightfieldShape(int size,
                                                    float *data,
                                                    const float &min_height,
                                                    const float &max_height,
                                                    const Ogre::Vector3 &position,
                                                    const float &scale) {
  // Convert height data in a format suitable for the physics engine
  auto *terrainHeights = new float[size * size];
  assert(terrainHeights != 0);

  for (int i = 0; i < size; i++) {
    memcpy(terrainHeights + size * i, data + size * (size - i - 1), sizeof(float) * size);
  }

  const btScalar heightScale = 1.0f;

  btVector3 localScaling(scale, heightScale, scale);

  auto *terrainShape = new btHeightfieldTerrainShape(size,
                                                     size,
                                                     terrainHeights,
                                                     1,
                                                     min_height,
                                                     max_height,
                                                     1,
                                                     PHY_FLOAT,
                                                     false);

  terrainShape->setUseDiamondSubdivision(true);
  terrainShape->setLocalScaling(localScaling);

  auto *groundMotionState =
      new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(size / 2, 0, size / 2)));
  btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, terrainShape, btVector3(0, 0, 0));

  //Create Rigid Body using 0 mass so it is static
  auto *entBody = new btRigidBody(groundRigidBodyCI);

  entBody->setFriction(0.8f);
  entBody->setHitFraction(0.8f);
  entBody->setRestitution(0.6f);
  entBody->getWorldTransform().setOrigin(btVector3(position.x,
                                                   position.y + (max_height - min_height) / 2 - 0.5,
                                                   position.z));
  entBody->getWorldTransform().setRotation(BtOgre::Convert::toBullet(Ogre::Quaternion::IDENTITY));
  entBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);

  physics_->AddRigidBody(entBody);
  physics_->GetPhyWorld()->setForceUpdateAllAabbs(false);
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::GetTerrainImage(bool flipX,
                                      bool flipY,
                                      Ogre::Image &ogre_image,
                                      const std::string &filename = "terrain.dds") {
  ogre_image.load(filename, Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);

  if (flipX) {
    ogre_image.flipAroundY();
  }

  if (flipY) {
    ogre_image.flipAroundX();
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::DefineTerrain(long x, long y, bool flat, const std::string &filename = "terrain.dds") {
  // if a file is available, use it
  // if not, generate file from import

  // Usually in a real project you'll know whether the compact terrain data is
  // available or not; I'm doing it this way to save distribution size

  if (flat) {
    ogre_terrain_group_->defineTerrain(x, y, 0.0f);
    return;
  }

  Ogre::Image image;
  GetTerrainImage(x % 2 != 0, y % 2 != 0, image, filename);

  std::string cached = ogre_terrain_group_->generateFilename(x, y);
  if (Ogre::ResourceGroupManager::getSingleton().resourceExists(ogre_terrain_group_->getResourceGroup(),
                                                                cached)) {
    ogre_terrain_group_->defineTerrain(x, y);
  } else {
    ogre_terrain_group_->defineTerrain(x, y, &image);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::InitBlendMaps(Ogre::Terrain *terrain,
                                    int layer,
                                    const std::string &image = "terrain_blendmap.dds") {
  Ogre::TerrainLayerBlendMap *blendMap = terrain->getLayerBlendMap(layer);
  auto *pBlend1 = blendMap->getBlendPointer();

  Ogre::Image img;
  img.load(image, Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);

  int bmSize = terrain->getLayerBlendMapSize();

  for (int y = 0; y < bmSize; ++y) {
    for (int x = 0; x < bmSize; ++x) {
      float tx = static_cast<float>(x) / static_cast<float>(bmSize);
      float ty = static_cast<float>(y) / static_cast<float>(bmSize);
      int ix = static_cast<int>(img.getWidth() * tx);
      int iy = static_cast<int >(img.getWidth() * ty);

      Ogre::ColourValue cv = Ogre::ColourValue::Black;
      cv = img.getColourAt(ix, iy, 0);
//            Ogre::Real val = cv[0]*opacity[bi];
      float val = cv[0];
      *pBlend1++ = val;
    }
  }

  blendMap->dirty();
  blendMap->update();

  // set up a colour map
  if (terrain->getGlobalColourMapEnabled()) {
    Ogre::Image colourMap;
    colourMap.load("testcolourmap.dds", Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
    terrain->getGlobalColourMap()->loadImage(colourMap);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessTerrainGroup_(pugi::xml_node &xml_node) {
  float worldSize = GetAttribReal(xml_node, "worldSize");
  int mapSize = GetAttribInt(xml_node, "mapSize");
  int minBatchSize = Ogre::StringConverter::parseInt(xml_node.attribute("minBatchSize").value());
  int maxBatchSize = Ogre::StringConverter::parseInt(xml_node.attribute("maxBatchSize").value());
  int inputScale = Ogre::StringConverter::parseInt(xml_node.attribute("inputScale").value());
  int tuningCompositeMapDistance =
      Ogre::StringConverter::parseInt(xml_node.attribute("tuningCompositeMapDistance").value());
  int tuningMaxPixelError = GetAttribInt(xml_node, "tuningMaxPixelError", 8);
  auto *terrain_global_options = Ogre::TerrainGlobalOptions::getSingletonPtr();

  if (!terrain_global_options) {
    terrain_global_options = new Ogre::TerrainGlobalOptions();
  }

  OgreAssert(terrain_global_options, "Ogre::TerrainGlobalOptions not available");
  terrain_global_options->setMaxPixelError(static_cast<float>(tuningMaxPixelError));
  terrain_global_options->setCompositeMapDistance(static_cast<float>(tuningCompositeMapDistance));
  terrain_global_options->setCastsDynamicShadows(terrain_cast_shadows_);
  terrain_global_options->setUseRayBoxDistanceCalculation(terrain_raybox_calculation_);
  terrain_global_options->setDefaultMaterialGenerator(std::make_shared<TerrainMaterialGeneratorB>());

  ogre_terrain_group_ =
      std::make_shared<Ogre::TerrainGroup>(scene_manager_, Ogre::Terrain::ALIGN_X_Z, mapSize, worldSize);
  ogre_terrain_group_->setFilenameConvention("terrain", "bin");
  ogre_terrain_group_->setOrigin(Ogre::Vector3::ZERO);
  ogre_terrain_group_->setResourceGroup(Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

  Ogre::Terrain::ImportData &defaultimp = ogre_terrain_group_->getDefaultImportSettings();

  defaultimp.terrainSize = mapSize;
  defaultimp.worldSize = worldSize;
  defaultimp.inputScale = inputScale;
  defaultimp.minBatchSize = minBatchSize;
  defaultimp.maxBatchSize = maxBatchSize;

  for (auto pPageElement : xml_node.children("terrain")) {
    int pageX = Ogre::StringConverter::parseInt(pPageElement.attribute("x").value());
    int pageY = Ogre::StringConverter::parseInt(pPageElement.attribute("y").value());

    std::string heighmap = pPageElement.attribute("heightmap").value();
    DefineTerrain(pageX, pageY, false, heighmap);
    ogre_terrain_group_->loadTerrain(pageX, pageY, true);
    ogre_terrain_group_->getTerrain(pageX, pageY)->setGlobalColourMapEnabled(false);

    int layers_count = 0;
    for (const auto &pLayerElement : pPageElement.children("layer")) {
      layers_count++;
    }

    defaultimp.layerList.resize(layers_count);

    int layer_counter = 0;
    for (auto pLayerElement : pPageElement.children("layer")) {
      defaultimp.layerList[layer_counter].worldSize =
          Ogre::StringConverter::parseInt(pLayerElement.attribute("scale").value());
      defaultimp.layerList[layer_counter].textureNames.push_back(pLayerElement.attribute("diffuse").value());
      defaultimp.layerList[layer_counter].textureNames.push_back(pLayerElement.attribute("normal").value());

      layer_counter++;
    }

    layer_counter = 0;
    for (auto pLayerElement : pPageElement.children("layer")) {
      layer_counter++;
      if (layer_counter != layers_count) {
        InitBlendMaps(ogre_terrain_group_->getTerrain(pageX, pageY),
                      layer_counter,
                      pLayerElement.attribute("blendmap").value());
      }

    }
  }

  ogre_terrain_group_->freeTemporaryResources();

  bool terrain_collider = physics_enable_;

  if (physics_enable_ && terrain_collider) {
    auto terrainIterator = ogre_terrain_group_->getTerrainIterator();

    while (terrainIterator.hasMoreElements()) {
      auto *terrain = terrainIterator.getNext()->instance;

      CreateTerrainHeightfieldShape(terrain->getSize(),
                                    terrain->getHeightData(),
                                    terrain->getMinHeight(),
                                    terrain->getMaxHeight(),
                                    terrain->getPosition(),
                                    terrain->getWorldSize() / (static_cast<float>(terrain->getSize() - 1))
      );
    }
  }

  terrain_created_ = true;
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessLight_(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  // Process attributes
  static long counter = 0;
  auto *scene = Ogre::Root::getSingleton().getSceneManager("Default");
  std::string name = GetAttrib(xml_node, "name");

  if (!parent) {
    parent = scene->createSceneNode();
  }

  // Create the light
  name += std::to_string(counter);
  counter++;
  Ogre::Light *light = scene_manager_->createLight(name);

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

  // lights are oriented using SceneNodes that expect -Z to be the default direction
  // exporters should not write normal or direction if they attach lights to nodes
  if (auto element = xml_node.child("rotation")) {
    parent->rotate(ParseQuaternion(element));
  }

  light->setVisible(GetAttribBool(xml_node, "visible", true));
  light->setCastShadows(GetAttribBool(xml_node, "castShadows", false));
  light->setPowerScale(GetAttribReal(xml_node, "powerScale", 1.0));

  // Process colourDiffuse (?)
  if (auto element = xml_node.child("colourDiffuse")) {
    light->setDiffuseColour(ParseColour(element));
  }

  // Process colourSpecular (?)
  if (auto element = xml_node.child("colourSpecular")) {
    light->setSpecularColour(ParseColour(element));
  }

  if (sValue != "directional") {
    // Process lightRange (?)
    if (auto element = xml_node.child("lightRange")) {
      ProcessLightRange_(element, light);
    }

    // Process lightAttenuation (?)
    if (auto element = xml_node.child("lightAttenuation")) {
      ProcessLightAttenuation_(element, light);
    }
  }

  // Process userDataReference (?)
  if (auto element = xml_node.child("userData")) {
    ProcessUserData_(element, light->getUserObjectBindings());
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessCamera_(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  // Process attributes
  std::string name = GetAttrib(xml_node, "name");
  float fov = GetAttribReal(xml_node, "fov", 45);
  float aspectRatio = GetAttribReal(xml_node, "aspectRatio", 0);
  std::string projectionType = GetAttrib(xml_node, "projectionType", "perspective");

  // Create the camera
  auto *pCamera = Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default");

  if (!camera_man_) {
    camera_man_ = std::make_shared<CameraMan>();
    io_->RegListener(camera_man_.get());
  }
  camera_man_->RegCamera(parent, pCamera);
  camera_man_->SetStyle(CameraMan::FPS);

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

  float mass = 100.0f;
  entShape->calculateLocalInertia(mass, inertia);
  auto *bodyState = new BtOgre::RigidBodyState(parent);
  entBody = new btRigidBody(mass, bodyState, entShape, inertia);
  entBody->setAngularFactor(0);
  entBody->activate(true);
  entBody->forceActivationState(DISABLE_DEACTIVATION);
  entBody->setActivationState(DISABLE_DEACTIVATION);
  entBody->setFriction(1.0);
  physics_->AddRigidBody(entBody);
  camera_man_->SetRigidBody(entBody);
  // Set the field-of-view
  pCamera->setFOVy(Ogre::Radian(fov));

  // Set the aspect ratio
  if (aspectRatio != 0)
    pCamera->setAspectRatio(aspectRatio);

  // Set the projection type
  if (projectionType == "perspective") {
    pCamera->setProjectionType(Ogre::PT_PERSPECTIVE);
  } else if (projectionType == "orthographic") {
    pCamera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
  }

  // Process clipping (?)
  if (auto element = xml_node.child("clipping")) {
    float nearDist = GetAttribReal(element, "near");
    pCamera->setNearClipDistance(nearDist);

    float farDist = GetAttribReal(element, "far");
    pCamera->setFarClipDistance(farDist);
  }

  // Process userDataReference (?)
  if (auto element = xml_node.child("userData")) {
    ProcessUserData_(element, static_cast<Ogre::MovableObject *>(pCamera)->getUserObjectBindings());
  }

  if (auto element = xml_node.child("light")) {
    ProcessLight_(element, camera_man_->GetCameraNode());
  }

}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessNode_(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  // Construct the node's name
  static long counter = 0;
  std::string name = GetAttrib(xml_node, "name");

  // Create the scene node
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
    name += std::to_string(counter);
    counter++;
    if (parent) {
      node = parent->createChildSceneNode(name);
    } else {
      node = attach_node_->createChildSceneNode(name);
    }
  }

  // Process other attributes

  // Process position (?)
  if (auto element = xml_node.child("position")) {
    node->setPosition(ParsePosition(element));
    node->setInitialState();
  }

  // Process rotation (?)
  if (auto element = xml_node.child("rotation")) {
    node->setOrientation(ParseQuaternion(element));
    node->setInitialState();
  }

  // Process rotation (?)
  if (auto element = xml_node.child("direction")) {
    node->setDirection(ParsePosition(element).normalisedCopy());
    node->setInitialState();
  }

  // Process scale (?)
  if (auto element = xml_node.child("scale")) {
    node->setScale(ParseScale(element));
    node->setInitialState();
  }

  // Process lookTarget (?)
  if (auto element = xml_node.child("lookTarget"))
    ProcessLookTarget_(element, node);

  // Process trackTarget (?)
  if (auto element = xml_node.child("trackTarget"))
    ProcessTrackTarget_(element, node);

  // Process camera (*)
  for (auto element : xml_node.children("camera")) {
    ProcessCamera_(element, node);
  }

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

  // Process userDataReference (?)
  if (auto element = xml_node.child("userData")) {
    ProcessUserData_(element, node->getUserObjectBindings());
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessLookTarget_(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  //! @todo Is this correct? Cause I don't have a clue actually

  // Process attributes
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

  // Process position (?)
  Ogre::Vector3 position;

  if (auto element = xml_node.child("position")) {
    position = ParseVector3(element);
  }

  // Process localDirection (?)
  Ogre::Vector3 localDirection = Ogre::Vector3::NEGATIVE_UNIT_Z;

  if (auto element = xml_node.child("localDirection")) {
    localDirection = ParseVector3(element);
  }

  // Setup the look target
  try {
    if (!nodeName.empty()) {
      Ogre::SceneNode *pLookNode = scene_manager_->getSceneNode(nodeName);
      position = pLookNode->_getDerivedPosition();
    }

    parent->lookAt(position, relativeTo, localDirection);
  }
  catch (Ogre::Exception &e) {
    Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error processing a look target!");
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessTrackTarget_(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  // Process attributes
  std::string nodeName = GetAttrib(xml_node, "nodeName");

  // Process localDirection (?)
  Ogre::Vector3 localDirection = Ogre::Vector3::NEGATIVE_UNIT_Z;

  if (auto element = xml_node.child("localDirection"))
    localDirection = ParseVector3(element);

  // Process offset (?)
  Ogre::Vector3 offset = Ogre::Vector3::ZERO;

  if (auto element = xml_node.child("offset"))
    offset = ParseVector3(element);

  // Setup the track target
  try {
    Ogre::SceneNode *pTrackNode = scene_manager_->getSceneNode(nodeName);
    parent->setAutoTracking(true, pTrackNode, localDirection, offset);
  }
  catch (Ogre::Exception &e) {
    Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error processing a track target!");
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessEntity_(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  // Process attributes
  static long long counter = 0;
  std::string name = GetAttrib(xml_node, "name");
  std::string id = GetAttrib(xml_node, "id");
  std::string meshFile = GetAttrib(xml_node, "meshFile");
  std::string material = GetAttrib(xml_node, "material");
  bool castShadows = GetAttribBool(xml_node, "castShadows", true);

  // Create the entity
  Ogre::Entity *entity = nullptr;

  try {
    name += std::to_string(counter);
    counter++;
    entity = scene_manager_->createEntity(name, meshFile);
    entity->setCastShadows(castShadows);
    parent->attachObject(entity);

    auto mesh = entity->getMesh();

    if (lod_generator_enable_) {
      if (entity->getNumManualLodLevels() == 0) {
        Ogre::LodConfig lodConfig(mesh);
        lodConfig.advanced.useBackgroundQueue = false; // Non-threaded
        lodConfig.createGeneratedLodLevel(25, 0.10, Ogre::LodLevel::VRM_COLLAPSE_COST);
        lodConfig.createGeneratedLodLevel(50, 0.25, Ogre::LodLevel::VRM_COLLAPSE_COST);
        lodConfig.createGeneratedLodLevel(75, 0.50f, Ogre::LodLevel::VRM_COLLAPSE_COST);
        Ogre::MeshLodGenerator().generateAutoconfiguredLodLevels(mesh);

        if (!lodConfig.advanced.useBackgroundQueue) {
          auto *scene = Ogre::Root::getSingleton().getSceneManager("Default");
          if (entity) {
            scene->destroyEntity(entity);
            entity = nullptr; // createEntity may throw exception, so it is safer to reset to 0.
          }

          entity = scene->createEntity(name, lodConfig.mesh);
          entity->setCastShadows(castShadows);
          parent->attachObject(entity);
        }
      }
    }

    std::string shadow_technique = "texture";
//    ConfiguratorJson::Instance().Assign(shadow_technique, "graphics_shadows_tecnique");

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
      }
    }

    // Process userDataReference (?)
    if (auto element = xml_node.child("userData")) {
      ProcessUserData_(element, entity->getUserObjectBindings());
      physics_->ProcessData(entity->getUserObjectBindings(), entity, parent);
    }

  }
  catch (Ogre::Exception &e) {
    Ogre::LogManager::getSingleton().logMessage(e.getFullDescription());
    Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error loading an entity!");
  }

}//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessParticleSystem_(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  // Process attributes
  std::string name = GetAttrib(xml_node, "name");
  std::string templateName = GetAttrib(xml_node, "template");
  bool attachedCamera = GetAttribBool(xml_node, "attachedCamera", false);

  if (templateName.empty()) {
    templateName = GetAttrib(xml_node, "file"); // compatibility with old scenes
  }

  // Create the particle system
  try {
    Ogre::ParticleSystem *pParticles = scene_manager_->createParticleSystem(name, templateName);
    UpdatePbrParams(pParticles->getMaterialName());

    const Ogre::uint32 SUBMERGED_MASK = 0x0F0;
    const Ogre::uint32 SURFACE_MASK = 0x00F;
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
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessBillboardSet_(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  //! @todo Implement this
}
//----------------------------------------------------------------------------------------------------------------------
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
  if (!rcamera_) rcamera_ = std::make_unique<ReflectionCamera>();

  normal = {0, 1, 0};
  up = {0, 0, 1};

  Ogre::Plane plane(normal, distance);

  std::string mesh_name = name + "mesh";

  Ogre::MeshPtr terrain_mesh = Ogre::MeshManager::getSingleton().getByName(mesh_name,
                                                                           Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
  if (terrain_mesh)
    Ogre::MeshManager::getSingleton().remove(terrain_mesh);
  Ogre::MeshPtr res =
      Ogre::MeshManager::getSingletonPtr()->createPlane(mesh_name, group_name_, plane, width, height, xSegments,
                                                        ySegments, hasNormals, numTexCoordSets, uTile, vTile, up);
  res->buildTangentVectors();
  Ogre::Entity *entity = scene_manager_->createEntity(name, mesh_name);

  if (material.empty())
    return;

  if (!material.empty()) {
    entity->setMaterialName(material);
    UpdatePbrParams(material);
    UpdatePbrShadowReceiver(material);
    UpdatePbrShadowCaster(material);
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
//  auto *entShape = converter->createTrimesh();
  auto *entShape = converter->createBox();
  auto *bodyState = new BtOgre::RigidBodyState(parent);
  btRigidBody *entBody = new btRigidBody(0, bodyState, entShape, btVector3(0, 0, 0));
  entBody->setFriction(1);
  physics_->AddRigidBody(entBody);

  const Ogre::uint32 SUBMERGED_MASK = 0x0F0;
  const Ogre::uint32 SURFACE_MASK = 0x00F;
  const Ogre::uint32 WATER_MASK = 0xF00;
//  entity->setVisibilityFlags(WATER_MASK);
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessForest_(pugi::xml_node &xml_node) {
//  StaticForest::Instance().Create();
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessFog_(pugi::xml_node &xml_node) {
  // Process attributes
  float expDensity = GetAttribReal(xml_node, "density", 0.001);
  float linearStart = GetAttribReal(xml_node, "start", 0.0);
  float linearEnd = GetAttribReal(xml_node, "end", 1.0);
  Ogre::ColourValue colour = ColourValue::White;

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

  // Setup the fog
  scene_manager_->setFog(mode, colour, expDensity, linearStart, linearEnd);
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessSkyBox_(pugi::xml_node &xml_node) {
  // Process attributes
  std::string material = GetAttrib(xml_node, "material", "Skybox");
  std::string cubemap = GetAttrib(xml_node, "cubemap", "OutputCube.dds");
  float distance = GetAttribReal(xml_node, "distance", 1000);
  bool drawFirst = GetAttribBool(xml_node, "drawFirst", true);
  bool active = GetAttribBool(xml_node, "active", true);

  if (!active) {
    return;
  }

  // Process rotation (?)
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

  // Setup the sky box
  scene_manager_->setSkyBox(true, material, distance, drawFirst, rotation, group_name_);
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessSkyDome_(pugi::xml_node &xml_node) {
  // Process attributes
  std::string material = xml_node.attribute("material").value();
  float curvature = GetAttribReal(xml_node, "curvature", 10);
  float tiling = GetAttribReal(xml_node, "tiling", 8);
  float distance = GetAttribReal(xml_node, "distance", 1000);
  bool drawFirst = GetAttribBool(xml_node, "drawFirst", true);
  bool active = GetAttribBool(xml_node, "active", false);

  if (!active) {
    return;
  }

  // Process rotation (?)
  Ogre::Quaternion rotation = Ogre::Quaternion::IDENTITY;

  if (auto element = xml_node.child("rotation")) {
    rotation = ParseQuaternion(element);
  }

  // Setup the sky dome
  scene_manager_->setSkyDome(true,
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
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessSkyPlane_(pugi::xml_node &xml_node) {
  // Process attributes
  std::string material = GetAttrib(xml_node, "material");
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
  scene_manager_->setSkyPlane(true, plane, material, scale, tiling, drawFirst, bow, 1, 1, group_name_);
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessLightRange_(pugi::xml_node &xml_node, Ogre::Light *light) {
  // Process attributes
  float inner = GetAttribReal(xml_node, "inner");
  float outer = GetAttribReal(xml_node, "outer");
  float falloff = GetAttribReal(xml_node, "falloff", 1.0);

  // Setup the light range
  light->setSpotlightRange(Ogre::Angle(inner), Ogre::Angle(outer), falloff);
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessLightAttenuation_(pugi::xml_node &xml_node, Ogre::Light *light) {
  // Process attributes
  float range = GetAttribReal(xml_node, "range");
  float constant = GetAttribReal(xml_node, "constant");
  float linear = GetAttribReal(xml_node, "linear");
  float quadratic = GetAttribReal(xml_node, "quadratic");

  // Setup the light attenuation
  light->setAttenuation(range, constant, linear, quadratic);
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessUserData_(pugi::xml_node &xml_node, Ogre::UserObjectBindings &user_object_bindings) {
  // Process node (*)
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
