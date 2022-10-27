/// created by Andrey Vasiliev

#pragma once

#include "System.h"
#include <Ogre.h>
#include <OgreTerrainGroup.h>
#include <functional>
#include <memory>
#include <thread>

// forward declaration
class btRigidBody;
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
  void OnSetUp() override;
  void OnClean() override;
  void OnUpdate(float time) override;
  void OnResume() override;
  void OnPause() override;

  void AddRigidBody(btRigidBody *rigidBody);
  void ProcessData(Ogre::Entity *entity, const Ogre::UserObjectBindings &userData);
  void CreateTerrainHeightfieldShape(Ogre::Terrain *terrain);
  void CreateTerrainHeightfieldShape(Ogre::TerrainGroup *terrainGroup);

 protected:
  std::unique_ptr<btDiscreteDynamicsWorld> btWorld;
  std::unique_ptr<btDbvtBroadphase> btBroadphase;
  std::unique_ptr<btDefaultCollisionConfiguration> btConfig;
  std::unique_ptr<btCollisionDispatcher> btDispatcher;
  std::unique_ptr<btSequentialImpulseConstraintSolver> btSolver;
  std::unique_ptr<std::thread> updateThread;
  int SubSteps = 4;
  bool threaded = false;
  int64_t updateRate = 60;
};

}  // namespace Glue
