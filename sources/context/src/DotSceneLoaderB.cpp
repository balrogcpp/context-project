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

#include "pcheader.hpp"

#include "DotSceneLoaderB.hpp"

#include "StaticForestManager.hpp"
#include "PhysicsManager.hpp"
#include "CameraMan.hpp"
#include "ConfigManager.hpp"
#include "TerrainMaterialGeneratorB.hpp"
#include "TerrainMaterialGeneratorC.hpp"
#include "ShaderResolver.hpp"
#include "PSSMShadowCameraSetupB.hpp"
#include "CubeMapCamera.hpp"
#include "ReflectionCamera.hpp"

namespace {
//----------------------------------------------------------------------------------------------------------------------
std::string GetAttrib(const pugi::xml_node &xml_node,
                      const std::string &attrib,
                      const std::string &defaultValue = "") {
  if (auto anode = xml_node.attribute(attrib.c_str())) {
    return anode.value();
  } else {
    return defaultValue;
  }
}
//----------------------------------------------------------------------------------------------------------------------
float GetAttribReal(const pugi::xml_node &xml_node, const std::string &attrib, float defaultValue = 0) {
  if (auto anode = xml_node.attribute(attrib.c_str())) {
    return Ogre::StringConverter::parseReal(anode.value());
  } else {
    return defaultValue;
  }
}
//----------------------------------------------------------------------------------------------------------------------
int GetAttribInt(const pugi::xml_node &xml_node, const std::string &attrib, int defaultValue = 0) {
  if (auto anode = xml_node.attribute(attrib.c_str())) {
    return Ogre::StringConverter::parseInt(anode.value());
  } else {
    return defaultValue;
  }
}
//----------------------------------------------------------------------------------------------------------------------
bool GetAttribBool(const pugi::xml_node &xml_node, const std::string &attrib, bool defaultValue = false) {
  if (auto anode = xml_node.attribute(attrib.c_str())) {
    return anode.as_bool();
  } else {
    return defaultValue;
  }
}
//----------------------------------------------------------------------------------------------------------------------
Ogre::Vector3 ParseVector3(const pugi::xml_node &xml_node) {
  float x = Ogre::StringConverter::parseReal(xml_node.attribute("x").value());
  float y = Ogre::StringConverter::parseReal(xml_node.attribute("y").value());
  float z = Ogre::StringConverter::parseReal(xml_node.attribute("z").value());

  return Ogre::Vector3(x, y, z);
}
//----------------------------------------------------------------------------------------------------------------------
Ogre::Vector3 ParsePosition(const pugi::xml_node &xml_node) {
  float x = Ogre::StringConverter::parseReal(xml_node.attribute("x").value());
  float y = Ogre::StringConverter::parseReal(xml_node.attribute("y").value());
  float z = Ogre::StringConverter::parseReal(xml_node.attribute("z").value());

#ifdef OGRE_BUILD_COMPONENT_TERRAIN
  y += Context::DotSceneLoaderB::GetSingleton().GetHeigh(x, z);
#endif

  return Ogre::Vector3(x, y, z);
}
//----------------------------------------------------------------------------------------------------------------------
Ogre::Vector3 ParseScale(const pugi::xml_node &xml_node) {
  float x = Ogre::StringConverter::parseReal(xml_node.attribute("x").value());
  float y = Ogre::StringConverter::parseReal(xml_node.attribute("y").value());
  float z = Ogre::StringConverter::parseReal(xml_node.attribute("z").value());

  return Ogre::Vector3(x, y, z);
}
//----------------------------------------------------------------------------------------------------------------------
void ParseMaterial(Ogre::Entity *ent, const std::string &material_name) {
  Ogre::MaterialPtr material;
  if (!material_name.empty()) {
    material = Ogre::MaterialManager::getSingleton().getByName(material_name);

  }

  for (unsigned i = 0; i < ent->getNumManualLodLevels(); i++) {
    auto *lod = ent->getManualLodLevel(i);

    for (unsigned j = 0; j < lod->getNumSubEntities(); j++) {
      auto *subEnt = lod->getSubEntity(j);
      if (!material) {
        material = ent->getManualLodLevel(1)->getSubEntity(j)->getMaterial();
      }
      subEnt->setMaterial(material);
    }
  }
//  }
}
//----------------------------------------------------------------------------------------------------------------------
Ogre::Quaternion ParseQuaternion(const pugi::xml_node &xml_node) {
  //! @todo Fix this crap!

  Ogre::Quaternion orientation;

  if (xml_node.attribute("qw")) {
    orientation.w = Ogre::StringConverter::parseReal(xml_node.attribute("qw").value());
    orientation.x = Ogre::StringConverter::parseReal(xml_node.attribute("qx").value());
    orientation.y = Ogre::StringConverter::parseReal(xml_node.attribute("qy").value());
    orientation.z = Ogre::StringConverter::parseReal(xml_node.attribute("qz").value());
  } else if (xml_node.attribute("axisX")) {
    Ogre::Vector3 axis;
    axis.x = Ogre::StringConverter::parseReal(xml_node.attribute("axisX").value());
    axis.y = Ogre::StringConverter::parseReal(xml_node.attribute("axisY").value());
    axis.z = Ogre::StringConverter::parseReal(xml_node.attribute("axisZ").value());
    float angle = Ogre::StringConverter::parseReal(xml_node.attribute("angle").value());

    orientation.FromAngleAxis(Ogre::Angle(angle), axis);
  } else if (xml_node.attribute("angleX")) {
    Ogre::Matrix3 rot;
    rot.FromEulerAnglesXYZ(Ogre::StringConverter::parseAngle(xml_node.attribute("angleX").value()),
                           Ogre::StringConverter::parseAngle(xml_node.attribute("angleY").value()),
                           Ogre::StringConverter::parseAngle(xml_node.attribute("angleZ").value()));
    orientation.FromRotationMatrix(rot);
  } else if (xml_node.attribute("x")) {
    orientation.x = Ogre::StringConverter::parseReal(xml_node.attribute("x").value());
    orientation.y = Ogre::StringConverter::parseReal(xml_node.attribute("y").value());
    orientation.z = Ogre::StringConverter::parseReal(xml_node.attribute("z").value());
    orientation.w = Ogre::StringConverter::parseReal(xml_node.attribute("w").value());
  } else if (xml_node.attribute("w")) {
    orientation.w = Ogre::StringConverter::parseReal(xml_node.attribute("w").value());
    orientation.x = Ogre::StringConverter::parseReal(xml_node.attribute("x").value());
    orientation.y = Ogre::StringConverter::parseReal(xml_node.attribute("y").value());
    orientation.z = Ogre::StringConverter::parseReal(xml_node.attribute("z").value());
  }

  return orientation;
}
//----------------------------------------------------------------------------------------------------------------------
Ogre::ColourValue ParseColour(pugi::xml_node &xml_node) {
  return Ogre::ColourValue(Ogre::StringConverter::parseReal(xml_node.attribute("r").value()),
                           Ogre::StringConverter::parseReal(xml_node.attribute("g").value()),
                           Ogre::StringConverter::parseReal(xml_node.attribute("b").value()),
                           xml_node.attribute("a")
                           ? Ogre::StringConverter::parseReal(xml_node.attribute("a").value()) : 1);
}
//----------------------------------------------------------------------------------------------------------------------
Ogre::ColourValue ParseProperty(pugi::xml_node &xml_node) {
  std::string name = GetAttrib(xml_node, "name");
  std::string type = GetAttrib(xml_node, "type");
  std::string data = GetAttrib(xml_node, "data");

  return Ogre::ColourValue(Ogre::StringConverter::parseReal(xml_node.attribute("r").value()),
                           Ogre::StringConverter::parseReal(xml_node.attribute("g").value()),
                           Ogre::StringConverter::parseReal(xml_node.attribute("b").value()),
                           xml_node.attribute("a")
                           ? Ogre::StringConverter::parseReal(xml_node.attribute("a").value()) : 1);
}
}

namespace Context {

DotSceneLoaderB DotSceneLoaderB::DotSceneLoaderSingleton;

//----------------------------------------------------------------------------------------------------------------------
DotSceneLoaderB &DotSceneLoaderB::GetSingleton() {
  return DotSceneLoaderSingleton;
}
//----------------------------------------------------------------------------------------------------------------------
DotSceneLoaderB *DotSceneLoaderB::GetSingletonPtr() {
  return &DotSceneLoaderSingleton;
}
//----------------------------------------------------------------------------------------------------------------------
float DotSceneLoaderB::GetHeigh(float x, float z) {
#ifdef OGRE_BUILD_COMPONENT_TERRAIN
  if (terrain_created_) {
    if (!legacy_terrain_) {
      if (x < -terrain_box_.x && x > terrain_box_.y && z < terrain_box_.z && z > terrain_box_.w)
        return 0.0f;

      float size = std::sqrt(heigh_data_.size());
      int x_ = (x - terrain_box_.x) * size / (terrain_box_.y - terrain_box_.x);
      int z_ = (z - terrain_box_.z) * size / (terrain_box_.w - terrain_box_.z);
      z_ = size - z_ - 1;
      return DotSceneLoaderB::GetSingleton().heigh_data_[x_ + z_ * size];
    } else {
      return DotSceneLoaderB::GetSingleton().ogre_terrain_group_->getHeightAtWorldPosition(x, 1000, z);
    }
  } else {
    return 0.0f;
  }
#else
  return 0.0f;
#endif
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::Setup() {
  if (Ogre::SceneLoaderManager::getSingleton()._getSceneLoader("DotScene")) {
    Ogre::SceneLoaderManager::getSingleton().unregisterSceneLoader("DotScene");
  }
  if (Ogre::SceneLoaderManager::getSingleton()._getSceneLoader("DotSceneB")) {
    Ogre::SceneLoaderManager::getSingleton().unregisterSceneLoader("DotSceneB");
  }

  Ogre::SceneLoaderManager::getSingleton().registerSceneLoader("DotSceneB", {".scene", ".xml"}, GetSingletonPtr());

  ConfigManager::Assign(physics_enable_, "physics_enable");
  ConfigManager::Assign(lod_generator_enable_, "lod_generator_enable");
  ConfigManager::Assign(terrain_fog_perpixel_, "terrain_fog_perpixel");
  ConfigManager::Assign(terrain_receive_shadows_, "terrain_receive_shadows");
  ConfigManager::Assign(terrain_receive_shadows_low_lod_, "terrain_receive_shadows_low_lod");
  ConfigManager::Assign(terrain_cast_shadows_, "terrain_cast_shadows");
  ConfigManager::Assign(terrain_lightmap_enable_, "terrain_lightmap_enable");
  ConfigManager::Assign(terrain_lightmap_, "terrain_lightmap");
  ConfigManager::Assign(terrain_parallaxmap_enable_, "terrain_parallaxmap_enable");
  ConfigManager::Assign(terrain_specularmap_enable_, "terrain_specularmap_enable");
  ConfigManager::Assign(terrain_normalmap_enable_, "terrain_normalmap_enable");
  ConfigManager::Assign(terrain_save_terrains_, "terrain_save_terrains");
  ConfigManager::Assign(terrain_colourmap_enable_, "terrain_colourmap_enable");
  ConfigManager::Assign(terrain_raybox_calculation_, "terrain_raybox_calculation");
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DotSceneLoaderB::Reset() {
#ifdef OGRE_BUILD_COMPONENT_TERRAIN
  if (ogre_terrain_group_) {
    ogre_terrain_group_->removeAllTerrains();
    ogre_terrain_group_.reset();
  }

  terrain_created_ = false;
#endif
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DotSceneLoaderB::preRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) {

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DotSceneLoaderB::postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt) {

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool DotSceneLoaderB::frameRenderingQueued(const Ogre::FrameEvent &evt) {

  return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DotSceneLoaderB::ResetGlobal() {
  ManagerCommon::ResetGlobal();

  if (Ogre::SceneLoaderManager::getSingleton()._getSceneLoader("DotScene")) {
    Ogre::SceneLoaderManager::getSingleton().unregisterSceneLoader("DotScene");
  }

  if (Ogre::SceneLoaderManager::getSingleton()._getSceneLoader("DotSceneB")) {
    Ogre::SceneLoaderManager::getSingleton().unregisterSceneLoader("DotSceneB");
  }

#ifdef OGRE_BUILD_COMPONENT_TERRAIN
  if (ogre_terrain_group_) {
    ogre_terrain_group_->removeAllTerrains();
    ogre_terrain_group_.reset();
  }
#endif
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
  ProcessScene(XMLRoot);
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::load(const std::string &filename, const std::string &groupName, Ogre::SceneNode *rootNode) {
  group_name_ = groupName;
  scene_manager_ = rootNode->getCreator();

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
  attach_node_ = rootNode;

  // Process the scene
  ProcessScene(XMLRoot);
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessScene(pugi::xml_node &xml_root) {
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
    ProcessLight(element);
  }

  if (auto element = xml_root.child("environment")) {
    ProcessEnvironment(element);
  }

  if (auto element = xml_root.child("terrainGroup")) {
    ProcessTerrainGroup(element);
  }

  // Process nodes (?)
  if (auto element = xml_root.child("nodes")) {
    ProcessNodes(element);
  }

  if (auto element = xml_root.child("terrainGroup")) {
    ProcessTerrainLightmap(element);
  }

  // Process externals (?)
  if (auto element = xml_root.child("externals")) {
    ProcessExternals(element);
  }

  // Process externals (?)
  if (auto element = xml_root.child("forest")) {
    ProcessForest(element);
  }

  // Process userDataReference (?)
  if (auto element = xml_root.child("userData")) {
    ProcessUserData(element, attach_node_->getUserObjectBindings());
  }

  // Process camera (?)
  if (auto element = xml_root.child("camera")) {
    ProcessCamera(element);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessNodes(pugi::xml_node &xml_node) {
  // Process node (*)
  for (auto element : xml_node.children("node")) {
    ProcessNode(element);
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
void DotSceneLoaderB::ProcessExternals(pugi::xml_node &xml_node) {
  //! @todo Implement this
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessEnvironment(pugi::xml_node &xml_node) {
  // Process camera (?)
  if (auto element = xml_node.child("camera")) {
    ProcessCamera(element);
  }

  // Process fog (?)
  if (auto element = xml_node.child("fog")) {
    ProcessFog(element);
  }

  // Process skyBox (?)
  if (auto element = xml_node.child("skyBox")) {
    ProcessSkyBox(element);
  }

  // Process skyDome (?)
  if (auto element = xml_node.child("skyDome")) {
    ProcessSkyDome(element);
  }

  // Process skyPlane (?)
  if (auto element = xml_node.child("skyPlane")) {
    ProcessSkyPlane(element);
  }

  // Process colourAmbient (?)
  if (auto element = xml_node.child("colourAmbient")) {
    scene_manager_->setAmbientLight(ParseColour(element));
  }

  // Process colourBackground (?)
  if (auto element = xml_node.child("colourBackground")) {
    ogre_viewport_->setBackgroundColour(ParseColour(element));
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::PageConstructed(size_t pagex, size_t pagez, float *heightData) {
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

  auto phyWorld = PhysicsManager::GetSingleton().GetPhyWorld();

  phyWorld->addRigidBody(entBody);
  phyWorld->setForceUpdateAllAabbs(false);
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
  if (Ogre::ResourceGroupManager::getSingleton().resourceExists(DotSceneLoaderB::GetSingleton().GetTerrainGroup()->getResourceGroup(),
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
void DotSceneLoaderB::ProcessTerrainGroup(pugi::xml_node &xml_node) {
#ifdef OGRE_BUILD_COMPONENT_TERRAIN
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

  const bool flat = false;

  terrain_global_options->setMaxPixelError(static_cast<float>(tuningMaxPixelError));
  terrain_global_options->setCompositeMapDistance(static_cast<float>(tuningCompositeMapDistance));
  terrain_global_options->setUseRayBoxDistanceCalculation(terrain_raybox_calculation_);
  terrain_global_options->setDefaultMaterialGenerator(std::make_shared<TerrainMaterialGeneratorB>());
  terrain_global_options->setCastsDynamicShadows(terrain_cast_shadows_);

  if (legacy_terrain_) {
    auto *matProfile =
        dynamic_cast<TerrainMaterialGeneratorB::SM2Profile *>(terrain_global_options->getDefaultMaterialGenerator()->getActiveProfile());

    matProfile->setReceiveDynamicShadowsEnabled(terrain_receive_shadows_);
    matProfile->setReceiveDynamicShadowsLowLod(terrain_receive_shadows_low_lod_);
    matProfile->setReceiveDynamicShadowsDepth(terrain_receive_shadows_);
    matProfile->setReceiveDynamicShadowsPSSM(dynamic_cast<Ogre::PSSMShadowCameraSetup *>(ContextManager().GetSingleton().GetOgreShadowCameraSetup().get()));
    matProfile->setLayerParallaxMappingEnabled(terrain_parallaxmap_enable_);
    matProfile->setLayerNormalMappingEnabled(terrain_normalmap_enable_);
    matProfile->setLayerSpecularMappingEnabled(terrain_specularmap_enable_);
    matProfile->SetTerrainFogPerpixel(terrain_fog_perpixel_);
  }

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
    DefineTerrain(pageX, pageY, flat, heighmap);
    ogre_terrain_group_->loadTerrain(pageX, pageY, true);
    ogre_terrain_group_->getTerrain(pageX, pageY)->setGlobalColourMapEnabled(false);

    if (legacy_terrain_) {
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
  }

  if (terrain_save_terrains_ && legacy_terrain_) {
    ogre_terrain_group_->saveAllTerrains(true, true);
  }
  ogre_terrain_group_->freeTemporaryResources();

  bool terrain_collider = physics_enable_;

  if (physics_enable_ && terrain_collider) {
    auto terrainIterator = ogre_terrain_group_->getTerrainIterator();
    terrain_box_ = Ogre::Vector4(-worldSize / 2, worldSize / 2, -worldSize / 2, worldSize / 2);

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

  if (!legacy_terrain_) {
    ManualObject *obj = ogre_scene_manager_->createManualObject("Terrain_Manual_Object");
    obj->begin("Plane", Ogre::RenderOperation::OT_TRIANGLE_LIST);

    auto terrainIterator = ogre_terrain_group_->getTerrainIterator();
    terrain_box_ = Ogre::Vector4(-worldSize / 2, worldSize / 2, -worldSize / 2, worldSize / 2);

    while (terrainIterator.hasMoreElements()) {
      auto *terrain = terrainIterator.getNext()->instance;

      size_t size = terrain->getSize();
      float *data = terrain->getHeightData();
      heigh_data_.resize(size * size);

      const int step = 1;
      size_t counter = 0;

      for (int z = 0; z < size; z += step) {
        for (int x = 0; x < size; x += step) {
          float heigh0 = data[x + z * size];
          float heigh1 = data[x + 1 + z * size];
          float heigh2 = data[x + (z + 1) * size];

          heigh_data_[x + z * size] = heigh0;

          float x0 = worldSize * ((float) (x) / (float) size) - worldSize / 2;
          float z0 = worldSize * ((float) (size - z) / (float) size) - worldSize / 2;

          float x1 = worldSize * ((float) (x + step) / (float) size) - worldSize / 2;
          float z1 = worldSize * ((float) (size - z) / (float) size) - worldSize / 2;

          float x2 = worldSize * ((float) (x) / (float) size) - worldSize / 2;
          float z2 = worldSize * ((float) (size - z - step) / (float) size) - worldSize / 2;

          auto position0 = Ogre::Vector3(x0, heigh0, z0);
          auto position1 = Ogre::Vector3(x1, heigh1, z1);
          auto position2 = Ogre::Vector3(x2, heigh2, z2);

          auto v1 = position1 - position0;
          auto v2 = position2 - position0;

          Ogre::Vector3 normal = v1.crossProduct(v2);
          normal.normalise();

          if (x == 0 && z == 0 && x == size && z == size)
            normal = Ogre::Vector3(0, 1, 0);

          obj->position(position0);
          obj->normal(normal);

          obj->textureCoord((float) x / 4, (float) z / 4);
        }
      }

      for (int z = 0; z < size - 1; z++) {
        for (int x = 0; x < size - 1; x++) {
          obj->quad((x) + (z) * size, (x + 1) + (z) * size, (x + 1) + (z + 1) * size, (x) + (z + 1) * size);
//          obj->quad((x) + (z) * size, (x) + (z + 1) * size, (x + 1) + (z + 1) * size, (x + 1) + (z) * size);
        }
      }

      obj->end();

      std::string terrain_mesh_name = "Terrain_" + std::to_string(counter);
      if (Ogre::MeshManager::getSingleton().getByName(terrain_mesh_name,
                                                      Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME)) {
        Ogre::MeshManager::getSingleton().remove(terrain_mesh_name,
                                                 Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
      }

      auto terrain_mesh = obj->convertToMesh(terrain_mesh_name);
      terrain_mesh->buildTangentVectors();
      counter++;
      ogre_scene_manager_->getRootSceneNode()->createChildSceneNode()->attachObject(obj);

      auto *terrain_entity = ogre_scene_manager_->createEntity(terrain_mesh);
      terrain_entity->setCastShadows(false);
      const Ogre::uint32 SUBMERGED_MASK = 0x0F0;
      const Ogre::uint32 SURFACE_MASK = 0x00F;
      const Ogre::uint32 WATER_MASK = 0xF00;
      terrain_entity->setVisibilityFlags(SUBMERGED_MASK);
      ogre_scene_manager_->getRootSceneNode()->createChildSceneNode()->attachObject(terrain_entity);
      ogre_scene_manager_->destroyManualObject(obj);

      FixPbrParams("Plane");
      FixPbrShadowReceiver("Plane");
    }

    if (ogre_terrain_group_) {
      ogre_terrain_group_->removeAllTerrains();
      ogre_terrain_group_.reset();
    }
  }

  terrain_created_ = true;
#else
  OGRE_EXCEPT(Ogre::Exception::ERR_INVALID_CALL, "recompile with Ogre::Terrain component");
#endif
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessTerrainLightmap(pugi::xml_node &xml_node) {
#ifdef OGRE_BUILD_COMPONENT_TERRAIN
  auto *terrain_global_options = Ogre::TerrainGlobalOptions::getSingletonPtr();

  auto *matProfile =
      dynamic_cast<TerrainMaterialGeneratorB::SM2Profile *>(terrain_global_options->getDefaultMaterialGenerator()->getActiveProfile());

  matProfile->setLightmapEnabled(terrain_lightmap_enable_);

  if (!terrain_lightmap_enable_)
    return;

  if (!ogre_scene_manager_->getLight("Sun"))
    return;

  if (terrain_lightmap_enable_) {
    terrain_global_options->setLightMapSize(terrain_lightmap_size_);
    terrain_global_options->setLightMapDirection(ogre_scene_manager_->getLight("Sun")->getDerivedDirection());
    matProfile->setLightmapEnabled(terrain_lightmap_enable_);

    // Disable the lightmap for OpenGL ES 2.0. The minimum number of samplers allowed is 8(as opposed to 16 on desktop).
    // Otherwise we will run over the limit by just one. The minimum was raised to 16 in GL ES 3.0.
    if (Ogre::Root::getSingletonPtr()->getRenderSystem()->getCapabilities()->getNumTextureUnits() < 9) {
      TerrainMaterialGeneratorB::SM2Profile *matProfile =
          static_cast<TerrainMaterialGeneratorB::SM2Profile *>(terrain_global_options->getDefaultMaterialGenerator()->getActiveProfile());
      matProfile->setLightmapEnabled(false);
    }

  }

#else
  OGRE_EXCEPT(Ogre::Exception::ERR_INVALID_CALL, "recompile with Ogre::Terrain component");
#endif
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::EnsureHasTangents(Ogre::MeshPtr mesh_ptr) {
  bool generateTangents = false;
  if (mesh_ptr->sharedVertexData) {
    Ogre::VertexDeclaration *vertexDecl = mesh_ptr->sharedVertexData->vertexDeclaration;
    generateTangents |= HasNoTangentsAndCanGenerate(vertexDecl);
  }

  for (unsigned i = 0; i < mesh_ptr->getNumSubMeshes(); ++i) {
    Ogre::SubMesh *subMesh = mesh_ptr->getSubMesh(i);
    if (subMesh->vertexData) {
      Ogre::VertexDeclaration *vertexDecl = subMesh->vertexData->vertexDeclaration;
      generateTangents |= HasNoTangentsAndCanGenerate(vertexDecl);
    }
  }

  try {
    if (generateTangents) {
      mesh_ptr->buildTangentVectors();
    }

  }
  catch (...) {}
}
//----------------------------------------------------------------------------------------------------------------------
bool DotSceneLoaderB::HasNoTangentsAndCanGenerate(Ogre::VertexDeclaration *vertex_declaration) {
  bool hasTangents = false;
  bool hasUVs = false;
  const Ogre::VertexDeclaration::VertexElementList &elementList = vertex_declaration->getElements();
  auto itor = elementList.begin();
  auto end = elementList.end();

  while (itor != end && !hasTangents) {
    const Ogre::VertexElement &vertexElem = *itor;
    if (vertexElem.getSemantic() == Ogre::VES_TANGENT)
      hasTangents = true;
    if (vertexElem.getSemantic() == Ogre::VES_TEXTURE_COORDINATES)
      hasUVs = true;

    ++itor;
  }

  return !hasTangents && hasUVs;
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessLight(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  // Process attributes
  static long counter = 0;
  std::string name = GetAttrib(xml_node, "name");

  if (!parent) {
    parent = ogre_scene_manager_->createSceneNode();
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
  light->setCastShadows(GetAttribBool(xml_node, "castShadows", true));
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
      ProcessLightRange(element, light);
    }

    // Process lightAttenuation (?)
    if (auto element = xml_node.child("lightAttenuation")) {
      ProcessLightAttenuation(element, light);
    }
  }

  // Process userDataReference (?)
  if (auto element = xml_node.child("userData")) {
    ProcessUserData(element, light->getUserObjectBindings());
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessCamera(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  // Process attributes
  std::string name = GetAttrib(xml_node, "name");
  float fov = GetAttribReal(xml_node, "fov", 45);
  float aspectRatio = GetAttribReal(xml_node, "aspectRatio", 0);
  std::string projectionType = GetAttrib(xml_node, "projectionType", "perspective");

  // Create the camera
  auto *pCamera = ContextManager::GetSingleton().GetOgreCamera();

  ContextManager::GetSingleton().GetCameraMan()->UnregCamera();
  ContextManager::GetSingleton().GetCameraMan()->RegCamera(parent);
  CubeMapCamera::GetSingleton().FreeCamera();

  auto *actor = ogre_scene_manager_->createEntity("Actor", "Icosphere.mesh");
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
  PhysicsManager::GetSingleton().AddRigidBody(entBody);
  ContextManager::GetSingleton().GetCameraMan()->SetRigidBody(entBody);
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
    ProcessUserData(element, static_cast<Ogre::MovableObject *>(pCamera)->getUserObjectBindings());
  }

  if (auto element = xml_node.child("light")) {
    ProcessLight(element, ContextManager::GetSingleton().GetCameraMan()->GetCameraNode());
  }

}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessNode(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
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

  // Process scale (?)
  if (auto element = xml_node.child("scale")) {
    node->setScale(ParseScale(element));
    node->setInitialState();
  }

  // Process lookTarget (?)
  if (auto element = xml_node.child("lookTarget"))
    ProcessLookTarget(element, node);

  // Process trackTarget (?)
  if (auto element = xml_node.child("trackTarget"))
    ProcessTrackTarget(element, node);

  // Process camera (*)
  for (auto element : xml_node.children("camera")) {
    ProcessCamera(element, node);
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

  // Process userDataReference (?)
  if (auto element = xml_node.child("userData")) {
    ProcessUserData(element, node->getUserObjectBindings());
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessLookTarget(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
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
void DotSceneLoaderB::ProcessTrackTarget(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
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
void DotSceneLoaderB::UpdatePbrParams() {

}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::FixPbrShadowCaster(Ogre::MaterialPtr material) {
  std::string material_name = material->getName();
  static std::vector<std::string> material_list;

  if (std::find(material_list.begin(), material_list.end(), material_name) != material_list.end()) {
    return;
  } else {
    material_list.push_back(material_name);
  }

  auto *pass = material->getTechnique(0)->getPass(0);
  int alpha_rejection = static_cast<int>(pass->getAlphaRejectValue());

  if (material->getTechnique(0)->getPass(0)->getNumTextureUnitStates() > 0 && alpha_rejection > 0) {
    auto caster_material = Ogre::MaterialManager::getSingleton().getByName("PSSM/shadow_caster");
    auto new_caster = caster_material->clone("PSSM/shadow_caster" + std::to_string(material_list.size()));
    material->getTechnique(0)->setShadowCasterMaterial(new_caster);

    auto texture_albedo = pass->getTextureUnitState("Albedo");
    if (texture_albedo) {
      std::string texture_name = pass->getTextureUnitState("Albedo")->getTextureName();

      auto *texPtr3 = new_caster->getTechnique(0)->getPass(0)->getTextureUnitState("BaseColor");

      if (texPtr3) {
        texPtr3->setContentType(Ogre::TextureUnitState::CONTENT_NAMED);
        texPtr3->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_POINT);
        texPtr3->setTextureName(texture_name);
      }
    }
  }
  else {
    auto caster_material = Ogre::MaterialManager::getSingleton().getByName("PSSM/NoAlpha/shadow_caster");
    auto new_caster = caster_material->clone("PSSM/NoAlpha/shadow_caster" + std::to_string(material_list.size()));
    material->getTechnique(0)->setShadowCasterMaterial(new_caster);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::FixPbrParams(Ogre::MaterialPtr material) {
  std::string material_name = material->getName();
  bool registered = false;
  static std::vector<std::string> material_list;

  if (std::find(material_list.begin(), material_list.end(), material_name) != material_list.end()) {
    registered = true;
    return;
  } else {
    material_list.push_back(material_name);
  }

  const int light_count = 5;

  if (material->getTechnique(0)->getPass(0)->hasVertexProgram()) {
    auto vert_params = material->getTechnique(0)->getPass(0)->getVertexProgramParameters();

    auto &constants = vert_params->getConstantDefinitions();
    if (constants.map.count("uMVPMatrix") == 0) {
      return;
    }

    vert_params->setNamedAutoConstant("uMVPMatrix", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
    vert_params->setNamedAutoConstant("uModelMatrix", Ogre::GpuProgramParameters::ACT_WORLD_MATRIX);
    vert_params->setNamedAutoConstant("uLightCount", Ogre::GpuProgramParameters::ACT_LIGHT_COUNT);

    if (constants.map.count("fadeRange") > 0) {
      vert_params->setNamedConstant("fadeRange", 1 / (100.0f * 2.0f));
    }

    if (constants.map.count("uTime") > 0) {
      vert_params->setNamedConstantFromTime("uTime", 1);
    }
  }

  if (material->getTechnique(0)->getPass(0)->hasFragmentProgram()) {
    auto frag_params = material->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

    auto pass = material->getTechnique(0)->getPass(0);

    frag_params->setNamedAutoConstant("uAlphaRejection", Ogre::GpuProgramParameters::ACT_SURFACE_ALPHA_REJECTION_VALUE);
    frag_params->setNamedAutoConstant("uSurfaceAmbientColour", Ogre::GpuProgramParameters::ACT_SURFACE_AMBIENT_COLOUR);
    frag_params->setNamedAutoConstant("uSurfaceDiffuseColour", Ogre::GpuProgramParameters::ACT_SURFACE_DIFFUSE_COLOUR);
    frag_params->setNamedAutoConstant("uSurfaceSpecularColour",
                                      Ogre::GpuProgramParameters::ACT_SURFACE_SPECULAR_COLOUR);
    frag_params->setNamedAutoConstant("uSurfaceEmissiveColour",
                                      Ogre::GpuProgramParameters::ACT_SURFACE_EMISSIVE_COLOUR);
    frag_params->setNamedAutoConstant("uSurfaceShininessColour", Ogre::GpuProgramParameters::ACT_SURFACE_SHININESS);
    frag_params->setNamedAutoConstant("uAmbientLightColour", Ogre::GpuProgramParameters::ACT_AMBIENT_LIGHT_COLOUR);
    frag_params->setNamedAutoConstant("uLightCount", Ogre::GpuProgramParameters::ACT_LIGHT_COUNT);
    frag_params->setNamedAutoConstant("uLightPositionArray",
                                      Ogre::GpuProgramParameters::ACT_LIGHT_POSITION_ARRAY,
                                      light_count);
    frag_params->setNamedAutoConstant("uLightPositionObjectSpaceArray",
                                      Ogre::GpuProgramParameters::ACT_LIGHT_POSITION_OBJECT_SPACE_ARRAY,
                                      light_count);
    frag_params->setNamedAutoConstant("uLightPositionViewSpaceArray",
                                      Ogre::GpuProgramParameters::ACT_LIGHT_POSITION_VIEW_SPACE_ARRAY,
                                      light_count);
    frag_params->setNamedAutoConstant("uLightDirectionArray",
                                      Ogre::GpuProgramParameters::ACT_LIGHT_DIRECTION_ARRAY,
                                      light_count);
    frag_params->setNamedAutoConstant("uLightDiffuseScaledColourArray",
                                      Ogre::GpuProgramParameters::ACT_LIGHT_DIFFUSE_COLOUR_POWER_SCALED_ARRAY,
                                      light_count);
    frag_params->setNamedAutoConstant("uLightAttenuationArray",
                                      Ogre::GpuProgramParameters::ACT_LIGHT_ATTENUATION_ARRAY,
                                      light_count);
    frag_params->setNamedAutoConstant("uLightSpotParamsArray",
                                      Ogre::GpuProgramParameters::ACT_SPOTLIGHT_PARAMS_ARRAY,
                                      light_count);
    frag_params->setNamedAutoConstant("uFogColour", Ogre::GpuProgramParameters::ACT_FOG_COLOUR);
    frag_params->setNamedAutoConstant("uFogParams", Ogre::GpuProgramParameters::ACT_FOG_PARAMS);
    frag_params->setNamedAutoConstant("uCameraPosition", Ogre::GpuProgramParameters::ACT_CAMERA_POSITION);

    auto ibl_texture = pass->getTextureUnitState("IBL_Specular");
    const bool realtime_cubemap = false;
    if (ibl_texture) {
      if (realtime_cubemap) {
        CubeMapCamera::GetSingleton().Setup();
        ibl_texture->setTexture(CubeMapCamera::GetSingleton().GetDyncubemap());
      } else {
        std::string skybox_cubemap =
            Ogre::MaterialManager::getSingleton().getByName("SkyBox")->getTechnique(0)->getPass(0)->getTextureUnitState(
                "CubeMap")->getTextureName();
        ibl_texture->setTextureName(skybox_cubemap);
      }
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::FixPbrShadowReceiver(Ogre::MaterialPtr material) {
  std::string material_name = material->getName();
  bool registered = false;
  const int pssm_split_count = 3;
  const int light_count = 5;
  const int shadows = pssm_split_count * light_count;
  static std::vector<std::string> shadowed_list;

  if (!material->getReceiveShadows()) {
    return;
  }

  if (std::find(shadowed_list.begin(), shadowed_list.end(), material_name) != shadowed_list.end()) {
    registered = true;
    return;
  } else {
    shadowed_list.push_back(material_name);
  }

  auto *pssm =
      dynamic_cast<Ogre::PSSMShadowCameraSetup *>(ContextManager::GetSingleton().GetOgreShadowCameraSetup().get());

  if (material->getTechnique(0)->getPass(0)->hasVertexProgram()) {
    auto vert_params = material->getTechnique(0)->getPass(0)->getVertexProgramParameters();

    auto &constants = vert_params->getConstantDefinitions();
    if (constants.map.count("uTexWorldViewProjMatrixArray") == 0) {
      return;
    }

    vert_params->setNamedAutoConstant("uTexWorldViewProjMatrixArray",
                                      Ogre::GpuProgramParameters::ACT_TEXTURE_WORLDVIEWPROJ_MATRIX_ARRAY,
                                      pssm_split_count);
    vert_params->setNamedAutoConstant("uLightCastsShadowsArray",
                                      Ogre::GpuProgramParameters::ACT_LIGHT_CASTS_SHADOWS_ARRAY,
                                      light_count);
  }

  if (material->getTechnique(0)->getPass(0)->hasFragmentProgram()) {
    auto frag_params = material->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

    auto pass = material->getTechnique(0)->getPass(0);

    if (ConfigManager::GetSingleton().GetBool("graphics_shadows_enable")) {
      Ogre::uint numTextures = 3;
      Ogre::Vector4 splitPoints;
      if (ConfigManager::GetSingleton().GetString("graphics_shadows_projection") == "pssm") {
        const Ogre::PSSMShadowCameraSetup::SplitPointList &splitPointList = pssm->getSplitPoints();
        // Populate from split point 1, not 0, since split 0 isn't useful (usually 0)

        for (int j = 1; j < numTextures; ++j) {
          splitPoints[j - 1] = splitPointList[j];
        }
      }
      const int light_count = 5;

      auto &constants = frag_params->getConstantDefinitions();
      if (constants.map.count("pssmSplitPoints") != 0) {
        frag_params->setNamedConstant("pssmSplitPoints", splitPoints);
        frag_params->setNamedAutoConstant("uShadowColour", Ogre::GpuProgramParameters::ACT_SHADOW_COLOUR);
        frag_params->setNamedAutoConstant("uLightCastsShadowsArray",
                                          Ogre::GpuProgramParameters::ACT_LIGHT_CASTS_SHADOWS_ARRAY,
                                          light_count);

        int texture_count = pass->getNumTextureUnitStates();

        for (int k = 0; k < 3; k++) {
          if (!registered) {
            Ogre::TextureUnitState *tu = pass->createTextureUnitState();
            tu->setContentType(Ogre::TextureUnitState::CONTENT_SHADOW);
            tu->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
            tu->setTextureBorderColour(Ogre::ColourValue::White);
            tu->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_POINT);
          }

          frag_params->setNamedConstant("shadowMap" + std::to_string(k), texture_count + k);

          frag_params->setNamedAutoConstant("inverseShadowmapSize" + std::to_string(k),
                                            Ogre::GpuProgramParameters::ACT_INVERSE_TEXTURE_SIZE,
                                            texture_count + k);
        }
      }
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::UpdateForestParams(Ogre::MaterialPtr material) {
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::FixPbrParams(const std::string &material) {
  DotSceneLoaderB::FixPbrParams(Ogre::MaterialManager::getSingleton().getByName(material));
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::FixPbrShadowReceiver(const std::string &material) {
  DotSceneLoaderB::FixPbrShadowReceiver(Ogre::MaterialManager::getSingleton().getByName(material));
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::FixPbrShadowCaster(const std::string &material) {
  DotSceneLoaderB::FixPbrShadowCaster(Ogre::MaterialManager::getSingleton().getByName(material));
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::UpdateForestParams(const std::string &material) {
  DotSceneLoaderB::UpdateForestParams(Ogre::MaterialManager::getSingleton().getByName(material));
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessEntity(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
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

          if (entity) {
            ogre_scene_manager_->destroyEntity(entity);
            entity = nullptr; // createEntity may throw exception, so it is safer to reset to 0.
          }

          entity = ogre_scene_manager_->createEntity(name, lodConfig.mesh);
          entity->setCastShadows(castShadows);
          parent->attachObject(entity);
        }
      }
    }

    std::string shadow_technique;
    ConfigManager::Assign(shadow_technique, "graphics_shadows_tecnique");

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

//    if (material.empty()) {
//      material = entity->getMesh()->getSubMesh(0)->getMaterialName();
//    }

//    if (!material.empty()) {
//      entity->setMaterialName(material);
//
//      for (int j = 0; j < entity->getNumManualLodLevels(); j++) {
//        auto *lod = entity->getManualLodLevel(j);
//        lod->setMaterialName(material);
//      }
//    }

    for (auto &submesh : mesh->getSubMeshes()) {
      Ogre::MaterialPtr material_ptr;

      if (!material.empty()) {
        entity->setMaterialName(material);
        material_ptr = Ogre::MaterialManager::getSingleton().getByName(material);
      } else if (submesh->getMaterial()) {
        material_ptr = submesh->getMaterial();
      }

      if (material_ptr) {
        FixPbrParams(material_ptr);

        if (entity->getCastShadows())
          FixPbrShadowCaster(material_ptr);

        if (material_ptr->getReceiveShadows())
          FixPbrShadowReceiver(material_ptr);
      }
    }

    // Process userDataReference (?)
    if (auto element = xml_node.child("userData")) {
      ProcessUserData(element, entity->getUserObjectBindings());
      PhysicsManager::GetSingleton().ProcessData(entity->getUserObjectBindings(), entity, parent);
    }

  }
  catch (Ogre::Exception &e) {
    Ogre::LogManager::getSingleton().logMessage(e.getFullDescription());
    Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error loading an entity!");
  }

}//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessParticleSystem(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
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
    FixPbrParams(pParticles->getMaterialName());

    const Ogre::uint32 SUBMERGED_MASK = 0x0F0;
    const Ogre::uint32 SURFACE_MASK = 0x00F;
    const Ogre::uint32 WATER_MASK = 0xF00;
    pParticles->setVisibilityFlags(WATER_MASK);

    if (attachedCamera) {
      ogre_camera_->getParentSceneNode()->createChildSceneNode(Ogre::Vector3{0, 10, 0})->attachObject(pParticles);
    } else {
      parent->attachObject(pParticles);
    }
  }
  catch (Ogre::Exception &e) {
    Ogre::LogManager::getSingleton().logMessage("[DotSceneLoader] Error creating a particle system!");
  }
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessBillboardSet(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
  //! @todo Implement this
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessPlane(pugi::xml_node &xml_node, Ogre::SceneNode *parent) {
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
    FixPbrParams(material);
    FixPbrShadowReceiver(material);
    FixPbrShadowCaster(material);
  }

  if (reflection) {
    Ogre::MaterialPtr material_ptr = Ogre::MaterialManager::getSingleton().getByName(material);

    auto material_unit = material_ptr->getTechnique(0)->getPass(0)->getTextureUnitState("ReflectionMap");

    if (material_unit) {
      ReflectionCamera::GetSingleton().Setup();

      material_unit->setTexture(ReflectionCamera::GetSingleton().GetReflectionTex());
      material_unit->setTextureAddressingMode(Ogre::TAM_CLAMP);
      material_unit->setTextureFiltering(Ogre::FO_LINEAR, Ogre::FO_LINEAR, Ogre::FO_POINT);
    }
  }

  ReflectionCamera::GetSingleton().RegPlane(plane);
  parent->attachObject(entity);

  std::unique_ptr<BtOgre::StaticMeshToShapeConverter>
      converter = std::make_unique<BtOgre::StaticMeshToShapeConverter>(entity);
  auto *entShape = converter->createTrimesh();
  auto *bodyState = new BtOgre::RigidBodyState(parent);
  btRigidBody *entBody = new btRigidBody(0, bodyState, entShape, btVector3(0, 0, 0));
  entBody->setFriction(1);
  PhysicsManager::GetSingleton().AddRigidBody(entBody);

  const Ogre::uint32 SUBMERGED_MASK = 0x0F0;
  const Ogre::uint32 SURFACE_MASK = 0x00F;
  const Ogre::uint32 WATER_MASK = 0xF00;
//  entity->setVisibilityFlags(WATER_MASK);
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessForest(pugi::xml_node &xml_node) {
  StaticForestManager::GetSingleton().Create();
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessFog(pugi::xml_node &xml_node) {
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
void DotSceneLoaderB::ProcessSkyBox(pugi::xml_node &xml_node) {
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
void DotSceneLoaderB::ProcessSkyDome(pugi::xml_node &xml_node) {
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
void DotSceneLoaderB::ProcessSkyPlane(pugi::xml_node &xml_node) {
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
void DotSceneLoaderB::ProcessLightRange(pugi::xml_node &xml_node, Ogre::Light *light) {
  // Process attributes
  float inner = GetAttribReal(xml_node, "inner");
  float outer = GetAttribReal(xml_node, "outer");
  float falloff = GetAttribReal(xml_node, "falloff", 1.0);

  // Setup the light range
  light->setSpotlightRange(Ogre::Angle(inner), Ogre::Angle(outer), falloff);
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessLightAttenuation(pugi::xml_node &xml_node, Ogre::Light *light) {
  // Process attributes
  float range = GetAttribReal(xml_node, "range");
  float constant = GetAttribReal(xml_node, "constant");
  float linear = GetAttribReal(xml_node, "linear");
  float quadratic = GetAttribReal(xml_node, "quadratic");

  // Setup the light attenuation
  light->setAttenuation(range, constant, linear, quadratic);
}
//----------------------------------------------------------------------------------------------------------------------
void DotSceneLoaderB::ProcessUserData(pugi::xml_node &xml_node, Ogre::UserObjectBindings &user_object_bindings) {
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
//----------------------------------------------------------------------------------------------------------------------
std::shared_ptr<Ogre::TerrainGroup> DotSceneLoaderB::GetTerrainGroup() {
  return ogre_terrain_group_;
}
}
