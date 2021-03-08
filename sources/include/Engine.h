//MIT License
//
//Copyright (c) 2021 Andrei Vasilev
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
#include "Input.h"
#include "Renderer.h"
#include "Sound.h"
#include "Physics.h"
#include "DotSceneLoaderB.h"
#include "Overlay.h"
#include "AppState.h"
#include "Configurator.h"
#include "Engine.h"
#include "view_ptr.h"

namespace xio {

class Engine {
 public:
  Engine();
  virtual ~Engine();

  void Capture();
  void Pause();
  void Resume();
  void Clean();
  void Refresh();
  void Update(float time);
  void RenderOneFrame();

 private:
  view_ptr<InputSequencer> input_;
  std::unique_ptr<Configurator> conf_;
  std::unique_ptr<Renderer> renderer_;
  std::unique_ptr<Physics> physics_;
  std::unique_ptr<Sound> sound_;
  std::unique_ptr<Overlay> overlay_;
  std::unique_ptr<DotSceneLoaderB> loader_;
  std::vector<view_ptr<Component>> components_;
};

} //namespace
