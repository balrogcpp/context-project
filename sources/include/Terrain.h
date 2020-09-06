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

#pragma once

#include "Component.h"
#include "ComponentLocator.h"
#include <memory>

namespace Ogre {
class Terrain;
class TerrainGroup;
class TerrainGlobalOptions;
}

namespace pugi {
class xml_node;
}

namespace xio {
class Terrain final : public ComponentLocator {
 public:
  Terrain ();
  virtual ~Terrain ();

  void ProcessTerrainGroup(pugi::xml_node &xml_node);
  float GetHeigh(float x, float z);

 private:
  void GetTerrainImage_(bool flipX, bool flipY, Ogre::Image &ogre_image, const std::string &filename);
  void DefineTerrain_(long x, long y, bool flat, const std::string &filename);
  void InitBlendMaps_(Ogre::Terrain *terrain, int layer, const std::string &image);

  std::unique_ptr<Ogre::TerrainGroup> terrain_;
};
}