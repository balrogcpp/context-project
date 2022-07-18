// This source file is part of Glue Engine. Created by Andrey Vasiliev

#pragma once
#include "Components/Component.h"
#include "Fonts/IconsFontAwesome5.h"
#include "Fonts/IconsKenney.h"
#include "Fonts/IconsMaterialDesign.h"
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

class Overlay final : public Component<Overlay>, public Ogre::RenderTargetListener {
 public:
  explicit Overlay(Ogre::RenderWindow* OgreRenderWindowPtr);
  virtual ~Overlay();

  void OnSetUp() override;
  void OnClean() override;
  void OnPause() override;
  void OnResume() override;
  void OnUpdate(float time) override;
  void preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt) override;
  void AddFont(const std::string& FontName, const ImFontConfig* FontConfig = nullptr, const ImWchar* GlyphRanges = nullptr,
               const std::string& ResourceGroup = Ogre::RGN_INTERNAL);

  void Show();
  void Hide();
  void DrawMouseCursor(bool Draw);
  static void NewFrame();

 protected:
  std::unique_ptr<ImGuiInputListener> ImGuiListener;
  Ogre::ImGuiOverlay* ImGuiOverlayPtr = nullptr;
  Ogre::OverlaySystem* OgreOverlayPtr = nullptr;
  Ogre::RenderWindow* OgreRenderWindow = nullptr;
  ImGuiIO* IoPtr = nullptr;
};

}  // namespace Glue
