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

#include "pcheader.h"

#include "Overlay.h"
#include "Gorilla.h"

using namespace Gorilla;
using namespace std;

namespace xio {
Overlay::Overlay() {
  atlas_ = make_unique<Silverback>();
  atlas_->loadAtlas("dejavu");
  auto *viewport = Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default")->getViewport();
  screen_ = atlas_->createScreen(viewport, "dejavu");
  layer_ = screen_->createLayer(0);
  Ogre::Real vpW = screen_->getWidth(), vpH = screen_->getHeight();

  // Init our drawing layer
  layer_ = screen_->createLayer(0);
  rect_ = layer_->createRectangle(0, 0, vpW, vpH);
  rect_->background_colour(rgb(0, 0, 0, 0));
  caption_ = layer_->createCaption(24, vpW - 55, 66, "");
  caption_->width(0);
  caption_->align(TextAlign_Right);

  console_ = make_unique<OgreConsole>();
  console_->init(screen_);
  console_->setVisible(false);
}

//----------------------------------------------------------------------------------------------------------------------
Overlay::~Overlay() {

}

//----------------------------------------------------------------------------------------------------------------------
void Overlay::Update(float time) {
  caption_->text((1.0 / time));
}
//----------------------------------------------------------------------------------------------------------------------
void Overlay::Text(const string &str) {
  caption_->text(str);
}
//----------------------------------------------------------------------------------------------------------------------
void Overlay::Show() {
  screen_->show();
}
//----------------------------------------------------------------------------------------------------------------------
void Overlay::Hide() {
  screen_->hide();
}
}