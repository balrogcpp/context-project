// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Components/Component.h"
#include "LazySingleton.h"
#include "PhysicalInput/ImGuiInputListener.h"

namespace Ogre {
class RenderTarget;
class Texture;
class SceneNode;
class ImGuiOverlay;
class OverlaySystem;
class RenderTargetViewportEvent;
}  // namespace Ogre

namespace Glue {
class Overlay final : public Component, public DynamicSingleton<Overlay>, public Ogre::RenderTargetListener {
 public:
  Overlay(Ogre::RenderWindow* render_window);
  virtual ~Overlay();

  void Cleanup() override;
  void Pause() override;
  void Resume() override;
  void Update(float time) override;
  void preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt) override;
  void PrepareTexture(const std::string& name_, const std::string& group_ = Ogre::RGN_AUTODETECT);

 protected:
  std::unique_ptr<ImGuiInputListener> imgui_listener;
  Ogre::ImGuiOverlay* imgui_overlay = nullptr;
  Ogre::OverlaySystem* overlay_system = nullptr;
  Ogre::RenderWindow* render_window = nullptr;
};

}  // namespace Glue
