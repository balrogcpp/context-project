// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include <Terrain/OgreTerrainMaterialGenerator.h>
#include <Terrain/OgreTerrainPrerequisites.h>

namespace Ogre {
class PSSMShadowCameraSetup;
class Terrain;
};  // namespace Ogre

namespace glue {
class TerrainMaterialGeneratorB : public Ogre::TerrainMaterialGenerator {
 public:
  TerrainMaterialGeneratorB();
  virtual ~TerrainMaterialGeneratorB();

  class SM2Profile : public Ogre::TerrainMaterialGenerator::Profile {
   public:
    SM2Profile(Ogre::TerrainMaterialGenerator *parent, const Ogre::String &name, const Ogre::String &desc);
    virtual ~SM2Profile();

    Ogre::MaterialPtr generate(const Ogre::Terrain *terrain) override;
    Ogre::MaterialPtr generateForCompositeMap(const Ogre::Terrain *terrain) override;

    inline Ogre::uint8 getMaxLayers(const Ogre::Terrain *terrain) const override { return max_layers_; }
    inline void updateParams(const Ogre::MaterialPtr &mat, const Ogre::Terrain *terrain) override {}
    inline void updateParamsForCompositeMap(const Ogre::MaterialPtr &mat, const Ogre::Terrain *terrain) override {}
    void requestOptions(Ogre::Terrain *terrain) override;
    bool isVertexCompressionSupported() const override;
    void setLightmapEnabled(bool enabled) override;

   protected:
    bool lightmap_ = false;
    int8_t max_layers_ = 4;
  };
};

}  // namespace glue
