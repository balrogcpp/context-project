//MIT License
//
//Copyright (c) 2021 Andrey Vasiliev
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

#include "pcheader.h"

#include "Engine.h"
#include "ComponentLocator.h"
#include "InputHandler.h"

namespace xio {

//----------------------------------------------------------------------------------------------------------------------
InputHandler& GetIo() {
  static auto &io = *Engine::GetInstance().io_;
  return io;
}

//----------------------------------------------------------------------------------------------------------------------
Configurator& GetConf() {
  static auto &config = *Engine::GetInstance().conf_;
  return config;
}

//----------------------------------------------------------------------------------------------------------------------
Engine& GetEngine() {
  static auto &engine = Engine::GetInstance();
  return engine;
}

//----------------------------------------------------------------------------------------------------------------------
Render& GetRender() {
  static auto &renderer = *Engine::GetInstance().renderer_;
  return renderer;
}

//----------------------------------------------------------------------------------------------------------------------
Window& GetWindow() {
  static auto &window = GetRender().GetWindow();
  return window;
}

//----------------------------------------------------------------------------------------------------------------------
Compositor& GetCompositor() {
  static auto &compositor = GetRender().GetCompositor();
  return compositor;
}

//----------------------------------------------------------------------------------------------------------------------
Physics& GetPhysics() {
  static auto &physics = *Engine::GetInstance().physics_;
  return physics;
}

//----------------------------------------------------------------------------------------------------------------------
Audio& GetAudio() {
  static auto &audio = *Engine::GetInstance().audio_;
  return audio;
}

//----------------------------------------------------------------------------------------------------------------------
Overlay& GetOverlay() {
  static auto &overlay = *Engine::GetInstance().overlay_;
  return overlay;
}

//----------------------------------------------------------------------------------------------------------------------
DotSceneLoaderB& GetLoader() {
  static auto &loader = *Engine::GetInstance().loader_;
  return loader;
}


} //namespace
