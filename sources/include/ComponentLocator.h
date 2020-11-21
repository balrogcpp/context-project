//MIT License
//
//Copyright (c) 2020 Andrey Vasiliev
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

namespace xio {
class JsonConfigurator;
class YamlConfigurator;
class Renderer;
class Physics;
class Sound;
class Overlay;
class DotSceneLoaderB;
class InputSequencer;

class ComponentLocator {
 public:
//----------------------------------------------------------------------------------------------------------------------
  static void LocateComponents(YamlConfigurator *conf,
                        InputSequencer *input,
                        Renderer *renderer,
                        Physics *physics,
                        Sound *sounds,
                        Overlay *overlay,
                        DotSceneLoaderB *loader) {
    conf_ = conf;
    input_ = input;
    renderer_ = renderer;
    physics_ = physics;
    sound_ = sounds;
    overlay_ = overlay;
    loader_ = loader;
  }

 protected:
  inline static YamlConfigurator *conf_ = nullptr;
  inline static Renderer *renderer_ = nullptr;
  inline static Physics *physics_ = nullptr;
  inline static Sound *sound_ = nullptr;
  inline static Overlay *overlay_ = nullptr;
  inline static DotSceneLoaderB *loader_ = nullptr;
  inline static InputSequencer *input_ = nullptr;
};
}