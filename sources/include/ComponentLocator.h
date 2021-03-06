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

#include "view_ptr.h"

namespace xio {
class JsonConfigurator;
class Configurator;
class Renderer;
class Physics;
class Sound;
class Overlay;
class DotSceneLoaderB;
class InputSequencer;

class ComponentLocator {
 public:
//----------------------------------------------------------------------------------------------------------------------
  static void LocateComponents(view_ptr<Configurator> conf,
							   view_ptr<InputSequencer> input,
							   view_ptr<Renderer> renderer,
							   view_ptr<Physics> physics,
							   view_ptr<Sound> sounds,
							   view_ptr<Overlay> overlay,
							   view_ptr<DotSceneLoaderB> loader) {
	conf_ = conf;
	input_ = input;
	renderer_ = renderer;
	physics_ = physics;
	sound_ = sounds;
	overlay_ = overlay;
	loader_ = loader;
  }

 protected:
  inline static view_ptr<Configurator> conf_;
  inline static view_ptr<Renderer> renderer_;
  inline static view_ptr<Physics> physics_;
  inline static view_ptr<Sound> sound_;
  inline static view_ptr<Overlay> overlay_;
  inline static view_ptr<DotSceneLoaderB> loader_;
  inline static view_ptr<InputSequencer> input_;
};
}
