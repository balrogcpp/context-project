// MIT License
//
// Copyright (c) 2021 Andrew Vasiliev
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once
#include <Terrain/OgreTerrainMaterialGenerator.h>
#include <Terrain/OgreTerrainPrerequisites.h>

namespace Ogre {
class PSSMShadowCameraSetup;
class Terrain;
};  // namespace Ogre

namespace xio {
class TerrainMaterialGeneratorB : public Ogre::TerrainMaterialGenerator {
 public:
  TerrainMaterialGeneratorB();
  virtual ~TerrainMaterialGeneratorB();

  class SM2Profile : public Ogre::TerrainMaterialGenerator::Profile {
   public:
    SM2Profile(Ogre::TerrainMaterialGenerator *parent, const Ogre::String &name,
               const Ogre::String &desc);
    virtual ~SM2Profile();

    Ogre::MaterialPtr generate(const Ogre::Terrain *terrain) override;
    Ogre::MaterialPtr generateForCompositeMap(
        const Ogre::Terrain *terrain) override;

    inline Ogre::uint8 getMaxLayers(
        const Ogre::Terrain *terrain) const override {
      return max_layers_;
    }
    inline void updateParams(const Ogre::MaterialPtr &mat,
                             const Ogre::Terrain *terrain) override {}
    inline void updateParamsForCompositeMap(
        const Ogre::MaterialPtr &mat, const Ogre::Terrain *terrain) override {}
    void requestOptions(Ogre::Terrain *terrain) override;
    bool isVertexCompressionSupported() const override;
    void setLightmapEnabled(bool enabled) override;

   private:
    bool lightmap_ = false;
    int8_t max_layers_ = 4;
  };
};

}  // namespace xio
