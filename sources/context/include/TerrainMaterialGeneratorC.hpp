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
*//*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2014 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#pragma once

#include <Terrain/OgreTerrainPrerequisites.h>
#include <Terrain/OgreTerrainMaterialGenerator.h>

namespace Ogre {
class PSSMShadowCameraSetup;
};

using namespace Ogre;

namespace Context {
class TerrainMaterialGeneratorC : public TerrainMaterialGenerator {
 public:
  TerrainMaterialGeneratorC();
  ~TerrainMaterialGeneratorC() override;

  class SM2Profile :
      public TerrainMaterialGenerator::Profile {
   public:
    SM2Profile(TerrainMaterialGenerator *parent,
               const String &name,
               const String &desc);
    ~SM2Profile() override;
    MaterialPtr generate(const Terrain *terrain) override;
    MaterialPtr generateForCompositeMap(const Terrain *terrain) override;
    uint8 getMaxLayers(const Terrain *terrain) const override;
    void updateParams(const MaterialPtr &mat, const Terrain *terrain) override;
    void updateParamsForCompositeMap(const MaterialPtr &mat, const Terrain *terrain) override;
    void requestOptions(Terrain *terrain) override;
    [[nodiscard]] bool isVertexCompressionSupported() const override;
    void setLightmapEnabled(bool enabled) override;

    ShaderHelper *mShaderGen;
    bool mLayerNormalMappingEnabled;
    bool mLayerParallaxMappingEnabled;
    bool mLayerSpecularMappingEnabled;
    bool mGlobalColourMapEnabled;
    bool mLightmapEnabled;
    bool mCompositeMapEnabled;
    bool mReceiveDynamicShadows;
    PSSMShadowCameraSetup *mPSSM;
    bool mDepthShadows;
    bool mLowLodShadows;
    bool terrain_fog_perpixel_ = true;
  };
};

//typedef TerrainMaterialGeneratorC::SM2Profile SM2Profile;
} //namespace Context
