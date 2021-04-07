//MIT License
//
//Copyright (c) 2021 Andrew Vasiliev
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#pragma once
#include "SubComponent.h"

#include <functional>
#include <memory>
#include "view_ptr.h"

namespace Forests {
class PagedGeometry;
class PageLoader;
class GeometryPage;
}

namespace xio {
class Forest final : public SubComponent {
 public:
  Forest();
  virtual ~Forest();
  void GenerateGrassStatic();
  void GenerateGrassPaged();
  void GenerateRocksStatic();
  void GenerateTreesPaged();
  void ProcessForest();
  void Update(float time) override;

 private:
  inline static std::function<float(float, float)> heigh_func_;
  std::vector<std::unique_ptr<Forests::PagedGeometry>> pgeometry_;
  std::vector<std::unique_ptr<Forests::PageLoader>> ploaders_;
  std::vector<std::unique_ptr<Forests::GeometryPage>> gpages_;
  std::vector<view_ptr<Ogre::StaticGeometry>> sgeometry_;
  const Ogre::uint32 SUBMERGED_MASK = 0x0F0;
  const Ogre::uint32 SURFACE_MASK = 0x00F;
  const Ogre::uint32 WATER_MASK = 0xF00;

 public:
  static void SetHeighFunc(const std::function<float(float, float)> &heigh_func) {
	heigh_func_ = heigh_func;
  }
};

} //namespace
