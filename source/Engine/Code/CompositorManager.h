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
                          public Ogre::RenderTargetListener,
                          public Ogre::RenderObjectListener {
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

  /// System impl
  void OnSetUp() override;
  void OnClean() override;
  void OnUpdate(float time) override;
  void SetSleep(bool sleep) override;

 protected:
  /// Ogre::Viewport::Listener impl
  void viewportCameraChanged(Ogre::Viewport* viewport) override;
  void viewportDimensionsChanged(Ogre::Viewport* viewport) override;

  /// Ogre::CompositorInstance::Listener impl
  void notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr& mat) override;
  void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr& mat) override;
  void notifyResourcesCreated(bool forResizeOnly) override;

  /// Ogre::CompositorLogic impl
  void compositorInstanceCreated(Ogre::CompositorInstance* newInstance) override;
  void compositorInstanceDestroyed(Ogre::CompositorInstance* destroyedInstance) override;

  /// Ogre::RenderTargetListener impl
  void preRenderTargetUpdate(const Ogre::RenderTargetEvent& evt) override;
  void postRenderTargetUpdate(const Ogre::RenderTargetEvent& evt) override;

  /// Ogre::RenderObjectListener impl
  void notifyRenderSingleObject(Ogre::Renderable *rend, const Ogre::Pass *pass, const Ogre::AutoParamDataSource *source,
                                const Ogre::LightList *pLightList, bool suppressRenderStateChanges) override;

  Ogre::Plane plane;
  bool fixedViewportSize;
  std::queue<std::pair<std::string, bool>> compositorList;

  Ogre::CompositorManager* compositorManager = nullptr;
  Ogre::CompositorChain* compositorChain = nullptr;
  Ogre::SceneManager* sceneManager = nullptr;
  Ogre::Camera* camera = nullptr;
  Ogre::Camera* cubeCamera = nullptr;
  Ogre::Viewport* viewport = nullptr;
};
}  // namespace gge
