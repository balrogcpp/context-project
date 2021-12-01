// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "ImGuiInputListener.h"
#include "Singleton.h"
#include "System.h"

namespace Ogre {
class RenderTarget;
class Texture;
class SceneNode;
class ImGuiOverlay;
class OverlaySystem;
class RenderTargetViewportEvent;
}  // namespace Ogre

namespace glue {
class Overlay final : public System, public Singleton<Overlay>, public Ogre::RenderTargetListener {
 public:
  Overlay(ViewPtr<Ogre::RenderWindow> render_window);
  virtual ~Overlay();

  void Cleanup() override;
  void Pause() override;
  void Resume() override;
  void Update(float time) override;
  void preViewportUpdate(const Ogre::RenderTargetViewportEvent &evt) override;
  void PrepareTexture(const std::string &name_, const std::string &group_ = Ogre::RGN_AUTODETECT);

 protected:
  std::unique_ptr<ImGuiInputListener> imgui_listener;
  ViewPtr<Ogre::ImGuiOverlay> imgui_overlay;
  ViewPtr<Ogre::OverlaySystem> overlay_system;
  ViewPtr<Ogre::RenderWindow> render_window;
};

}  // namespace glue
