// This source file is part of "glue project". Created by Andrey Vasiliev

#pragma once
#include <Terrain/OgreTerrainMaterialGenerator.h>
#include <Terrain/OgreTerrainPrerequisites.h>

namespace Ogre {
class PSSMShadowCameraSetup;
class Terrain;
};  // namespace Ogre

namespace Glue {
class TerrainMaterialGeneratorB final : public Ogre::TerrainMaterialGenerator {
 public:
  TerrainMaterialGeneratorB();
  virtual ~TerrainMaterialGeneratorB();

  class SM2Profile : public Ogre::TerrainMaterialGenerator::Profile {
   public:
    SM2Profile(Ogre::TerrainMaterialGenerator *parent, const Ogre::String &name, const Ogre::String &desc);
    virtual ~SM2Profile();

    Ogre::MaterialPtr generate(const Ogre::Terrain *OgreTerrainPtr) override;
    Ogre::MaterialPtr generateForCompositeMap(const Ogre::Terrain *terrain) override;
    inline Ogre::uint8 getMaxLayers(const Ogre::Terrain *terrain) const override { return TerrainMaxLayers; }
    inline void updateParams(const Ogre::MaterialPtr &mat, const Ogre::Terrain *terrain) override {}
    inline void updateParamsForCompositeMap(const Ogre::MaterialPtr &mat, const Ogre::Terrain *terrain) override {}
    void requestOptions(Ogre::Terrain *OgreTerrainPtr) override;
    bool isVertexCompressionSupported() const override;
    void setLightmapEnabled(bool enabled) override;

   protected:
    bool EnableLightmap = false;
    int8_t TerrainMaxLayers = 4;
  };
};

}  // namespace Glue
