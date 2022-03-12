// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include "Components/Component.h"
#include "Input/ImGuiInputListener.h"
#include <imgui.h>

namespace Ogre {
class RenderTarget;
class Texture;
class SceneNode;
class ImGuiOverlay;
class OverlaySystem;
class RenderTargetViewportEvent;
}  // namespace Ogre

namespace Glue {

class Overlay final : public Component<Overlay>, public Ogre::RenderTargetListener {
 public:
  explicit Overlay(Ogre::RenderWindow* OgreRenderWindowPtr);
  virtual ~Overlay();

  void OnClean() override;
  void OnPause() override;
  void OnResume() override;
  void OnUpdate(float time) override;
  void OrangeStyle();
  void SetupImGuiStyle(bool bStyleDark_, float alpha_);
  void preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt) override;
  void PrepareFontTexture(const std::string& FontName, const ImFontConfig* FontConfig = NULL, const ImWchar* GlyphRanges = NULL,
                          const std::string& ResourceGroup = Ogre::RGN_INTERNAL);

  void DrawSimpleOverlay(bool* p_open);
  void DrawAppFullscreen(bool* p_open);

 protected:
  std::unique_ptr<ImGuiInputListener> ImGuiListener;
  Ogre::ImGuiOverlay* ImGuiOverlayPtr = nullptr;
  Ogre::OverlaySystem* OgreOverlayPtr = nullptr;
  Ogre::RenderWindow* OgreRenderWindow = nullptr;
};

}  // namespace Glue
