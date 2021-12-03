// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "SubSystem.h"
#include "ViewPtr.h"
#include <functional>
#include <memory>

namespace Forests {
class PagedGeometry;
class PageLoader;
class GeometryPage;
}  // namespace Forests

namespace Glue {

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
  static void SetHeighFunc(const std::function<float(float, float)> &func) { heigh_func = func; }

 protected:
  inline static std::function<float(float, float)> heigh_func;
  std::vector<std::unique_ptr<Forests::PagedGeometry>> pgeometry;
  std::vector<std::unique_ptr<Forests::PageLoader>> ploaders;
  std::vector<std::unique_ptr<Forests::GeometryPage>> gpages;
  std::vector<ViewPtr<Ogre::StaticGeometry>> sgeometry;
};

}  // namespace glue
