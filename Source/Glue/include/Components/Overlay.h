// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Components/Component.h"
#include "LazySingleton.h"
#include "Input/ImGuiInputListener.h"

namespace Ogre {
class RenderTarget;
class Texture;
class SceneNode;
class ImGuiOverlay;
class OverlaySystem;
class RenderTargetViewportEvent;
}  // namespace Ogre

namespace Glue {

class Overlay final : public Component, public Singleton<Overlay>, public Ogre::RenderTargetListener {
 public:
  Overlay(Ogre::RenderWindow* OgreRenderWindowPtr);
  virtual ~Overlay();

  void Cleanup() override;
  void Pause() override;
  void Resume() override;
  void Update(float time) override;
  void preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt) override;
  void PrepareFontTexture(const std::string& FontName, const std::string& ResourceGroup = Ogre::RGN_AUTODETECT);

 protected:
  std::unique_ptr<ImGuiInputListener> ImGuiListener;
  Ogre::ImGuiOverlay* imgui_overlay = nullptr;
  Ogre::OverlaySystem* OgreOverlay = nullptr;
  Ogre::RenderWindow* OgreRenderWindow = nullptr;
};

}  // namespace Glue
