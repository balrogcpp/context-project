// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Components/Component.h"

namespace Ogre {
class Terrain;
class TerrainGroup;
class TerrainGlobalOptions;
}  // namespace Ogre

namespace pugi {
class xml_node;
}  // namespace pugi

namespace Glue {
class Landscape final : public Component {
 public:
  Landscape();
  virtual ~Landscape();

  void ProcessTerrainGroup(pugi::xml_node &xml_node);
  float GetHeigh(float x, float z);
  void Update(float time) override {}

 protected:
  void GetTerrainImage_(bool flipX, bool flipY, Ogre::Image &ogre_image, const std::string &filename);
  void DefineTerrain_(long x, long y, bool flat, const std::string &filename);
  void InitBlendMaps_(Ogre::Terrain *terrain, int layer, const std::string &image);

  std::unique_ptr<Ogre::TerrainGroup> terrain;
};

}  // namespace Glue
