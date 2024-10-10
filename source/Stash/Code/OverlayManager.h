/// created by Andrey Vasiliev

#pragma once
#include "System.h"
#include "SDLListener.h"

namespace gge {

class OverlayManager final : public DeviceListener, public WindowListener, public System<OverlayManager> {
 public:
  OverlayManager();
  virtual ~OverlayManager();

  /// System impl
  void OnSetUp() override;
  void OnUpdate(float time) override;
  void OnClean() override;

  void OnKeyEvent(SDL_Scancode key, bool pressed) override;
  void OnSizeChanged(int x, int y, uint32_t id) override;

 protected:
  Ogre::Root *ogreRoot = nullptr;
  Ogre::SceneManager *sceneManager = nullptr;
  Ogre::Camera *camera = nullptr;
};

}  // namespace gge
