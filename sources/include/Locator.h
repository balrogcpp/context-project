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

class Locator {
 public:
//----------------------------------------------------------------------------------------------------------------------
  void LocateComponents(YamlConfigurator *conf,
                        InputSequencer *io,
                        Renderer *renderer,
                        Physics *physics,
                        Sound *sounds,
                        Overlay *overlay,
                        DotSceneLoaderB *loader) {
    conf_ = conf;
    io_ = io;
    renderer_ = renderer;
    physics_ = physics;
    sound_ = sounds;
    overlay_ = overlay;
    loader_ = loader;
  }

 protected:
  static YamlConfigurator *conf_;
  static Renderer *renderer_ ;
  static Physics *physics_;
  static Sound *sound_;
  static Overlay *overlay_;
  static DotSceneLoaderB *loader_;
  static InputSequencer *io_;
};
}