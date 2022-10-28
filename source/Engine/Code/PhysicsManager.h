/// created by Andrey Vasiliev

#pragma once

#include "System.h"
#include <OgrePrerequisites.h>
#include <OgreTerrainGroup.h>
#include <functional>
#include <thread>

// forward declaration
class btDiscreteDynamicsWorld;
class btDbvtBroadphase;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;

namespace Glue {
class PhysicsManager final : public System<PhysicsManager> {
 public:
  PhysicsManager();
  virtual ~PhysicsManager();
  void InitThread();

  void ProcessData(Ogre::Entity *entity, const Ogre::UserObjectBindings &userData);
  void CreateTerrainHeightfieldShape(Ogre::Terrain *terrain);
  void CreateTerrainHeightfieldShape(Ogre::TerrainGroup *terrainGroup);

 protected:
  void OnSetUp() override;
  void OnClean() override;
  void OnUpdate(float time) override;

  std::atomic<bool> sleep;
  int SubSteps;
  bool threaded;
  int64_t updateRate;
  std::unique_ptr<std::thread> updateThread;

  std::unique_ptr<btDiscreteDynamicsWorld> btWorld;
  std::unique_ptr<btDbvtBroadphase> btBroadphase;
  std::unique_ptr<btDefaultCollisionConfiguration> btConfig;
  std::unique_ptr<btCollisionDispatcher> btDispatcher;
  std::unique_ptr<btSequentialImpulseConstraintSolver> btSolver;
};
}  // namespace Glue
