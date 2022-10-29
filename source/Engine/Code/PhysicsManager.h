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
}  // namespace BtOgre

namespace Glue {
class PhysicsManager final : public System<PhysicsManager> {
 public:
  PhysicsManager();
  virtual ~PhysicsManager();

  void ProcessData(Ogre::Entity *entity, const Ogre::UserObjectBindings &userData);
  void CreateTerrainHeightfieldShape(Ogre::Terrain *terrain);
  void CreateTerrainHeightfieldShape(Ogre::TerrainGroup *terrainGroup);

 protected:
  /// System impl
  void OnSetUp() override;
  void OnClean() override;
  void OnUpdate(float time) override;

  bool sleep;
  int subSteps;

  std::unique_ptr<BtOgre::DynamicsWorld> dynamicWorld;
};
}  // namespace Glue
