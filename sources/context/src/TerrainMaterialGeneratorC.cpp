//MIT License
//
//Copyright (c) 2020 Andrey Vasiliev
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//    of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//    copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//    copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.
//

/*
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
#include "pcheader.hpp"

#include "TerrainMaterialGeneratorC.hpp"
#include "DotSceneLoaderB.hpp"

namespace Context {
//---------------------------------------------------------------------
TerrainMaterialGeneratorC::TerrainMaterialGeneratorC() {
  mProfiles.push_back(OGRE_NEW SM2Profile(this, "SM2", "Profile for rendering on Shader Model 2 capable cards"));

  // TODO - check hardware capabilities & use fallbacks if required (more profiles needed)
  setActiveProfile(mProfiles.back());
}
//---------------------------------------------------------------------
TerrainMaterialGeneratorC::~TerrainMaterialGeneratorC() = default;
//---------------------------------------------------------------------
TerrainMaterialGeneratorC::SM2Profile::SM2Profile(TerrainMaterialGenerator *parent,
                                                  const Ogre::String &name,
                                                  const Ogre::String &desc)
    : Profile(parent, name, desc)
{
}
//---------------------------------------------------------------------
TerrainMaterialGeneratorC::SM2Profile::~SM2Profile() {
}
//---------------------------------------------------------------------
void TerrainMaterialGeneratorC::SM2Profile::requestOptions(Ogre::Terrain *terrain) {
  terrain->_setMorphRequired(false);
  terrain->_setNormalMapRequired(false);
  terrain->_setLightMapRequired(false, false);
  terrain->_setCompositeMapRequired(false);
  terrain->_setMorphRequired(true);
  terrain->_setNormalMapRequired(false);
  terrain->_setLightMapRequired(false);
  terrain->_setCompositeMapRequired(false);
}
//---------------------------------------------------------------------
bool TerrainMaterialGeneratorC::SM2Profile::isVertexCompressionSupported() const {
  return false;
}
//---------------------------------------------------------------------
void TerrainMaterialGeneratorC::SM2Profile::setLightmapEnabled(bool enabled) {
}
//---------------------------------------------------------------------
Ogre::uint8 TerrainMaterialGeneratorC::SM2Profile::getMaxLayers(const Ogre::Terrain *terrain) const {
  // count the texture units free
  return 1;
}
//---------------------------------------------------------------------
Ogre::MaterialPtr TerrainMaterialGeneratorC::SM2Profile::generate(const Ogre::Terrain *terrain) {
  std::string material_name = "Plane";
  DotSceneLoaderB::FixPbrParams(material_name);
  DotSceneLoaderB::FixPbrShadowReceiver(material_name);

  static long long counter = 0;
  counter++;
  return Ogre::MaterialManager::getSingleton().getByName(material_name)->clone(material_name + std::to_string(counter));
}
//---------------------------------------------------------------------
Ogre::MaterialPtr TerrainMaterialGeneratorC::SM2Profile::generateForCompositeMap(const Ogre::Terrain *terrain) {
  std::string material_name = "Plane";
  DotSceneLoaderB::FixPbrParams(material_name);
  DotSceneLoaderB::FixPbrShadowReceiver(material_name);

  static long long counter = 0;
  counter++;
  return Ogre::MaterialManager::getSingleton().getByName(material_name)->clone(material_name + std::to_string(counter));
}
//---------------------------------------------------------------------
void TerrainMaterialGeneratorC::SM2Profile::updateParams(const Ogre::MaterialPtr &mat, const Ogre::Terrain *terrain) {
}
//---------------------------------------------------------------------
void TerrainMaterialGeneratorC::SM2Profile::updateParamsForCompositeMap(const Ogre::MaterialPtr &mat,
                                                                        const Ogre::Terrain *terrain) {
}
} //namespace Context