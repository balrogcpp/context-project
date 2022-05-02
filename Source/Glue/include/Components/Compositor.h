// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include "Components/Component.h"
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

enum Compositors { FX_SSAO, FX_BLOOM, FX_BLUR, FX_FXAA, FX_HDR, FX_MAX };

class Compositor : public Component<Compositor> {
 public:
  Compositor();
  virtual ~Compositor();

  void OnSetUp() override;
  void OnClean() override;
  void OnPause() override;
  void OnResume() override;
  void OnUpdate(float time) override;
  void EnableEffect(const Compositors FX, bool Enable);

 protected:
  void AddCompositorEnabled(const std::string& Name);
  void AddCompositorDisabled(const std::string& Name);
  void EnableCompositor(const std::string& Name);
  void InitMRT();

  bool CompositorList[FX_MAX];
  const std::string OutputCompositor = "Output";
  const std::string BlurCompositor = "Blur";
  const std::string BloomCompositor = "Bloom";
  const std::string SSAOCompositor = "SSAO";
  const std::string HDRCompositor = "Output";
  const std::string FXAACompositor = "Output";
  const std::string BlurEnable = "uBlurEnable";
  const std::string BlurOutput = "Blur";
  const std::vector<std::string> BlurCompositorChain = {};
  const std::string BloomEnable = "uBloomEnable";
  const std::string BloomOutput = "Output";
  const std::vector<std::string> BloomCompositorChain = {"Bloom", "Bloom/FilterY", "Bloom/FilterX", "Downscale3x3", "Downscale2x2"};
  const std::string SSAOEnable = "uSSAOEnable";
  const std::string SSAOOutput = "Output";
  const std::vector<std::string> SSAOCompositorChain = {"SSAO"};
  const std::string FXAAEnable = "uFXAAEnable";
  const std::string FXAAOutput = "Blur";
  const std::vector<std::string> FXAACompositorChain = {};
  const std::string HDREnable = "uHDREnable";
  const std::string HDROutput = "Blur";
  const std::vector<std::string> HDRCompositorChain = {};
  Ogre::CompositorManager* OgreCompositorManager = nullptr;
  Ogre::CompositorChain* OgreCompositorChain = nullptr;
  Ogre::SceneManager* OgreSceneManager = nullptr;
  Ogre::Camera* OgreCamera = nullptr;
  Ogre::Viewport* OgreViewport = nullptr;
};

}  // namespace Glue
