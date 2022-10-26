// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include "Observer.h"
#include "System.h"
#include <Ogre.h>

namespace Glue {

class CompositorManager : public System<CompositorManager>, public WindowObserver {
 public:
  CompositorManager();
  virtual ~CompositorManager();

  void OnSetUp() override;
  void OnClean() override;
  void OnPause() override;
  void OnResume() override;
  void OnUpdate(float time) override;

  void AddCompositor(const std::string& name, bool enable, int position = -1);
  void SetCompositorEnabled(const std::string& name, bool enable);

 protected:
  void InitMRT();

  void OnEvent(const SDL_Event& event) override;
  void OnQuit() override;
  void OnFocusLost() override;
  void OnFocusGained() override;
  void OnSizeChanged(int x, int y, uint32_t id) override;
  void OnExposed() override;

  Ogre::CompositorManager* compositorManager = nullptr;
  Ogre::CompositorChain* compositorChain = nullptr;
  Ogre::SceneManager* sceneManager = nullptr;
  Ogre::Camera* ogreCamera = nullptr;
  Ogre::Viewport* ogreViewport = nullptr;
  const std::string MRT_COMPOSITOR;
  std::vector<std::string> compositorList;
};

}  // namespace Glue
