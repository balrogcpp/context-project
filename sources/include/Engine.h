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
#include "InputHandler.h"
#include "RenderSystem.h"
#include "AudioSystem.h"
#include "PhysicsSystem.h"
#include "DotSceneLoaderB.h"
#include "Overlay.h"
#include "AppState.h"
#include "Engine.h"
#include "Singleton.h"
#include "view_ptr.h"

namespace xio {


class Engine : public LazySingleton<Engine> {
 public:
  Engine();
  virtual ~Engine();


  void InitComponents();
  void Capture();
  void Pause();
  void InMenu();
  void OffMenu();
  void Resume();
  void Cleanup();
  void Refresh();
  void Update(float time);
  void RenderOneFrame();
  void RegComponent(view_ptr<Component> component);


 private:
  std::unique_ptr<InputHandler> io_;
  std::unique_ptr<Config> conf_;
  std::unique_ptr<RenderSystem> renderer_;
  std::unique_ptr<PhysicsSystem> physics_;
  std::unique_ptr<AudioSystem> audio_;
  std::unique_ptr<Overlay> overlay_;
  std::unique_ptr<DotSceneLoaderB> loader_;
  std::vector<view_ptr<Component>> components_;


  friend InputHandler& GetIo();
  friend Config& GetConf();
  friend RenderSystem& GetRS();
  friend Window& GetWindow();
  friend Compositor& GetCompositor();
  friend PhysicsSystem& GetPhysics();
  friend AudioSystem& GetAudio();
  friend Overlay& GetOverlay();
  friend DotSceneLoaderB& GetLoader();
};

} //namespace
