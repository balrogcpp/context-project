//Cpp file for dummy context2_deps target
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

#include "pcheader.h"
#include "Field.h"
#include "PbrUtils.h"
using namespace Forests;

namespace xio {
//----------------------------------------------------------------------------------------------------------------------
Field::Field() = default;

Field::~Field() {}
//----------------------------------------------------------------------------------------------------------------------
void Field::Create() {
  auto *grass = new PagedGeometry(Ogre::Root::getSingleton().getSceneManager("Default")->getCamera("Default"), 50);
  grass->addDetailLevel<GrassPage>(100);//Draw grass up to 100
  auto *grassLoader = new GrassLoader(grass);
  grass->setPageLoader(grassLoader);
  if (heigh_func_)
    grassLoader->setHeightFunction([](float x, float z, void*){return Ogre::Real(heigh_func_(x, z) - 0.1);});
  UpdatePbrParams("GrassCustom");
  UpdatePbrShadowReceiver("GrassCustom");
  GrassLayer *layer = grassLoader->addLayer("GrassCustom");
  layer->setFadeTechnique(FADETECH_ALPHAGROW);
  layer->setRenderTechnique(GRASSTECH_CROSSQUADS);
  layer->setMaximumSize(1.0f, 1.0f);
  layer->setAnimationEnabled(true);
  layer->setSwayDistribution(10.0f);
  layer->setSwayLength(1.0f);
  layer->setSwaySpeed(0.5f);
  layer->setDensity(2.0f);
  layer->setMapBounds(TBounds(-100, -100, 100, 100));
  layer->setDensityMap("terrain2.png");
  layer->setColorMap("terrain2.png");
  grass->update();

//  auto *scene = Ogre::Root::getSingleton().getSceneManager("Default");
//  auto *trees = new Forests::PagedGeometry(scene->getCamera("Default"), 100);
//  trees->addDetailLevel<Forests::BatchPage>(100, 50);
//  auto *treeLoader = new Forests::TreeLoader2D(trees, TBounds(-200, -200, 200, 200));
//  if (heigh_func_)
//    treeLoader->setHeightFunction([](float x, float z, void*){return Ogre::Real(heigh_func_(x, z) - 0.1);});
//  trees->setPageLoader(treeLoader);
//  Ogre::Entity *fir1EntPtr = scene->createEntity("fir1", "fir05_30.mesh");
//  Ogre::Entity *fir2EntPtr = scene->createEntity("fir2", "fir06_30.mesh");
//  Ogre::Entity *fir3EntPtr = scene->createEntity("fir3", "fir14_25.mesh");
//
////Add trees
//  float x = 0, y = 0, z = 0, yaw, scale;
//  for (int i = 0; i < 50; i++) {
//    yaw = Ogre::Math::RangeRandom(0, 360);
//    if (Ogre::Math::RangeRandom(0, 1) <= 0.8f) {
//      x = Ogre::Math::RangeRandom(-200, 200);
//      z = Ogre::Math::RangeRandom(-200, 200);
//      if (x < -200) x = -200; else if (x > 200) x = 200;
//      if (z < -200) z = -200; else if (z > 200) z = 200;
//    }
//    else {
//      x = Ogre::Math::RangeRandom(-200, 200);
//      z = Ogre::Math::RangeRandom(-200, 200);
//    }
//    y = 0;
//    scale = Ogre::Math::RangeRandom(0.9f, 1.1f);
//    scale *= 0.2;
//    Ogre::Quaternion quat;
//    quat.FromAngleAxis(Ogre::Degree(yaw), Ogre::Vector3::UNIT_Y);
//
//    if (i % 2 == 0) {
//      treeLoader->addTree(fir1EntPtr, Ogre::Vector3(x, y, z), Ogre::Degree(yaw), scale);
//    }
//    else if (i % 3 == 0) {
//      treeLoader->addTree(fir2EntPtr, Ogre::Vector3(x, y, z), Ogre::Degree(yaw), scale);
//    }
//    else {
//      treeLoader->addTree(fir3EntPtr, Ogre::Vector3(x, y, z), Ogre::Degree(yaw), scale);
//    }
//  }
//  trees->update();
}
}