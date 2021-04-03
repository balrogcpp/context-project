//MIT License
//
//Copyright (c) 2021 Andrew Vasiliev
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#pragma once
#include "Component.h"
#include "Singleton.h"
#include "gorilla/Gorilla.h"
#include "gorilla/OgreConsole.h"
//#include <Overlay/OgreImGuiInputListener.h>

namespace Ogre {
class RenderTarget;
class Texture;
class SceneNode;
class ImGuiOverlay;
class OverlaySystem;
class RenderTargetViewportEvent;
}

namespace xio {
 class Overlay final : public Component, public Singleton<Overlay>, public Ogre::RenderTargetListener {
 public:
  Overlay(view_ptr<Ogre::RenderWindow> render_window);
  virtual ~Overlay();

  void Cleanup() override;
  void Pause() override;
  void Resume() override;
  void Update(float time) override;
  void preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt) override;

  void Text(const std::string &str);
  void Show();
  void Hide();

 private:

  std::unique_ptr<Gorilla::Silverback> atlas_;
  view_ptr<Gorilla::Screen> screen_;
  view_ptr<Gorilla::Layer> layer_;
  view_ptr<Gorilla::Caption> caption_;
  view_ptr<Gorilla::Rectangle> rect_;

  std::unique_ptr<Gorilla::OgreConsole> console_;
  std::unique_ptr<Ogre::ImGuiOverlay> imgui_;
  view_ptr<Ogre::OverlaySystem> overlay_;
  view_ptr<Ogre::RenderWindow> window_;
};

} //namespace
