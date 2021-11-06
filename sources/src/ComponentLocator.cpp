// MIT License
//
// Copyright (c) 2021 Andrey Vasiliev
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "ComponentLocator.h"

#include "Engine.h"
#include "InputHandler.h"
#include "pcheader.h"

namespace glue {

//----------------------------------------------------------------------------------------------------------------------
InputHandler& GetIo() {
  static auto& io = *Engine::GetInstance().io_;
  return io;
}

//----------------------------------------------------------------------------------------------------------------------
Config& GetConf() {
  static auto& config = *Engine::GetInstance().config_;
  return config;
}

//----------------------------------------------------------------------------------------------------------------------
Engine& GetEngine() {
  static auto& engine = Engine::GetInstance();
  return engine;
}

//----------------------------------------------------------------------------------------------------------------------
RenderSystem& GetRS() {
  static auto& renderer = *Engine::GetInstance().rs_;
  return renderer;
}

//----------------------------------------------------------------------------------------------------------------------
Window& GetWindow() {
  static auto& window = GetRS().GetWindow();
  return window;
}

//----------------------------------------------------------------------------------------------------------------------
Compositor& GetCompositor() {
  static auto& compositor = GetRS().GetCompositor();
  return compositor;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsSystem& GetPhysics() {
  static auto& physics = *Engine::GetInstance().ps_;
  return physics;
}

//----------------------------------------------------------------------------------------------------------------------
AudioSystem& GetAudio() {
  static auto& audio = *Engine::GetInstance().as_;
  return audio;
}

//----------------------------------------------------------------------------------------------------------------------
Overlay& GetOverlay() {
  static auto& overlay = *Engine::GetInstance().overlay_;
  return overlay;
}

//----------------------------------------------------------------------------------------------------------------------
DotSceneLoaderB& GetLoader() {
  static auto& loader = *Engine::GetInstance().loader_;
  return loader;
}

}  // namespace glue
