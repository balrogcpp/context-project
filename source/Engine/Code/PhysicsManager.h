/// created by Andrey Vasiliev

#pragma once
#include "System.h"
#include <OgrePrerequisites.h>

// forward declaration
namespace Ogre {
class Terrain;
class TerrainGroup;
}  // namespace Ogre

namespace BtOgre {
class DynamicsWorld;
class DebugDrawer;
}  // namespace BtOgre

namespace gge {
class PhysicsManager final : public System<PhysicsManager> {
 public:
  PhysicsManager();
  virtual ~PhysicsManager();

  void SetDebugView(bool debug);
  void ProcessData(Ogre::Entity *entity);
  void CreateTerrainHeightfieldShape(Ogre::Terrain *terrain);
  void CreateTerrainHeightfieldShape(Ogre::TerrainGroup *terrainGroup);

  /// System impl
  void OnSetUp() override;
  void OnClean() override;
  void OnUpdate(float time) override;

 protected:
  bool debugView;
  int subSteps;
  float fixedTimeStep;
  std::unique_ptr<BtOgre::DynamicsWorld> dynamicWorld;
  std::unique_ptr<BtOgre::DebugDrawer> debugDrawer;
};
}  // namespace gge
