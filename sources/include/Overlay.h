// This source file is part of context-project
// Created by Andrew Vasiliev

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
  Overlay(view_ptr<Ogre::RenderWindow> render_window);
  virtual ~Overlay();

  void Cleanup() override;
  void Pause() override;
  void Resume() override;
  void Update(float time) override;
  void preViewportUpdate(const Ogre::RenderTargetViewportEvent &evt) override;
  void PrepareTexture(const std::string &name_, const std::string &group_ = Ogre::RGN_AUTODETECT);

 private:
  std::unique_ptr<ImGuiInputListener> imgui_listener_;
  view_ptr<Ogre::ImGuiOverlay> imgui_;
  view_ptr<Ogre::OverlaySystem> overlay_;
  view_ptr<Ogre::RenderWindow> render_window_;
};

}  // namespace glue
