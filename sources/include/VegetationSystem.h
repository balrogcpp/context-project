// This source file is part of "glue project"
// Created by Andrew Vasiliev

#pragma once
#include "SubSystem.h"
#include "view_ptr.h"
#include <functional>
#include <memory>

namespace Forests {
class PagedGeometry;
class PageLoader;
class GeometryPage;
}  // namespace Forests

namespace glue {

class VegetationSystem final : public SubSystem {
 public:
  VegetationSystem();
  virtual ~VegetationSystem();
  void GenerateGrassStatic();
  void GenerateGrassPaged();
  void GenerateRocksStatic();
  void GenerateTreesPaged();
  void ProcessForest();
  void Update(float time) override;

 protected:
  inline static std::function<float(float, float)> heigh_func_;
  std::vector<std::unique_ptr<Forests::PagedGeometry>> pgeometry_;
  std::vector<std::unique_ptr<Forests::PageLoader>> ploaders_;
  std::vector<std::unique_ptr<Forests::GeometryPage>> gpages_;
  std::vector<view_ptr<Ogre::StaticGeometry>> sgeometry_;
  //  const Ogre::uint32 SUBMERGED_MASK = 0x0F0;
  //  const Ogre::uint32 SURFACE_MASK = 0x00F;
  //  const Ogre::uint32 WATER_MASK = 0xF00;

 public:
  static void SetHeighFunc(const std::function<float(float, float)> &heigh_func) { heigh_func_ = heigh_func; }
};

}  // namespace glue
