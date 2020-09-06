/*
MIT License

Copyright (c) 2020 Andrey Vasiliev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "Component.h"

namespace Ogre {
class RenderTarget;
class Texture;
class SceneNode;
}

namespace Gorilla {
class Silverback;
class Screen;
class Layer;
class Polygon;
class LineList;
class Caption;
class Rectangle;
class QuadList;
class MarkupText;
}

namespace xio {

class Overlay final : public Component {
 public:
  Overlay();
  ~Overlay();

  void Create() final;
  void Clear() final;
  void Update(float time) final {};

 private:
  Gorilla::Silverback *mSilverback = nullptr;
  Gorilla::Screen *mScreen = nullptr;
  Gorilla::Layer *layer = nullptr;
  Gorilla::Polygon *poly = nullptr;
  Gorilla::LineList *list = nullptr;
  Gorilla::Caption *caption = nullptr;
  Gorilla::Rectangle *rect = nullptr;
  Gorilla::QuadList *quads = nullptr;
  Gorilla::MarkupText *markup = nullptr;
};
}