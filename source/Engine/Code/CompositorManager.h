// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include "System.h"
//#include <array>
//#include <map>
//#include <memory>
//#include <string>
//#include <vector>

namespace Ogre {
class Camera;
class Viewport;
class SceneManager;
class CompositorChain;
}  // namespace Ogre

namespace Glue {

struct FX {
  std::string Name;
  bool Enabled = false;
  std::string EnableParam;
  std::string OutputCompositor;
  std::vector<std::string> CompositorChain;

  operator bool() { return Enabled; }
};

class CompositorManager : public System<CompositorManager> {
 public:
  CompositorManager();
  virtual ~CompositorManager();

  void OnSetUp() override;
  void OnClean() override;
  void OnPause() override;
  void OnResume() override;
  void OnUpdate(float time) override;
  void EnableEffect(const std::string& FX, bool Enable);

 protected:
  void AddCompositorEnabled(const std::string& Name);
  void AddCompositorDisabled(const std::string& Name);
  void EnableCompositor(const std::string& Name);
  void InitMRT();

  Ogre::CompositorManager* OgreCompositorManager = nullptr;
  Ogre::CompositorChain* OgreCompositorChain = nullptr;
  Ogre::SceneManager* OgreSceneManager = nullptr;
  Ogre::Camera* OgreCamera = nullptr;
  Ogre::Viewport* OgreViewport = nullptr;
  int ViewportSizeX = 0;
  int ViewportSizeY = 0;
  std::map<std::string, FX> CompositorList;
};

}  // namespace Glue
