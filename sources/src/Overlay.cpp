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

#include "pcheader.h"

#include "Overlay.h"
#include "Gorilla.h"
#include "Application.h"

using namespace Gorilla;

namespace xio {
Overlay::Overlay() {}
Overlay::~Overlay() {}

//----------------------------------------------------------------------------------------------------------------------
void Overlay::Loop(float time) {
  caption_->text(std::to_string(1.0 / time));
}
//----------------------------------------------------------------------------------------------------------------------
void Overlay::Create() {
  mSilverback = new Silverback();
  mSilverback->loadAtlas("dejavu");
  auto *viewport = Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default")->getViewport();
  mScreen = mSilverback->createScreen(viewport, "dejavu");
  layer = mScreen->createLayer(0);
  Ogre::Real vpW = mScreen->getWidth(), vpH = mScreen->getHeight();

  // Create our drawing layer
  layer = mScreen->createLayer(0);
  rect = layer->createRectangle(0, 0, vpW, vpH);
  rect->background_colour(rgb(0, 0, 0, 0));
  caption_ = layer->createCaption(24, vpW - 55, 66, "");
  caption_->width(0);
  caption_->align(TextAlign_Right);

//  auto *parent = Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default")->getParentSceneNode();
//  auto *mPowerPanel = new D3Panel(mSilverback, Ogre::Root::getSingleton().getSceneManager("Default"), Ogre::Vector2(4,1), parent);
//  mPowerPanel->mNode->setPosition(Ogre::Vector3(0,1.5f,0));
//  Gorilla::Caption* caption = mPowerPanel->makeCaption(0,4, "Power Level");
//  caption->width(400);
//  caption->align(Gorilla::TextAlign_Centre);
//  auto *mPowerValueBackground = mPowerPanel->mGUILayer->createRectangle(10,35,380,10);
//  mPowerValueBackground->background_colour(Gorilla::rgb(255,255,255,100));
//  auto *mPowerValue = mPowerPanel->mGUILayer->createRectangle(10,35,200,10);
//  mPowerValue->background_gradient(Gorilla::Gradient_NorthSouth, Gorilla::rgb(255,255,255,200), Gorilla::rgb(64,64,64,200));
//  mPowerPanel->makeButton(10, 65, "-");
//  mPowerPanel->makeButton(84, 65, "+");
}
//----------------------------------------------------------------------------------------------------------------------
void Overlay::Text(const std::string &str) {
  caption_->text(str);
}
//----------------------------------------------------------------------------------------------------------------------
void Overlay::Clear() {
  mSilverback->destroyScreen(mScreen);
}
}