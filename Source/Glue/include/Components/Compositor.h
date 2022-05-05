// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include "Components/Component.h"
#include <array>
#include <map>
#include <memory>
#include <string>
#include <vector>

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

class Compositor : public Component<Compositor> {
 public:
  Compositor();
  virtual ~Compositor();

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

  int ViewportSizeX = 0;
  int ViewportSizeY = 0;
  std::map<std::string, FX> CompositorList;
  const FX Bloom{"Bloom", false, "uBloomEnable", "Output", {"Bloom", "Downscale3x3", "Downscale2x2"}};
  const FX SSAO{"SSAO", false, "uSSAOEnable", "Output", {"SSAO", "FilterX", "FilterY"}};
  const FX Blur{"Blur", false, "uBlurEnable", "Blur", {}};
  const FX FXAA{"FXAA", true, "uEnable", "FXAA", {}};
  const FX HDR{"HDR", false, "uHDREnable", "Output", {}};
  const std::string FX_SSAO = "SSAO";
  const std::string FX_BLOOM = "Bloom";
  const std::string FX_BLUR = "Blur";
  const std::string FX_FXAA = "FXAA";
  const std::string FX_HDR = "HDR";
  Ogre::CompositorManager* OgreCompositorManager = nullptr;
  Ogre::CompositorChain* OgreCompositorChain = nullptr;
  Ogre::SceneManager* OgreSceneManager = nullptr;
  Ogre::Camera* OgreCamera = nullptr;
  Ogre::Viewport* OgreViewport = nullptr;
};

}  // namespace Glue
