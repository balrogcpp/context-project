// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Components/Landscape.h"
#include "Components/ComponentLocator.h"
#include "TerrainMaterialGeneratorB.h"
#ifdef OGRE_BUILD_COMPONENT_TERRAIN
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainAutoUpdateLod.h>
#include <Terrain/OgreTerrainGroup.h>
#include <Terrain/OgreTerrainQuadTreeNode.h>
#endif
#include <pugixml.hpp>

using namespace std;

namespace Glue {

Landscape::Landscape() {}

Landscape::~Landscape() {
  if (terrain) {
    terrain->removeAllTerrains();
    terrain.reset();
  }
}

void Landscape::GetTerrainImage_(bool flipX, bool flipY, Ogre::Image &ogre_image,
                                 const string &filename = "terrain.dds") {
  ogre_image.load(filename, Ogre::RGN_AUTODETECT);

  if (flipX) ogre_image.flipAroundY();

  if (flipY) ogre_image.flipAroundX();
}

void Landscape::DefineTerrain_(long x, long y, bool flat, const string &filename = "terrain.dds") {
  // if a file is available, use it
  // if not, generate file from import

  // Usually in a real project you'll know whether the compact terrain data is
  // available or not; I'm doing it this way to save distribution size

  if (flat) {
    terrain->defineTerrain(x, y, 0.0f);
    return;
  }

  Ogre::Image image;
  GetTerrainImage_(x % 2 != 0, y % 2 != 0, image, filename);

  string cached = terrain->generateFilename(x, y);
  if (Ogre::ResourceGroupManager::getSingleton().resourceExists(terrain->getResourceGroup(), cached)) {
    terrain->defineTerrain(x, y);
  } else {
    terrain->defineTerrain(x, y, &image);
  }
}

void Landscape::InitBlendMaps_(Ogre::Terrain *terrain, int layer, const string &image = "terrain_blendmap.dds") {
  Ogre::TerrainLayerBlendMap *blendMap = terrain->getLayerBlendMap(layer);
  auto *pBlend1 = blendMap->getBlendPointer();

  Ogre::Image img;
  img.load(image, Ogre::RGN_AUTODETECT);

  int bmSize = terrain->getLayerBlendMapSize();

  for (int y = 0; y < bmSize; ++y) {
    for (int x = 0; x < bmSize; ++x) {
      float tx = static_cast<float>(x) / static_cast<float>(bmSize);
      float ty = static_cast<float>(y) / static_cast<float>(bmSize);
      int ix = static_cast<int>(img.getWidth() * tx);
      int iy = static_cast<int>(img.getWidth() * ty);

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
    colourMap.load("testcolourmap.dds", Ogre::RGN_AUTODETECT);
    terrain->getGlobalColourMap()->loadImage(colourMap);
  }
}

void Landscape::ProcessTerrainGroup(pugi::xml_node &xml_node) {
  float worldSize = GetAttribReal(xml_node, "worldSize");
  int mapSize = GetAttribInt(xml_node, "mapSize");
  int minBatchSize = Ogre::StringConverter::parseInt(xml_node.attribute("minBatchSize").value());
  int maxBatchSize = Ogre::StringConverter::parseInt(xml_node.attribute("maxBatchSize").value());
  int inputScale = Ogre::StringConverter::parseInt(xml_node.attribute("inputScale").value());
  int tuningMaxPixelError = GetAttribInt(xml_node, "tuningMaxPixelError", 8);
  auto *terrain_global_options = Ogre::TerrainGlobalOptions::getSingletonPtr();

  if (!terrain_global_options) terrain_global_options = new Ogre::TerrainGlobalOptions();

  OgreAssert(terrain_global_options, "Ogre::TerrainGlobalOptions not available");
  terrain_global_options->setUseVertexCompressionWhenAvailable(true);
  terrain_global_options->setCastsDynamicShadows(true);
  terrain_global_options->setCompositeMapDistance(200);
  terrain_global_options->setMaxPixelError(static_cast<float>(tuningMaxPixelError));
  terrain_global_options->setUseRayBoxDistanceCalculation(true);
  terrain_global_options->setDefaultMaterialGenerator(make_shared<TerrainMaterialGeneratorB>());

  auto *scene_manager = Ogre::Root::getSingleton().getSceneManager("Default");
  terrain = make_unique<Ogre::TerrainGroup>(scene_manager, Ogre::Terrain::ALIGN_X_Z, mapSize, worldSize);
  terrain->setFilenameConvention("terrain", "bin");
  terrain->setOrigin(Ogre::Vector3::ZERO);
  terrain->setResourceGroup(Ogre::RGN_DEFAULT);

  Ogre::Terrain::ImportData &defaultimp = terrain->getDefaultImportSettings();

  defaultimp.terrainSize = mapSize;
  defaultimp.worldSize = worldSize;
  defaultimp.inputScale = inputScale;
  defaultimp.minBatchSize = minBatchSize;
  defaultimp.maxBatchSize = maxBatchSize;

  //  auto* matProfile =
  //  dynamic_cast<Ogre::TerrainMaterialGeneratorA::SM2Profile*>(terrain_global_options->getDefaultMaterialGenerator()->getActiveProfile());
  //  matProfile->setLightmapEnabled(false);
  //  matProfile->setLayerParallaxMappingEnabled(false);
  //  matProfile->setReceiveDynamicShadowsEnabled(true);
  //  matProfile->setReceiveDynamicShadowsDepth(true);
  //  matProfile->setReceiveDynamicShadowsLowLod(false);
  //  matProfile->setReceiveDynamicShadowsPSSM(rs_->GetShadowSettings().GetPssmSetup());
  //
  //  Ogre::Image combined;
  //  combined.loadTwoImagesAsRGBA("Ground23_col.jpg", "Ground23_spec.png",
  //  "General");
  //  Ogre::TextureManager::getSingleton().loadImage("Ground23_diffspec",
  //  "General", combined);
  //
  //  defaultimp.layerList.resize(3);
  //  defaultimp.layerList[0].worldSize = 20;
  //  defaultimp.layerList[0].textureNames.push_back("Ground37_diffspec.dds");
  //  defaultimp.layerList[0].textureNames.push_back("Ground37_normheight.dds");
  //  defaultimp.layerList[1].worldSize = 20;
  //  defaultimp.layerList[1].textureNames.push_back("Ground23_diffspec"); //
  //  loaded from memory
  //  defaultimp.layerList[1].textureNames.push_back("Ground23_normheight.dds");
  //  defaultimp.layerList[2].worldSize = 20;
  //  defaultimp.layerList[2].textureNames.push_back("Rock20_diffspec.dds");
  //  defaultimp.layerList[2].textureNames.push_back("Rock20_normheight.dds");

  for (auto &pPageElement : xml_node.children("terrain")) {
    int pageX = Ogre::StringConverter::parseInt(pPageElement.attribute("x").value());
    int pageY = Ogre::StringConverter::parseInt(pPageElement.attribute("y").value());

    string heighmap = pPageElement.attribute("heightmap").value();
    DefineTerrain_(pageX, pageY, false, heighmap);
    terrain->loadTerrain(pageX, pageY, true);
    terrain->getTerrain(pageX, pageY)->setGlobalColourMapEnabled(false);

    int layers_count = 0;
    //    for (const auto &pLayerElement : pPageElement.children("layer")) layers_count++;
    for (auto pLayerElement = pPageElement.children("layer").begin();
         pLayerElement != pPageElement.children("layer").end(); pLayerElement++) {
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
        InitBlendMaps_(terrain->getTerrain(pageX, pageY), layer_counter, pLayerElement.attribute("blendmap").value());
      }
    }
  }

  terrain->freeTemporaryResources();

  auto terrainIterator = terrain->getTerrainIterator();

  while (terrainIterator.hasMoreElements()) {
    auto *terrain = terrainIterator.getNext()->instance;

    GetPhysics().CreateTerrainHeightfieldShape(terrain->getSize(), terrain->getHeightData(), terrain->getMinHeight(),
                                               terrain->getMaxHeight(), terrain->getPosition(),
                                               terrain->getWorldSize() / (static_cast<float>(terrain->getSize() - 1)));
  }
}

float Landscape::GetHeigh(float x, float z) {
  if (terrain)
    return terrain->getHeightAtWorldPosition(x, 1000, z);
  else
    return 0.0f;
}

}  // namespace Glue
