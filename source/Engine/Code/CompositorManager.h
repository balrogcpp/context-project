// This source file is part of gge Engine. Created by Andrey Vasiliev

#pragma once
#include "System.h"
#include <Ogre.h>
#include <OgreCompositorLogic.h>
#include <queue>

namespace gge {
class CompositorManager : public System<CompositorManager>,
                          public Ogre::CompositorInstance::Listener,
                          public Ogre::CompositorLogic,
                          public Ogre::Viewport::Listener,
                          public Ogre::RenderTargetListener {
 public:
  CompositorManager();
  virtual ~CompositorManager();

  void AddCompositor(const std::string& name, bool enable, int position = -1);
  void EnableCompositor(const std::string& name, bool enable);

  void SetFixedViewportSize(int x, int y);
  void SetFixedViewport(bool fixed);
  void SetCompositorScale(const std::string& name, float scale);
  bool IsCompositorInChain(const std::string& name);
  bool IsCompositorEnabled(const std::string& name);

  void CacheCompositorChain();
  void ApplyCachedCompositorChain();
  void RemoveAllCompositors();
  void DisableRendering();
  void EnableRendering();
  Ogre::Camera* GetOgreCamera();

  void AddFresnelCompositor(Ogre::Plane plane);
  void RemoveFresnelCompositor();
  void AddCubeCamera();
  void DestroyCubeCamera();

  /// System impl
  void OnSetUp() override;
  void OnClean() override;
  void OnUpdate(float time) override;
  void SetSleep(bool sleep) override;

 protected:
  void InitMRT(bool enable = true);

  /// Ogre::Viewport::Listener impl
  void viewportCameraChanged(Ogre::Viewport* viewport) override;
  void viewportDimensionsChanged(Ogre::Viewport* viewport) override;

  /// Ogre::CompositorInstance::Listener impl
  void notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr& mat) override;
  void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr& mat) override;

  /// Ogre::CompositorLogic impl
  void compositorInstanceCreated(Ogre::CompositorInstance* newInstance) override;
  void compositorInstanceDestroyed(Ogre::CompositorInstance* destroyedInstance) override;

  /// Ogre::RenderTargetListener impl
  void preRenderTargetUpdate(const Ogre::RenderTargetEvent& evt) override;
  void postRenderTargetUpdate(const Ogre::RenderTargetEvent& evt) override;

  std::string MRT_COMPOSITOR;
  std::string BLOOM_COMPOSITOR;
  Ogre::Plane plane;
  bool fixedViewportSize;
  std::queue<std::pair<std::string, bool>> compositorList;
  Ogre::Matrix4 viewProj;
  Ogre::Matrix4 viewProjPrev;

  Ogre::CompositorManager* compositorManager = nullptr;
  Ogre::CompositorChain* compositorChain = nullptr;
  Ogre::SceneManager* ogreSceneManager = nullptr;
  Ogre::Camera* ogreCamera = nullptr;
  Ogre::Camera* cubeCamera = nullptr;
  Ogre::Viewport* ogreViewport = nullptr;
};
}  // namespace gge
