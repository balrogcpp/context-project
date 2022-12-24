/// created by Andrey Vasiliev

#pragma once
#include <Terrain/OgreTerrainMaterialGenerator.h>
#include <Terrain/OgreTerrainPrerequisites.h>

namespace Ogre {
class TerrainMaterialGeneratorB final : public Ogre::TerrainMaterialGenerator {
 public:
  TerrainMaterialGeneratorB();
  virtual ~TerrainMaterialGeneratorB();

 private:
  class SM2Profile : public Ogre::TerrainMaterialGenerator::Profile {
   public:
    SM2Profile(Ogre::TerrainMaterialGenerator *parent, const Ogre::String &name, const Ogre::String &desc);
    virtual ~SM2Profile();

    Ogre::MaterialPtr generate(const Ogre::Terrain *terrain) override;
    Ogre::MaterialPtr generateForCompositeMap(const Ogre::Terrain *terrain) override;
    Ogre::uint8 getMaxLayers(const Ogre::Terrain *terrain) const override { return terrainMaxLayers; }
    void updateParams(const Ogre::MaterialPtr &mat, const Ogre::Terrain *terrain) override {}
    void updateParamsForCompositeMap(const Ogre::MaterialPtr &mat, const Ogre::Terrain *terrain) override {}
    void requestOptions(Ogre::Terrain *terrain) override;
    bool isVertexCompressionSupported() const override;
    void setLightmapEnabled(bool enabled) override;
    bool isLightmapEnabled();
    void setNormalmapEnabled(bool enabled);
    bool isNormalmapEnabled();
    void setCompositeMapEnabled(bool enabled);
    bool isCompositeMapEnabled();

   protected:
    bool enableLightmap;
    bool enableNormalmap;
    bool enableCompositeMap;
    int8_t terrainMaxLayers;
  };
};
}  // namespace Ogre
